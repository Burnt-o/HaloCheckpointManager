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
            { "H1_Checkpoint", new() },
            { "H1_Revert", new() },
            { "H1_CoreSave", new() },
            { "H1_CoreLoad", new() },
        };


        #endregion

        #region Constructor
        public PointerCollection()
        {
            Pointers = new();
        }

        #endregion

        public class PointerContainer
        {
            public PointerContainer(string name, string version, string? module = null, int[]? offsets = null)
            {
                Trace.WriteLine("name: " + name);
                Trace.WriteLine("version: " + version);
                Trace.WriteLine("module: " + module);
            }
        }

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

                foreach (XElement e in doc.Root.Elements())
                {
                    if (e.Name == "Pointer")
                    {
                        //TODO; deal with nullability (missing elements)
                        //TODO: parse hexadecimal numbers
                        string name = e.Element("Name").Value;
                        Trace.WriteLine(name);
                        string version = e.Element("Version").Value;
                        Trace.WriteLine(version);
                        string module = e.Element("Module").Value;
                        Trace.WriteLine(module);
                        int[] offsets = e.Element("Offsets").Elements().Select(x => XmlConvert.ToInt32(x.Value)).ToArray();
                        Trace.WriteLine("0: " + offsets[0]);
                        Trace.WriteLine("1: " + offsets[1]);
                        Trace.WriteLine("2: " + offsets[2]);
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

        public ReadWrite.Pointer? GetPointer(string pointerKey, string versionKey)
        {
            Dictionary<string, ReadWrite.Pointer> pointerVersionDictionary = Pointers[pointerKey];
            if (pointerVersionDictionary == null)
            {
                return null;
            }

            ReadWrite.Pointer pointer = pointerVersionDictionary[versionKey];
            return pointer;
        }
    }
}
