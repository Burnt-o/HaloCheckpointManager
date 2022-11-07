using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Diagnostics;
using HCM3.Models;

namespace HCM3.Services
{

    //public interface IService
    //{

    //}

    public partial class CheckpointServices
    {
        public void OpenInExplorer(SaveFolder? SelectedSaveFolder)
        {
            if (SelectedSaveFolder == null) return;



            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath) ) return;

            Process.Start(new ProcessStartInfo(SelectedSaveFolder.SaveFolderPath) { UseShellExecute = true });

        }
    }
   
}
