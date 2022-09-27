using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;

namespace HCM3.MainWindow
{
    public class PointerCollection
    {
        #region Properties
        private Dictionary<string, Dictionary<string, ReadWrite.Pointer>> Pointers { get; set; } = new()
        {
            { "H1_Checkpoint", new()},
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

        public bool LoadPointersFromGit(out string exceptionString)
        {
            exceptionString = "";
            try
            {
                // Download the xml from git
                string url = "https://raw.githubusercontent.com/Burnt-o/HaloCheckpointManager/master/WpfApp3/offsets/Updates.json";
                System.Net.WebClient client = new System.Net.WebClient();
                string xml = client.DownloadString(url);

                // Deserialise

            }
            catch (Exception ex)
            {
                exceptionString = ex.Message;
                return false;
            }
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
