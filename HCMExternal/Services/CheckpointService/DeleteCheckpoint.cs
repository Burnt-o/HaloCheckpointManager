using HCMExternal.Models;
using Serilog;
using System.IO;

namespace HCMExternal.Services.CheckpointServiceNS
{

    public partial class CheckpointService
    {

        /// <summary>
        /// Deletes a checkpoint.
        /// </summary>
        /// <param name="SelectedSaveFolder">The folder containing the checkpoint to delete.</param>
        /// <param name="SelectedCheckpoint">The checkpoint to delete.</param>
        public void DeleteCheckpoint(SaveFolder SelectedSaveFolder, Checkpoint? SelectedCheckpoint)
        {
            if (SelectedCheckpoint == null)
            {
                return;
            }

            string savePath = SelectedSaveFolder.SaveFolderPath + "//" + SelectedCheckpoint.CheckpointName + ".bin";

            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath) || !File.Exists(savePath))
            {
                Log.Error("Delete checkpoint failed! File didn't exist at path " + savePath);
                return;
            }

            File.Delete(savePath);

        }
    }

}
