using System;
using System.Collections.Generic;
using HCMExternal.Models;
using System.IO;
using System.Diagnostics;
using Serilog;

namespace HCMExternal.Services.CheckpointServiceNS
{ 
    public partial class CheckpointService
    {
        /// <summary>
        /// Rewrites the lastWriteTime of a list of checkpoints according to a specified order. LastWriteTimes are used to order the checkpoint list.
        /// </summary>
        /// <param name="listCheckpoints">List (in order) of checkpoint files whose lastWriteTimes will be modified.</param>
        /// <param name="listLWT">List (in order) of the new lastWriteTimes.</param>
        /// <param name="SelectedSaveFolder">The location of the checkpoint files.</param>
        public void BatchModCheckpointLWTs(List<Checkpoint> listCheckpoints, List<DateTime?> listLWT, SaveFolder SelectedSaveFolder)
        {

            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath))
            {
                Log.Error("BatchModCheckpointLWTs: selected save folder didn't actually exist at path " + SelectedSaveFolder.SaveFolderPath);
                return;
            }

            for (int i = 0; i < listCheckpoints.Count; i++)
            {
                string checkpointPath = SelectedSaveFolder?.SaveFolderPath + "\\" + listCheckpoints[i].CheckpointName + ".bin";
                FileInfo checkpointInfo = new(checkpointPath);
                if (checkpointInfo.Exists && listLWT[i] != null)
                {
                    checkpointInfo.LastWriteTime = (DateTime)listLWT[i];
                }
                else
                {
                    Log.Error("Something went wrong modifying CheckpointLWTs; path: " + checkpointPath);
                }
            }
        }
    }
}
