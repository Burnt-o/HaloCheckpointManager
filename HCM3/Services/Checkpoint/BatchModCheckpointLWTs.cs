using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Models;
using System.IO;
using System.Diagnostics;

namespace HCM3.Services
{ 
    public partial class CheckpointServices
    {
        public void BatchModCheckpointLWTs(List<Checkpoint> listCheckpoints, List<DateTime?> listLWT, SaveFolder? SelectedSaveFolder)
        {
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
                    Trace.WriteLine("Something went wrong modifying CheckpointLWTs; path: " + checkpointPath);
                }
            }
        }
    }
}
