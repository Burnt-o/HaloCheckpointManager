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

namespace HCM3
{
    public class DataPointers
    {
        #region Properties
        private Dictionary<string, Dictionary<string, Object>> PointerData { get; set; } = new()
        {
        };



        #endregion

        #region Constructor
        public DataPointers()
        {
            PointerData = new();
        }

        #endregion



        public bool LoadPointerDataFromGit(out string exceptionString, out string? HighestSupportedMCCVersion)
        {
            HighestSupportedMCCVersion = null;
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
                        HighestSupportedMCCVersion = e.Value == null || e.Value == "" ? null : e.Value;
                        Trace.WriteLine("Loaded HighestSupportedMCCVersion, value: " + HighestSupportedMCCVersion);
                    }
                    else if (e.Name == "Version")
                    {
                        Trace.WriteLine("1Version: " + e.Attribute("Version")?.Value);
                        string? pointerVersion = e.Attribute("Version")?.Value == null ? null : e.Attribute("Version")?.Value;
                        
                        foreach (XElement entry in e.Elements())
                        {
                            string? pointerName = null;
                            object? objectToStore = null;

                            if (entry.Attribute("Type")?.Value == "int")
                            {
                                Trace.WriteLine("Processing int entry");
                                pointerName = entry.Element("Name") == null ? null : entry.Element("Name")?.Value;
                                int? pointerOffset = entry.Element("Offset") == null ? null : ParseHexNumber(entry.Element("Offset")?.Value);

                                if (pointerOffset.HasValue)
                                {
                                    objectToStore = (int)pointerOffset;
                                }

                            }
                            if (entry.Attribute("Type")?.Value == "PreserveLocation[]")
                            {
                                Trace.WriteLine("Processing PreserveLocation[] entry");
                                pointerName = entry.Element("Name") == null ? null : entry.Element("Name")?.Value;
                                PreserveLocation?[]? preserveLocations = entry.Element("PreserveLocations") == null ? null : entry.Element("PreserveLocations")?.Elements().Select(x => ParseLocation(x)).ToArray();

                                if (preserveLocations != null && preserveLocations.Any() && preserveLocations[0] != null)
                                {
                                    objectToStore = preserveLocations;
                                }
                            }
                            else if (entry.Attribute("Type")?.Value == "ReadWrite.Pointer")
                            {
                                Trace.WriteLine("Processing ReadWrite.Pointer entry");
                                // Read the data from the Pointer element (if the data exists)
                                pointerName = entry.Element("Name") == null ? null : entry.Element("Name")?.Value;
                                string? pointerModule = entry.Element("Module") == null ? null : entry.Element("Module")?.Value;
                                int[]? pointerOffsets = entry.Element("Offsets") == null ? null : entry.Element("Offsets")?.Elements().Select(x => ParseHexNumber(x.Value)).ToArray();


                                // Check that all the data actually exists and make the pointer
                                if (pointerModule != null && pointerOffsets != null)
                                {
                                    objectToStore = new ReadWrite.Pointer(pointerModule, pointerOffsets);
                                }
                            }

                            if (pointerName != null && pointerVersion != null && objectToStore != null)
                            {
                                // Load the object into the Pointers dictionary, with pointerName and pointerVersion as the keys    
                                Dictionary<string, Object> versionDictionary = new();
                                versionDictionary.Add(pointerVersion, objectToStore);
                                PointerData.Add(pointerName, versionDictionary);
                                Trace.WriteLine("Added new object to pointer dictionary, name: " + pointerName + ", version: " + pointerVersion + ", type: " + objectToStore.GetType().ToString());
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
                return s.StartsWith("0x") ? Convert.ToInt32(s.Substring(2), 16) : Convert.ToInt32(s);
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
