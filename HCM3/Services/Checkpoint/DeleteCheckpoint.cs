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
        public void DeleteCheckpoint(SaveFolder? SelectedSaveFolder, Checkpoint? SelectedCheckpoint)
        {
            if (SelectedSaveFolder == null || SelectedCheckpoint == null) return;

            string savePath = SelectedSaveFolder.SaveFolderPath + "//" + SelectedCheckpoint.CheckpointName + ".bin";

            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath) || !File.Exists(savePath)) return;

            File.Delete(savePath);

        }
    }
   
}
