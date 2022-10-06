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

namespace HCM3
{
    public class DataCheckpoints
    {
        #region Properties
        private Dictionary<string, Dictionary<string,int>> CheckpointData { get; set; } = new()
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
                        string? dataName = e.Element("Name") == null ? null : e.Element("Name")?.Value;
                        string? dataVersion = e.Element("Version") == null ? null : e.Element("Version")?.Value;
                        int? dataOffset = e.Element("Offset") == null ? null : ParseHexNumber(e.Element("Offset").Value);



                        // Check that all the data actually exists and make the entry
                        if (dataName != null && dataVersion != null && dataOffset != null)
                        {
                            Dictionary<string, int> versionDictionary = new();
                            versionDictionary.Add(dataVersion, (int)dataOffset);
                            CheckpointData.Add(dataName, versionDictionary);
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


            int? ParseHexNumber(string? s)
            {
                if (s == null ) return null;

                return s.StartsWith("0x") ? Convert.ToInt32(s.Substring(2), 16) : Convert.ToInt32(s);
            }

        }

        public int? GetCheckpointOffset(string? dataName, string? dataVersion)
        {
            if (dataName == null || dataVersion == null)
            {
                return null;
            }

            bool success = CheckpointData.TryGetValue(dataName, out Dictionary<string, int>? dataVersionDictionary);
            if (!success || dataVersionDictionary == null)
            {
                return null;
            }

            success = dataVersionDictionary.TryGetValue(dataVersion, out int dataOffset);
            if (!success)
            {
                return null;
            }

            return dataOffset;
        }



    }
}
