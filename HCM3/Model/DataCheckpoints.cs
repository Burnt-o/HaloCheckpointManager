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
    public class DataCheckpoints
    {
        #region Properties
        private Dictionary<string, Dictionary<string,object>> CheckpointData { get; set; } = new()
        {
        };




        #endregion

        #region Constructor
        public DataCheckpoints()
        {
            CheckpointData = new();
        }

        #endregion



        public bool LoadCheckpointDataFromGit(out string exceptionString, out string? HighestSupportMCCVersion)
        {
            HighestSupportMCCVersion = null;
            exceptionString = "";
            try
            {
                const bool useOnline = false;
                string xml;
                if (useOnline)
                {
                    // Download the xml from git
                    string url = "https://raw.githubusercontent.com/Burnt-o/HaloCheckpointManager/HCM2/HCM3/CheckpointData.xml";
                    System.Net.WebClient client = new System.Net.WebClient();
                    xml = client.DownloadString(url);
                }
                else
                {
                    // Grab it from local repo for testing so that I don't have to push it to git everytime
                    xml = File.ReadAllText(@"C:\Users\mauri\source\repos\HaloCheckpointManager\HCM3\CheckpointData.xml");
                }

                // Deserialise
                XDocument doc = XDocument.Parse(xml);

                if (doc.Root == null)
                {
                    exceptionString = "Something went wrong parsing the Checkpoint data xml file; 'doc.Root' was null.";
                    return false;
                }

                foreach (XElement e in doc.Root.Elements())
                {
                    if (e.Name == "Data")
                    {
                        Trace.WriteLine("reading CheckpointData");
                        string? dataName = e.Element("Name") == null ? null : e.Element("Name")?.Value;
                        string? dataVersion = e.Element("Version") == null ? null : e.Element("Version")?.Value;
                        int? dataOffset = e.Element("Offset") == null ? null : ParseHexNumber(e.Element("Offset").Value);
                        PreserveLocation[]? preserveLocations = e.Element("PreserveLocations") == null ? null : e.Element("PreserveLocations")?.Elements().Select(x => ParseLocation(x)).ToArray();


                        // Check that all the data actually exists and make the entry
                        if (dataName != null && dataVersion != null)
                        {
                            if (dataOffset != null)
                            {
                                Dictionary<string, object> versionDictionary = new();
                                versionDictionary.Add(dataVersion, (int)dataOffset);
                                CheckpointData.Add(dataName, versionDictionary);
                            }
                            else if (preserveLocations != null)
                            {
                                Trace.WriteLine("Adding preserveLocations[] object to dictionary!");
                                Dictionary<string, object> versionDictionary = new();
                                versionDictionary.Add(dataVersion, (PreserveLocation[])preserveLocations);
                                CheckpointData.Add(dataName, versionDictionary);
                            }
                        }
                    }
                    else if (e.Name == "HighestSupportMCCVersion")
                    {
                        HighestSupportMCCVersion = e.Value;
                    }



                }

            }
            catch (Exception ex)
            {
                exceptionString = "HCM failed to download MCC checkpoint offset data. Check your internet connection, " +
                    "\\nor double check that HCM has the correct permissions."
                    + "\\nError: " + ex.Message;
                return false;
            }
            return true;

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

            int? ParseHexNumber(string? s)
            {
                if (s == null ) return null;

                return s.StartsWith("0x") ? Convert.ToInt32(s.Substring(2), 16) : Convert.ToInt32(s);
            }

        }

        public object? GetCheckpointData(string? dataName, string? dataVersion)
        {
            if (dataName == null || dataVersion == null)
            {
                return null;
            }

            bool success = CheckpointData.TryGetValue(dataName, out Dictionary<string, object>? dataVersionDictionary);
            if (!success || dataVersionDictionary == null)
            {
                return null;
            }

            success = dataVersionDictionary.TryGetValue(dataVersion, out object? dataObject);
            if (!success || dataObject == null)
            {
                return null;
            }

            return dataObject;
        }



    }
}
