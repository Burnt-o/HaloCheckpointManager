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
    public class PointerCollection
    {
        #region Properties
        private Dictionary<string, Dictionary<string, ReadWrite.Pointer>> Pointers { get; set; } = new()
        {
        };


        #endregion

        #region Constructor
        public PointerCollection()
        {
            Pointers = new();
        }

        #endregion



        public bool LoadPointersFromGit(out string exceptionString)
        {
            exceptionString = "";
            try
            {
                // Download the xml from git
                string url = "https://raw.githubusercontent.com/Burnt-o/HaloCheckpointManager/HCM2/HCM3/Pointers.xml";
                System.Net.WebClient client = new System.Net.WebClient();
                string xml = client.DownloadString(url);


                // but actually for testing & learning let's just do this
                xml = @"
                    <Pointers>
                    <Pointer>
	                    <Name>H1_Checkpoint</Name>
	                    <Version>1.2645.0.0</Version>
		                    <Module>halo1.dll</Module>
		                    <Offsets>
			                    <Offset>10</Offset>
			                    <Offset>20</Offset>
			                    <Offset>30</Offset>
		                    </Offsets>
                    </Pointer>
                    </Pointers>";


                // Deserialise
                XDocument doc = XDocument.Parse(xml);

                if (doc.Root == null)
                {
                    return false;
                }

                foreach (XElement e in doc.Root.Elements())
                {
                    if (e.Name == "Pointer")
                    {
                        // TODO: parse hexadecimal numbers in pointerOffsets
                        // Read the data from the Pointer element (if the data exists)
                        string? pointerName = e.Element("Name") == null ? null : e.Element("Name").Value;
                        string? pointerVersion = e.Element("Version") == null ? null : e.Element("Version").Value;
                        string? pointerModule = e.Element("Module") == null ? null : e.Element("Module").Value;
                        int[]? pointerOffsets = e.Element("Module") == null ? null : e.Element("Offsets").Elements().Select(x => XmlConvert.ToInt32(x.Value)).ToArray();

                        // Check that all the data actually exists
                        if (!(pointerName == null || pointerVersion == null || pointerModule == null || pointerOffsets == null))
                        {
                            // Load the pointer into the Pointers dictionary, with pointerName and pointerVersion as the keys
                            ReadWrite.Pointer newPointer = new(pointerModule, pointerOffsets);
                            Dictionary<string, ReadWrite.Pointer> versionDictionary = new();
                            versionDictionary.Add(pointerVersion, newPointer);
                            Pointers.Add(pointerName, versionDictionary);
                            Trace.WriteLine("Added new pointer, name: " + pointerName + ", module: " + pointerModule + ", version: " + pointerVersion);
                        }
                    }


                }

            }
            catch (Exception ex)
            {
                exceptionString = ex.Message;
                return false;
            }
            return true;
        }

        public ReadWrite.Pointer? GetPointer(string pointerName, string pointerVersion)
        {
                bool success = Pointers.TryGetValue(pointerName, out Dictionary<string, ReadWrite.Pointer>? pointerVersionDictionary);
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
    }
}
