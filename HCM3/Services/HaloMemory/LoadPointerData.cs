using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.IO;
using System.Xml.Linq;
using BurntMemory;
using HCM3.Helpers;
using HCM3.Services.Trainer;

namespace HCM3.Services
{
    public partial class DataPointersService
    {

        public void LoadPointerDataFromSource2(out string failedReads)
        {
            string? xml = null;
            string localPointerDataPath = Directory.GetCurrentDirectory() + "\\PointerData.xml";
            failedReads = "";

            // if on developer machine, user latest local version
            if (localPointerDataPath.Contains(@"mauri\source\repos") && File.Exists(@"C:\Users\mauri\source\repos\HaloCheckpointManager\HCM3\PointerData.xml"))
            {
                Trace.WriteLine("grabbing debug xml from repo");
                xml = File.ReadAllText(@"C:\Users\mauri\source\repos\HaloCheckpointManager\HCM3\PointerData.xml");
            }
            else
            {
                // Download the xml from git
                try
                {
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
                    if (File.Exists(localPointerDataPath))
                    {
                        Trace.WriteLine("grabbing local xml data");
                        xml = File.ReadAllText(localPointerDataPath);
                    }
                }
            }
            if (xml == null || !xml.Any()) throw new Exception("Couldn't find pointerdata!");

            // Deserialise
            XDocument doc = XDocument.Parse(xml);

            if (doc.Root == null) throw new Exception("Something went wrong parsing the Pointer data xml file; 'doc.Root' was null.");

            foreach (XElement entry in doc.Root.Elements())
            {
                try
                {
                    object? entryObject = null;
                    string? entryName = null;
                    string? entryVersion = null;

                    if (entry.Name == "HighestSupportedVersion")
                    {
                        this.HighestSupportedMCCVersion = entry.Value == null || entry.Value == "" ? null : entry.Value;
                        Trace.WriteLine("Loaded HighestSupportedMCCVersion, value: " + this.HighestSupportedMCCVersion);
                    }
                    else if (entry.Name == "LatestHCMVersion")
                    {
                        this.LatestHCMVersion = entry.Value == null || entry.Value == "" ? null : entry.Value;
                        Trace.WriteLine("Loaded LatestHCMVersion, value: " + this.LatestHCMVersion);
                    }
                    else if (entry.Name == "ObsoleteHCMVersions")
                    {
                        if (entry.Value != null && entry.Value != "")
                        {
                            foreach (XElement ver in entry.Elements().Where(x => x.Name == "Entry"))
                            { 
                            this.ObsoleteHCMVersions.Add(ver.Value);
                            }
                        }
                    }
                    else if (entry.Name == "Entry")
                    {
                        string entryType = entry.Attribute("Type").Value;
                        entryName = entry.Element("Name") == null ? null : entry.Element("Name")?.Value;
                        foreach (XElement VersionEntry in entry.Elements().Where(x => x.Name == "Version"))
                        {
                            entryVersion = VersionEntry.Attribute("Version").Value;
                            entryObject = ParseObject(VersionEntry, entryType);
                            StoreObject(entryName, entryVersion, entryObject);
                        }
                    }

                }
                catch (Exception ex)
                { 
                    failedReads = failedReads + "\n" + "Error processing an entry in pointerdata, " + ex.Message + ", entry.name: " + entry.Element("Name")?.Value + ", entry.Value: " + entry.Value;
                Trace.WriteLine("Error processing an entry in pointerdata, " + ex.Message);
                    continue;
                }

            }


        }
        public object ParseObject(XElement entry, string type)
        {
            object? returnObject = null;
            switch (type)
            {
                case "int":
                    returnObject = ParseHexNumberInt(entry.Element("Offset")?.Value);
                    break;

                case "uint":
                    returnObject = ParseHexNumberUint(entry.Element("Offset")?.Value);
                    break;

                case "PreserveLocation[]":
                    PreserveLocation[] preserveLocationArray = entry.Element("Array")?.Elements().Select(x => ParseLocation(x)).ToArray();
                    if (!preserveLocationArray.Any()) throw new Exception("Emptry PreserveLocation array");
                    returnObject = preserveLocationArray;
                    break;

                case "ReadWrite.Pointer":
                    returnObject = ParsePointer(entry);
                    break;

                case "ReadWrite.Pointer[]":
                    ReadWrite.Pointer?[]? pointerArray = entry.Element("Array")?.Elements().Select(x => ParsePointer(x)).ToArray();
                    if (!pointerArray.Any()) throw new Exception("Emptry Pointer Array");
                    returnObject = pointerArray;
                    break;

                case "int[]":
                    int[] intArray = entry.Element("Array")?.Elements().Select(x => ParseHexNumberInt(x.Value)).ToArray();
                    if (!intArray.Any()) throw new Exception("Emptry intArray");
                    returnObject = intArray;
                    break;

                case "byte[]":
                    byte[] byteArray = StringToByteArray(entry.Element("Array")?.Value);
                    if (!byteArray.Any()) throw new Exception("Emptry byteArray");
                    returnObject = byteArray;
                    break;

                case "string":
                    returnObject = entry.Element("Value")?.Value;
                    break;

                case "DetourInfoObject":
                    returnObject = ParseDetourInfoObject(entry);
                    break;

                case "PatchInfo":
                    returnObject = ParsePatchInfoObject(entry);
                    break;

                case "Dictionary<string, bool>":
                    Dictionary<string,bool> entryDictionary = new();
                    foreach (XElement element in entry.Elements())
                    {
                        entryDictionary.Add(element.Name.ToString(), Convert.ToBoolean(element.Value));
                    }
                    returnObject = entryDictionary;
                break;

                case "List<string>":
                    string? mergedString = entry.Element("ListString").Value;
                    if (!mergedString.Any()) throw new Exception("Empty mergedString in list<string>");
                    List<string> unmerged = mergedString.Split(", ").ToList();
                    returnObject = unmerged;
                    break;


                default:
                    throw new ArgumentException("Didn't recognise type of datapointer entry, " + type);
            }
            if (returnObject == null) throw new Exception("Failed to parse returnObject but not sure why");
            return returnObject;
        }

