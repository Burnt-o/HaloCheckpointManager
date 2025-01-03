using HCMExternal.Models;
using Serilog;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using HCMExternal.CheckpointSorting;

namespace HCMExternal.Services.CheckpointIO.Impl
{
    public partial class CheckpointIOService
    {

        /// <summary>
        /// Sorts all the checkpoints in a folder, according to a list (in order) of ICompareCheckpoints (the user sets these).
        /// </summary>
        /// <param name="SelectedSaveFolder">SaveFolder containing the checkpoints to be sorted.</param>
        /// <param name="CheckpointCollection">CheckpointCollection containing the checkopints.</param>
        /// <param name="comparers">List of comparerers passed from user selection.</param>
        /// <param name="gameEnum">Which game are we sorting? Needed to implement level sorting.</param>
        /// <exception cref="Exception"></exception>
        public void SortCheckpoints(SaveFolder SelectedSaveFolder, ObservableCollection<Checkpoint> CheckpointCollection, List<Tuple<ICompareCheckpoints, bool>> comparers, HaloGame gameEnum)
        {
            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath))
            {
                Log.Error("SortCheckpoints: selected save folder didn't actually exist at path " + SelectedSaveFolder.SaveFolderPath);
                return;
            }

            List<Checkpoint> UnsortedCheckpoints = new(CheckpointCollection);

            comparers.Reverse();

            Log.Information("Sorting checkpoints in " + SelectedSaveFolder.SaveFolderPath);

            foreach (Tuple<ICompareCheckpoints, bool> comparer in comparers)
            {
                Log.Debug("Sorting by comparer: " + comparer.Item1.Name);
                if (comparer.Item1.GetType() == typeof(SortNothing))
                {
                    continue;
                }

                if (comparer.Item1.GetType() == typeof(SortLevel))
                {
                    (comparer.Item1 as SortLevel).gameEnum = gameEnum;
                }

                if (!comparer.Item2) //reverse flag == false
                {
                    UnsortedCheckpoints = UnsortedCheckpoints.OrderBy(n => n, comparer.Item1).ToList();
                }
                else //reverse flag == true
                {
                    UnsortedCheckpoints.Reverse();
                    UnsortedCheckpoints = UnsortedCheckpoints.OrderBy(n => n, comparer.Item1).ToList();
                    UnsortedCheckpoints.Reverse();
                }
            }

            //now actually write the LastWriteTimes to reflect the new order
            List<DateTime> DateTimesToAssign = new();
            foreach (Checkpoint checkpoint in UnsortedCheckpoints)
            { DateTimesToAssign.Add(checkpoint.ModifiedOn.Value); }

            DateTimesToAssign.Sort();

            Log.Debug(DateTimesToAssign.Count + ", " + UnsortedCheckpoints.Count);



            for (int i = 0; i < UnsortedCheckpoints.Count; i++)
            {
                string filePath = SelectedSaveFolder.SaveFolderPath + "\\" + UnsortedCheckpoints[i].CheckpointName + ".bin";
                if (!File.Exists(filePath))
                {
                    throw new Exception("File didn't exist at path: " + filePath);
                }

                FileInfo fileInfo = new FileInfo(filePath)
                {
                    LastWriteTime = DateTimesToAssign[i]
                };


            }

            for (int i = 0; i < UnsortedCheckpoints.Count; i++)
            {
                Log.Verbose(UnsortedCheckpoints[i].CheckpointName + ", " + UnsortedCheckpoints[i].LevelName);
            }


        }
    }


}
