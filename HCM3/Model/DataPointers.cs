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
    public class DataPointers
    {
        #region Properties
        private Dictionary<string, Dictionary<string, ReadWrite.Pointer>> PointerData { get; set; } = new()
        {
        };



        #endregion

        #region Constructor
        public DataPointers()
        {
            PointerData = new();
        }

        #endregion



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
                    if (e.Name == "Pointer")
                    {
                        // Read the data from the Pointer element (if the data exists)
                        string? pointerName = e.Element("Name") == null ? null : e.Element("Name")?.Value;
                        string? pointerVersion = e.Element("Version") == null ? null : e.Element("Version")?.Value;
                        string? pointerModule = e.Element("Module") == null ? null : e.Element("Module")?.Value;
                        int[]? pointerOffsets = e.Element("Offsets") == null ? null : e.Element("Offsets")?.Elements().Select(x => ParseHexNumber(x.Value)).ToArray();
                        //IntPtr? pointerIntPtr = e.Element("IntPtr") == null ? null : (IntPtr?)ParseHexNumber(e.Element("IntPtr").Value);

                        // Check that all the data actually exists and make the pointer
                        if (pointerName != null && pointerVersion != null && pointerModule != null && pointerOffsets != null)
                        {
                                    ReadWrite.Pointer newPointer = new ReadWrite.Pointer(pointerModule, pointerOffsets);
                                    // Load the pointer into the Pointers dictionary, with pointerName and pointerVersion as the keys    
                                    Dictionary<string, ReadWrite.Pointer> versionDictionary = new();
                                    versionDictionary.Add(pointerVersion, newPointer);
                                    PointerData.Add(pointerName, versionDictionary);
                                    Trace.WriteLine("Added new pointer, name: " + pointerName);
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

        }

        public ReadWrite.Pointer? GetPointer(string? pointerName, string? pointerVersion)
        {
            if (pointerName == null || pointerVersion == null)
            {
                return null;
            }

            bool success = PointerData.TryGetValue(pointerName, out Dictionary<string, ReadWrite.Pointer>? pointerVersionDictionary);
                if (!success || pointerVersionDictionary == null)
                {
                    return null;
                }

                success = pointerVersionDictionary.TryGetValue(pointerVersion, out ReadWrite.Pointer? pointer);
                if (!success || pointer == null)
                {
                    return null;
                }

                return pointer;
        }

        public bool PointerExists(string? pointerName, string? pointerVersion)
        {
            if (pointerName == null || pointerVersion == null)
            {
                return false;
            }

            bool success = PointerData.TryGetValue(pointerName, out Dictionary<string, ReadWrite.Pointer>? pointerVersionDictionary);
            if (!success || pointerVersionDictionary == null)
            {
                return false;
            }

            success = pointerVersionDictionary.TryGetValue(pointerVersion, out ReadWrite.Pointer? pointer);
            return success && pointer != null;

        }



    }
}