        public void StoreObject(string entryName, string entryVersion, object entryObject)
        {
            if (entryObject == null) throw new Exception("entryObject was somehow null at StoreObject");
            if (entryObject.GetType() == typeof(System.Xml.Linq.XElement)) throw new Exception("Accidentally parsed XElement");



            if (PointerData.ContainsKey(entryName))
            {
                PointerData[entryName].Add(entryVersion, entryObject);
            }
            else
            {
                Dictionary<string, Object> versionDictionary = new();
                versionDictionary.Add(entryVersion, entryObject);
                PointerData.Add(entryName, versionDictionary);
            }


            Trace.WriteLine("Added new object to pointer dictionary, name: " + entryName + ", version: " + entryVersion + ", type: " + entryObject.GetType().ToString());
        }






        PreserveLocation? ParseLocation(XElement? location)
        {
            if (location == null) return null;
            //Trace.WriteLine("location: " + location.ToString());
            try
            {

                int? Offset = ParseHexNumberInt(location.Element("Offset")?.Value);
                int? Length = ParseHexNumberInt(location.Element("Length")?.Value);

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


        int ParseHexNumberInt(string? s)
        {
            if (s == null)
                throw new Exception("int was null");

            if (s == "0x0" || s == "0") return 0;

            int sign = 1;
            if (s.StartsWith("-"))
            {
                sign = -1;
                s = s.Substring(1);
            }


            return s.StartsWith("0x") ? Convert.ToInt32(s.Substring(2), 16) * sign : Convert.ToInt32(s) * sign;
        }
        uint ParseHexNumberUint(string? s)
        {
            if (s == null)
                throw new Exception("uint was null");

            if (s == "0x0" || s == "0") return 0;

            return s.StartsWith("0x") ? Convert.ToUInt32(s.Substring(2), 16) : Convert.ToUInt32(s);
        }


        ReadWrite.Pointer ParsePointer(XElement entry)
        {
            string? pointerModule = entry.Element("Module") == null ? null : entry.Element("Module")?.Value;
            int[]? pointerOffsets = entry.Element("Offsets") == null ? null : entry.Element("Offsets")?.Elements().Select(x => ParseHexNumberInt(x.Value)).ToArray();

            if (pointerModule == null) throw new Exception("ReadWrite.Pointer: pointerModule was null, name: " + entry.Element("Name")?.Value);
            if (pointerOffsets == null) throw new Exception("ReadWrite.Pointer: pointerOffsets was null, name: " + entry.Element("Name")?.Value);

            return new ReadWrite.Pointer(pointerModule, pointerOffsets);
        }

        DetourInfoObject ParseDetourInfoObject(XElement entry)
        {
            ReadWrite.Pointer OriginalCodeLocation = ParsePointer(entry.Element("OriginalCodeLocation"));
            int SizeToAlloc = ParseHexNumberInt(entry.Element("SizeToAlloc")?.Value);
            string DetourCodeASM = entry.Element("DetourCodeASM")?.Value ?? throw new Exception("failed reading entry DetourCodeASM");
            string HookCodeASM = entry.Element("HookCodeASM")?.Value ?? throw new Exception("failed reading entry HookCodeASM");

            byte[] OriginalCodeBytes = entry.Element("OriginalCodeBytes") == null ? throw new Exception("failed reading entry OriginalCodeBytes") : StringToByteArray(entry.Element("OriginalCodeBytes")?.Value);
            if (!OriginalCodeBytes.Any()) throw new Exception("failed reading entry for detourinfoobject, null values in OriginalCodeBytes");

            // Parse symbols in symbolPointers xml
            Dictionary<string, ReadWrite.Pointer> SymbolPointers = new();

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
            return new DetourInfoObject(OriginalCodeLocation, OriginalCodeBytes, SizeToAlloc, DetourCodeASM, HookCodeASM, SymbolPointers);

        }

        PatchInfo ParsePatchInfoObject(XElement entry)
        {

            ReadWrite.Pointer OriginalCodeLocation = ParsePointer(entry.Element("OriginalCodeLocation"));
            byte[] OriginalCodeBytes = entry.Element("OriginalCodeBytes") == null ? throw new Exception("failed reading entry OriginalCodeBytes") : StringToByteArray(entry.Element("OriginalCodeBytes")?.Value);
            if (!OriginalCodeBytes.Any()) throw new Exception("failed reading entry for patchinfo, null values in OriginalCodeBytes");

            byte[] PatchedCodeBytes = entry.Element("PatchedCodeBytes") == null ? throw new Exception("failed reading entry PatchedCodeBytes") : StringToByteArray(entry.Element("PatchedCodeBytes")?.Value);
            if (!PatchedCodeBytes.Any()) throw new Exception("failed reading entry for patchinfo, null values in PatchedCodeBytes");

            return new PatchInfo(OriginalCodeLocation, OriginalCodeBytes, PatchedCodeBytes);

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
}
