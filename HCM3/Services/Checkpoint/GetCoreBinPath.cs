using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml.Linq;
using System.Diagnostics;

namespace HCM3.Services
{
    public partial class CheckpointServices
    {

        public string GetCoreBinPath()
        {
            Dictionary<string, string> coreBinFolderData = new();

            string coreBinFolderDataLocation = Directory.GetCurrentDirectory() + "\\CoreBinFolder.xml";
            if (File.Exists(coreBinFolderDataLocation))
            {
                try
                {
                    string xml = File.ReadAllText(coreBinFolderDataLocation);
                    XDocument doc = XDocument.Parse(xml);

                    foreach (XElement entry in doc.Root.Elements())
                    {
                        string? key = entry.Name.ToString();
                        string? value = entry.Value.ToString();

                        if (key != null && value != null)
                        { 
                            coreBinFolderData.Add(key, value);
                        }
                    }
                }
                catch
                { }
            }

            string? currentVersion = this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion;

            if (currentVersion == null) throw new Exception("Failed to get corebinfolder path - wasn't attached to MCC");

            if (coreBinFolderData.ContainsKey(currentVersion))
            { 
            return coreBinFolderData[currentVersion];
            }

            //else, let's try to autodetect the path
            Process mcc = Process.GetProcessById((int)this.HaloMemoryService.HaloState.ProcessID);
            string mccExePath = mcc.MainModule.FileName;

            Trace.WriteLine("MCC EXE PATH: " + mccExePath);
            string pathToLookFor = @"mcc\binaries\win64\MCC-Win64-Shipping.exe";
            if (!mccExePath.Contains(pathToLookFor, StringComparison.OrdinalIgnoreCase)) throw new Exception("Couldn't evaluate path of MCC process, actual: " + mccExePath + "\nThis is an error with trying to inject core saves. Let Burnt know, \nin the meanwhile use checkpoint injection instead.");


            mccExePath = mccExePath.Replace(pathToLookFor, @"core\core.bin", StringComparison.OrdinalIgnoreCase);

            if (!File.Exists(mccExePath)) throw new Exception("Couldn't autodetect core.bin folder path");

            coreBinFolderData.Add(currentVersion, mccExePath);

            SerialiseCoreBinData(coreBinFolderDataLocation, coreBinFolderData);

            return mccExePath;


        }

        private void SerialiseCoreBinData(string coreBinFolderDataLocation, Dictionary<string, string> coreBinFolderData)
        { 
        if (File.Exists(coreBinFolderDataLocation)) File.Delete(coreBinFolderDataLocation);

            using (TextWriter writer = File.CreateText(coreBinFolderDataLocation))
            {
                writer.WriteLine("<?xml version=\"1.0\" encoding=\"utf - 8\" ?>");
                writer.WriteLine("<Root>");

                foreach (KeyValuePair<string, string> kvp in coreBinFolderData)
                {
                    writer.WriteLine($"<{kvp.Key}>{kvp.Value}</{kvp.Key}>");

                }
                writer.WriteLine("</Root>");
            }


        }

    }
}
