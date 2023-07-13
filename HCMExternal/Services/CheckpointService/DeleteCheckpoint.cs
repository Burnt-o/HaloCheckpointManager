using System.IO;
using HCMExternal.Models;

namespace HCMExternal.Services.CheckpointServiceNS
{

    public partial class CheckpointService
    {

        /// <summary>
        /// Deletes a checkpoint.
        /// </summary>
        /// <param name="SelectedSaveFolder">The folder containing the checkpoint to delete.</param>
        /// <param name="SelectedCheckpoint">The checkpoint to delete.</param>
        public void DeleteCheckpoint(SaveFolder? SelectedSaveFolder, Checkpoint? SelectedCheckpoint)
        {
            if (SelectedSaveFolder == null || SelectedCheckpoint == null) return;

            string savePath = SelectedSaveFolder.SaveFolderPath + "//" + SelectedCheckpoint.CheckpointName + ".bin";

            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath) || !File.Exists(savePath)) return;

            File.Delete(savePath);

        }
    }
   
}
