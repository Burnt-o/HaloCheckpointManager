using Serilog;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Reflection;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Animation;
using System.Xml.Linq;

namespace HCMExternal.Services.DataPointersServiceNS
{

    /// <summary>
    /// Grabs the latest data from the HCM github repository about how to decode each checkpoint file per MCC version, 
    /// as well as some housekeeping stuff like what the latest version of HCM is, which versions are obsolete, and what the highest supported version of MCC is.
    /// Data is stored in a dictionary for consumers to request as needed.
    /// </summary>
    // TODO: split this into CheckpointDecoderDataService, HCMVersioningService, and a static class for grabbing/parsing the data (Can't think of a good name.. LatestDataLoader? GitDataPuller? CurrentDataDownloader?)
    public class DataPointersService
    {

        private Dictionary<string, Dictionary<string, object>> PointerData { get; set; } = new()
        {
        };


        public DataPointersService()
        {
            PointerData = new();
            HighestSupportedMCCVersion = null;

        }


        public string? HighestSupportedMCCVersion { get; set; }
        public string? LatestHCMVersion { get; set; }
        public List<string> LatestHCMVersions = new();
        public List<string> ObsoleteHCMVersions = new();

        /// <summary>
        /// Public accessor for PointerData.
        /// </summary>
        /// <param name="pointerName">Name of the pointer.</param>
        /// <param name="pointerVersion">Which version of MCC do you need the data for?</param>
        /// <returns></returns>
        public object? GetPointer(string? pointerName, string? pointerVersion)
        {
            // validate args
            if (pointerName == null || pointerVersion == null)
            {
                return null;
            }

            // find pointer name in data
            bool success = PointerData.TryGetValue(pointerName, out Dictionary<string, object>? pointerVersionDictionary);
            if (success && pointerVersionDictionary != null)
            {
                // test "All" version first
                success = pointerVersionDictionary.TryGetValue("All", out object? allPointer);
                if (success && allPointer != null)
                {
                    return allPointer;
                }
                else
                {
                    // test specific requested version
                    success = pointerVersionDictionary.TryGetValue(pointerVersion, out object? pointer);
                    if (success && pointer != null)
                    {
                        return pointer;
                    }
                }
            }

            // failure
            return null;
        }







        public void LoadPointerDataFromSource(out string failedReads)
        {
            string? xml = null;
            string localPointerDataPath = Directory.GetCurrentDirectory() + "\\ExternalPointerData.xml";
            failedReads = "";


#if HCM_DEBUG
// Debug mode to use local data file as embedded resource
            var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream("HCMExternal.ExternalPointerData.xml");
            if (stream != null)
            {
                StreamReader reader = new StreamReader(stream);
                xml = reader.ReadToEnd();
            }

#else
            // Download the xml from git for latest version
            try
            {
                var client = new System.Net.Http.HttpClient();
                xml = client.GetStringAsync("https://raw.githubusercontent.com/Burnt-o/HaloCheckpointManager/master/HCMExternal/ExternalPointerData.xml").Result;
            }
            catch 
            {
                // otherwise use embedded resource
                Log.Error("Couldn't resolve github external pointer data");
                // Couldn't grab online data, read offline embedded resource instead
                var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream("HCMExternal.ExternalPointerData.xml");
                if (stream != null)
                {
                    StreamReader reader = new StreamReader(stream);
                    xml = reader.ReadToEnd();
                }
            }



#endif

            

            if (xml == null || !xml.Any())
            {
                throw new Exception("Couldn't find pointerdata!");
            }

            // Deserialise
            XDocument doc = XDocument.Parse(xml);

            if (doc.Root == null)
            {
                throw new Exception("Something went wrong parsing the Pointer data xml file; 'doc.Root' was null.");
            }

            foreach (XElement entry in doc.Root.Elements())
            {
                try
                {
                    object? entryObject = null;
                    string? entryName = null;
                    string? entryVersion = null;

                    if (entry.Name == "HighestSupportedVersion")
                    {
                        HighestSupportedMCCVersion = entry.Value == null || entry.Value == "" ? null : entry.Value;
                        Log.Debug("Loaded HighestSupportedMCCVersion, value: " + HighestSupportedMCCVersion);
                    }

                    // We'll get rid of "LatestHCMVersion" in favour of "LatestHCMVersions" once 2.0.6 is deprecated
                    else if (entry.Name == "LatestHCMVersions")
                    {
                        if (entry.Value != null && entry.Value != "")
                        {

                            foreach (XElement ver in entry.Elements().Where(x => x.Name == "Entry"))
                            {
                                LatestHCMVersions.Add(ver.Value);
                            }
                        }
                        LatestHCMVersion = entry.Value == null || entry.Value == "" ? null : entry.Value;
                        Log.Debug("Loaded LatestHCMVersions, value: " + LatestHCMVersion);
                    }
                    else if (entry.Name == "ObsoleteHCMVersions")
                    {
                        if (entry.Value != null && entry.Value != "")
                        {
                            foreach (XElement ver in entry.Elements().Where(x => x.Name == "Entry"))
                            {
                                ObsoleteHCMVersions.Add(ver.Value);
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
                    failedReads = failedReads + "\n" + "Error processing an entry in pointerdata, " + ex.ToString() + ", entry.name: " + entry.Element("Name")?.Value + ", entry.Value: " + entry.Value;
                    Log.Error("Error processing an entry in pointerdata, " + ex.ToString());
                    continue;
                }

            }


        }
        private object ParseObject(XElement entry, string type)
        {
            object? returnObject = null;
            switch (type)
            {
                case "int":
                    returnObject = ParseHexNumberInt(entry.Element("Offset")?.Value);
                    break;

                case "string":
                    returnObject = entry.Element("Value")?.Value;
                    break;

                default:
                    throw new ArgumentException("Didn't recognise type of datapointer entry, " + type);
            }
            if (returnObject == null)
            {
                throw new Exception("Failed to parse returnObject but not sure why");
            }

            return returnObject;
        }

        private void StoreObject(string entryName, string entryVersion, object entryObject)
        {
            if (entryObject == null)
            {
                throw new Exception("entryObject was somehow null at StoreObject");
            }

            if (entryObject.GetType() == typeof(System.Xml.Linq.XElement))
            {
                throw new Exception("Accidentally parsed XElement");
            }

            if (PointerData.ContainsKey(entryName))
            {
                PointerData[entryName].Add(entryVersion, entryObject);
            }
            else
            {
                Dictionary<string, object> versionDictionary = new()
                {
                    { entryVersion, entryObject }
                };
                PointerData.Add(entryName, versionDictionary);
            }


            Log.Verbose("Added new object to pointer dictionary, name: " + entryName + ", version: " + entryVersion + ", type: " + entryObject.GetType().ToString());

        }



        private int ParseHexNumberInt(string? s)
        {
            if (s == null)
            {
                throw new Exception("int was null");
            }

            if (s == "0x0" || s == "0")
            {
                return 0;
            }

            int sign = 1;
            if (s.StartsWith("-"))
            {
                sign = -1;
                s = s.Substring(1);
            }


            return s.StartsWith("0x") ? Convert.ToInt32(s.Substring(2), 16) * sign : Convert.ToInt32(s) * sign;
        }




    }




}