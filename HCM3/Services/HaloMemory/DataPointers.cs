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
using HCM3.Model.CheckpointModels;
using HCM3.Model;

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
                const bool useOnline = false;
                string xml;
                if (useOnline)
                {
                    // Download the xml from git
                    string url = "https://raw.githubusercontent.com/Burnt-o/HaloCheckpointManager/HCM2/HCM3/PointerData.xml";
                    System.Net.WebClient client = new System.Net.WebClient();
                    xml = client.DownloadString(url);
                }
                else
                {
                    // Grab it from local repo for testing so that I don't have to push it to git everytime
                    xml = File.ReadAllText(@"C:\Users\mauri\source\repos\HaloCheckpointManager\HCM3\PointerData.xml");
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
                                    byte?[]? entryArray = entry.Element("Array") == null ? null : entry.Element("Array")?.Elements().Select(x => (byte?)ParseHexNumber(x.Value)).ToArray();

                                    if (entryArray == null) throw new Exception("byte[]: entryArray was null");
                                    if (!entryArray.Any()) throw new Exception("byte[]: entryArray didn't have any elements");
                                    if (!entryArray.All(y => y != null)) throw new Exception("byte[]: entryArray had some null elements");

                                    entryToStore = (byte[])entryArray.Cast<byte>().ToArray();

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
