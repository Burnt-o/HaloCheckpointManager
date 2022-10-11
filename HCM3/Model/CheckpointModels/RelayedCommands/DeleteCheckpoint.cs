using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Diagnostics;
using HCM3.Model.CheckpointModels;

namespace HCM3.Model.CheckpointModels
{
    internal sealed partial class CheckpointModel
    {
        public void DeleteCheckpoint(SaveFolder? SelectedSaveFolder, Checkpoint? SelectedCheckpoint)
        {
            if (SelectedSaveFolder == null || SelectedCheckpoint == null) return;

            string savePath = SelectedSaveFolder.SaveFolderPath + "//" + SelectedCheckpoint.CheckpointName + ".bin";

            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath) || !File.Exists(savePath)) return;

            File.Delete(savePath);

            this.RefreshCheckpointList(SelectedSaveFolder);
        }

    }
}
