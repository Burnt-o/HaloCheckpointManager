using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using System.Xml.Linq;
using System.Xml.XPath;
using System.Diagnostics;
using System.IO;
using System.Xml;
using HCM3.Models;
using HCM3.Helpers;
using Keystone;
using HCM3.Services.Trainer;

namespace HCM3.Services
{
    public class DataPointersService
    {
        #region Properties
        private Dictionary<string, Dictionary<string, Object>> PointerData { get; set; } = new()
        {
        };



        #endregion

        #region Constructor
        public DataPointersService()
        {
            PointerData = new();
            HighestSupportedMCCVersion = null;
        }

        #endregion

        public string? HighestSupportedMCCVersion { get; set; }



        public bool LoadPointerDataFromGit(out string exceptionString)
        {

            exceptionString = "";
            try
            {

                string? xml = null;
                string localPointerDataPath = Directory.GetCurrentDirectory() + "\\PointerData.xml";


                // Download the xml from git
                try
                {
                    throw new Exception("for debugging we'll use local file");
                    string url = "https://raw.githubusercontent.com/Burnt-o/HaloCheckpointManager/HCM2/HCM3/PointerData.xml";
                    System.Net.WebClient client = new System.Net.WebClient();
                    xml = client.DownloadString(url);

                    //Write the contents to local PointerData.xml for offline use
                    if (File.Exists(localPointerDataPath)) File.Delete(localPointerDataPath);
                    File.WriteAllText(localPointerDataPath, xml);
                }
                catch
                {
                    // Couldn't grab online data, try offline backup
                    Trace.WriteLine("Couldn't find online xml data, trying local backup");
                    if (File.Exists(@"C:\Users\mauri\source\repos\HaloCheckpointManager\HCM3\PointerData.xml"))
                    {
                        Trace.WriteLine("grabbing debug xml from repo");
                        xml = File.ReadAllText(@"C:\Users\mauri\source\repos\HaloCheckpointManager\HCM3\PointerData.xml");
                    }
                    else if (File.Exists(localPointerDataPath))
                    {
                        Trace.WriteLine("grabbing local xml data");
                        xml = File.ReadAllText(localPointerDataPath);
                    }
                }


                if (xml == null) 
                {
                    Trace.WriteLine("couldn't find xml data onlien or offline!");
                    return false;
                }

                // Deserialise
                XDocument doc = XDocument.Parse(xml);

                if (doc.Root == null)
                {
                    exceptionString = "Something went wrong parsing the Pointer data xml file; 'doc.Root' was null.";
                    return false;
                }

                foreach (XElement e in doc.Root.Elements())
                {
                    if (e.Name == "HighestSupportedVersion")
                    {
                        this.HighestSupportedMCCVersion = e.Value == null || e.Value == "" ? null : e.Value;
                        Trace.WriteLine("Loaded HighestSupportedMCCVersion, value: " + this.HighestSupportedMCCVersion);
                    }
                    else if (e.Name == "Version")
                    {
                        Trace.WriteLine("1Version: " + e.Attribute("Version")?.Value);
                        string? entryVersion = e.Attribute("Version")?.Value == null ? null : e.Attribute("Version")?.Value;
                        
                        foreach (XElement entry in e.Elements())
                        {
                            string? entryName = null;
                            object? entryToStore = null;
                            try
                            {
                                if (entry.Attribute("Type")?.Value == "int")
                                {
                                    Trace.WriteLine("Processing int entry");
                                    entryName = entry.Element("Name") == null ? null : entry.Element("Name")?.Value;
                                    int? pointerOffset = entry.Element("Offset") == null ? null : ParseHexNumber(entry.Element("Offset")?.Value);

                                    if (!pointerOffset.HasValue) throw new Exception("int: didn't have value");

                                    entryToStore = (int)pointerOffset;

                                }
                                if (entry.Attribute("Type")?.Value == "PreserveLocation[]")
                                {
                                    Trace.WriteLine("Processing PreserveLocation[] entry");
                                    entryName = entry.Element("Name") == null ? null : entry.Element("Name")?.Value;
                                    PreserveLocation?[]? entryArray = entry.Element("Array") == null ? null : entry.Element("Array")?.Elements().Select(x => ParseLocation(x)).ToArray();

                                    if (entryArray == null) throw new Exception("PreserveLocation[]: entryArray was null");
                                    if (!entryArray.Any()) throw new Exception("PreserveLocation[]: entryArray didn't have any elements");
                                    if (!entryArray.All(y => y != null)) throw new Exception("PreserveLocation[]: entryArray had some null elements");

                                    entryToStore = (PreserveLocation[])entryArray.Cast<PreserveLocation>().ToArray(); ;
                                }
                                else if (entry.Attribute("Type")?.Value == "ReadWrite.Pointer")
                                {
                                    Trace.WriteLine("Processing ReadWrite.Pointer entry");
                                    // Read the data from the Pointer element (if the data exists)
                                    entryName = entry.Element("Name") == null ? null : entry.Element("Name")?.Value;
                                    entryToStore = ParsePointer(entry);
                                }
                                else if (entry.Attribute("Type")?.Value == "ReadWrite.Pointer[]")
                                {
                                    Trace.WriteLine("Processing ReadWrite.Pointer[] entry");
                                    // Read the data from the Pointer element (if the data exists)
                                    entryName = entry.Element("Name") == null ? null : entry.Element("Name")?.Value;
                                    ReadWrite.Pointer?[]? entryArray = entry.Element("Array") == null ? null : entry.Element("Array")?.Elements().Select(x => ParsePointer(x)).ToArray();

                                    if (entryArray == null) throw new Exception("ReadWrite.Pointer[]: entryArray was null");
                                    if (!entryArray.Any()) throw new Exception("ReadWrite.Pointer[]: entryArray didn't have any elements");
                                    if (!entryArray.All(y => y != null)) throw new Exception("ReadWrite.Pointer[]: entryArray had some null elements");
                                    entryToStore = (ReadWrite.Pointer[])entryArray.Cast<ReadWrite.Pointer>().ToArray();

                                }
                                else if (entry.Attribute("Type")?.Value == "int[]")
                                {
                                    Trace.WriteLine("Processing int[] entry");
                                    // Read the data from the Pointer element (if the data exists)
                                    entryName = entry.Element("Name") == null ? null : entry.Element("Name")?.Value;
                                    int?[]? entryArray = entry.Element("Array") == null ? null : entry.Element("Array")?.Elements().Select(x => (int?)ParseHexNumber(x.Value)).ToArray();

                                    if (entryArray == null) throw new Exception("int[]: entryArray was null");
                                    if (!entryArray.Any()) throw new Exception("int[]: entryArray didn't have any elements");
                                    if (!entryArray.All(y => y != null)) throw new Exception("int[]: entryArray had some null elements");

                                    entryToStore = (int[])entryArray.Cast<int>().ToArray(); ;

                                }
                                else if (entry.Attribute("Type")?.Value == "byte[]")
                                {
                                    Trace.WriteLine("Processing int[] entry");
                                    // Read the data from the Pointer element (if the data exists)
                                    entryName = entry.Element("Name") == null ? null : entry.Element("Name")?.Value;
                                    byte[]? entryArray = entry.Element("Array") == null ? null : StringToByteArray(entry.Element("Array")?.Value);

                                    if (entryArray == null) throw new Exception("byte[]: entryArray was null");
                                    if (!entryArray.Any()) throw new Exception("byte[]: entryArray didn't have any elements");

                                    entryToStore = (byte[])entryArray;

                                }
                                else if (entry.Attribute("Type")?.Value == "string")
                                {
                                    Trace.WriteLine("Processing string entry");
                                    // Read the data from the Pointer element (if the data exists)
                                    entryName = entry.Element("Name") == null ? null : entry.Element("Name")?.Value;
                                    entryToStore = entry.Element("Value") == null ? null : (string)entry.Element("Value")?.Value;
                                }
                                else if (entry.Attribute("Type")?.Value == "DetourInfoObject")
                                {
                                    Trace.WriteLine("Processing DetourInfoObject entry");
                                    // Read the data from the Pointer element (if the data exists)
                                    entryName = entry.Element("Name")?.Value ?? throw new Exception("failed reading entry name"); 
                                    ReadWrite.Pointer? OriginalCodeLocation = entry.Element("OriginalCodeLocation") == null ? throw new Exception("failed reading entry OGL") : ParsePointer(entry.Element("OriginalCodeLocation"));
                                    int? SizeToAlloc = entry.Element("SizeToAlloc") == null ? throw new Exception("failed reading entry SizeToAlloc") : ParseHexNumber(entry.Element("SizeToAlloc")?.Value);
                                    string? DetourCodeASM = entry.Element("DetourCodeASM")?.Value ?? throw new Exception("failed reading entry DetourCodeASM");
                                    string? HookCodeASM = entry.Element("HookCodeASM")?.Value ?? throw new Exception("failed reading entry HookCodeASM");

                                    byte[]? OriginalCodeBytes = entry.Element("OriginalCodeBytes") == null ? throw new Exception("failed reading entry OriginalCodeBytes") : StringToByteArray(entry.Element("OriginalCodeBytes")?.Value);

                                    if (OriginalCodeLocation == null || SizeToAlloc == null || DetourCodeASM == null || HookCodeASM == null || OriginalCodeBytes == null || !OriginalCodeBytes.Any()) throw new Exception("failed reading entry for detourinfoobject, null values");

                                    // Parse symbols in symbolPointers xml
                                    Dictionary<string, ReadWrite.Pointer> SymbolPointers = new();
                                    Trace.WriteLine("EHH2");
                                    if (entry.Element("SymbolPointers") != null && entry.Element("SymbolPointers").HasElements)
                                    {
                                        foreach (XElement Pointer in entry.Element("SymbolPointers").Elements())
                                        {
     
                                            ReadWrite.Pointer? newPointer = ParsePointer(Pointer);
                                            string? symbolName = Pointer.Element("Symbol")?.Value ?? throw new Exception("failed reading symbol for symbolPointer");
                                            if (newPointer == null || symbolName == null) throw new Exception("failed reading symbolpointer");
                                            SymbolPointers.Add(symbolName, newPointer);

                                        }
                                    }
                                    // Parse automatic symbol $returnControl
                                    // returnControl is OriginalCodeLocation + the number of bytes in Original Code bytes
                                    SymbolPointers.Add("$returnControl", OriginalCodeLocation + OriginalCodeBytes.Length);
                                    entryToStore = new DetourInfoObject(OriginalCodeLocation, OriginalCodeBytes, SizeToAlloc.Value, DetourCodeASM, HookCodeASM, SymbolPointers);
                                }


                                if (entryName == null) throw new Exception("entryName was null, type: " + entry.Attribute("Type")?.Value);
                                if (entryVersion == null) throw new Exception("entryVersion was null, type: " + entry.Attribute("Type")?.Value + ", name: " + entryName);
                                if (entryToStore == null) throw new Exception("entryToStore was null, type: " + entry.Attribute("Type")?.Value + ", name: " + entryName);


                                    // Load the object into the Pointers dictionary, with pointerName and pointerVersion as the keys    
                                    Dictionary<string, Object> versionDictionary = new();
                                    versionDictionary.Add(entryVersion, entryToStore);
                                    PointerData.Add(entryName, versionDictionary);
                                    Trace.WriteLine("Added new object to pointer dictionary, name: " + entryName + ", version: " + entryVersion + ", type: " + entryToStore.GetType().ToString());
                               
                            }
                            catch (Exception ex)
                            {
                                Trace.WriteLine("Error processing entry: " + ex.Message);
                            }

                        }


                    }


                }

            }
            catch (Exception ex)
            {
                exceptionString = "HCM failed to download MCC version offset data. Check your internet connection, " +
                    "\\nor double check that HCM has the correct permissions."
                    + "\\nError: " + ex.Message;
                return false;
            }
            return true;


            int ParseHexNumber(string? s)
            {
                if (s == null)
                    throw new Exception("int was null");
                return s.StartsWith("0x") ? Convert.ToInt32(s.Substring(2), 16) : Convert.ToInt32(s);
            }


            ReadWrite.Pointer ParsePointer(XElement entry)
            {
                string? pointerModule = entry.Element("Module") == null ? null : entry.Element("Module")?.Value;
                int[]? pointerOffsets = entry.Element("Offsets") == null ? null : entry.Element("Offsets")?.Elements().Select(x => ParseHexNumber(x.Value)).ToArray();

                if (pointerModule == null) throw new Exception("ReadWrite.Pointer: pointerModule was null, name: " + entry.Element("Name")?.Value);
                if (pointerOffsets == null) throw new Exception("ReadWrite.Pointer: pointerOffsets was null, name: " + entry.Element("Name")?.Value);

                return new ReadWrite.Pointer(pointerModule, pointerOffsets);



            }

            PreserveLocation? ParseLocation(XElement? location)
            {
                Trace.WriteLine("got here at least");
                if (location == null) return null;
                Trace.WriteLine("location: " + location.ToString());
                try
                {

                    int? Offset = ParseHexNumber(location.Element("Offset")?.Value);
                    int? Length = ParseHexNumber(location.Element("Length")?.Value);

                    if (Offset == null || Length == null)
                    {
                        Trace.WriteLine("offset or length was null when parsing location");
                        return null;
                    }

                    return new PreserveLocation((int)Offset, (int)Length);
                }
                catch
                {
                    return null;
                }


            }

            string ByteArrayToString(byte[] byteArray)
            {
                string ByteArrayToString = "";
                for (int i = 0; i < byteArray.Length; i++)
                {
                    ByteArrayToString = ByteArrayToString + "0x" + byteArray[i].ToString("X2") + ",";
                }

                Trace.WriteLine(ByteArrayToString);
                ByteArrayToString = ByteArrayToString.Remove(ByteArrayToString.Length - 1);
                Trace.WriteLine(ByteArrayToString);
                return ByteArrayToString;
            }

            byte[] StringToByteArray(string str)
            {

                str = str.Replace("0x", "");
                str = str.Replace(" ", "");
                string[] splitstr = str.Split(',');
                byte[] byteArray = new byte[splitstr.Length];
                for (int i = 0; i < splitstr.Length; i++)
                {
                    byteArray[i] = Convert.ToByte(splitstr[i], 16);
                }
                return byteArray;
            }

        }

        public object? GetPointer(string? pointerName, string? pointerVersion)
        {
            if (pointerName == null || pointerVersion == null)
            {
                return null;
            }

            bool success = PointerData.TryGetValue(pointerName, out Dictionary<string, object>? pointerVersionDictionary);
                if (!success || pointerVersionDictionary == null)
                {
                    return null;
                }

                success = pointerVersionDictionary.TryGetValue(pointerVersion, out object? pointer);
                if (!success || pointer == null)
                {
                    return null;
                }

                return (object)pointer;
        }

        



    }
}
