using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Diagnostics;
using HCMExternal.Models;
using System.Collections.ObjectModel;
using Serilog;
using HCMExternal.Helpers.DictionariesNS;

namespace HCMExternal.Services.CheckpointServiceNS
{

    public partial class CheckpointService
    {

        /// <summary>
        /// Sorts all the checkpoints in a folder, according to a list (in order) of ICompareCheckpoints (the user sets these).
        /// </summary>
        /// <param name="SelectedSaveFolder">SaveFolder containing the checkpoints to be sorted.</param>
        /// <param name="CheckpointCollection">CheckpointCollection containing the checkopints.</param>
        /// <param name="comparers">List of comparerers passed from user selection.</param>
        /// <param name="gameIndex">Which game are we sorting? Needed to implement level sorting.</param>
        /// <exception cref="Exception"></exception>
        public void SortCheckpoints(SaveFolder? SelectedSaveFolder, ObservableCollection<Checkpoint> CheckpointCollection, List<Tuple<ICompareCheckpoints, bool>> comparers, HaloTabEnum gameIndex)
        {
            if (SelectedSaveFolder == null) throw new Exception("Can't sort - no savefolder selected");

            List<Checkpoint> UnsortedCheckpoints = new(CheckpointCollection);
            
            comparers.Reverse();

            Log.Information("Sorting checkpoints in " + SelectedSaveFolder.SaveFolderPath);

            foreach (Tuple<ICompareCheckpoints, bool> comparer in comparers)
            {
                Log.Debug("Sorting by comparer: " + comparer.Item1.Name);
                if (comparer.Item1.GetType() == typeof(SortNothing)) continue;
                if (comparer.Item1.GetType() == typeof(SortLevel))
                {
                    (comparer.Item1 as SortLevel).gameIndex = gameIndex;
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
            foreach(Checkpoint checkpoint in UnsortedCheckpoints)
            { DateTimesToAssign.Add(checkpoint.ModifiedOn.Value); }

            DateTimesToAssign.Sort();

            Log.Debug(DateTimesToAssign.Count + ", " + UnsortedCheckpoints.Count);



            for (int i = 0; i < UnsortedCheckpoints.Count; i++)
            {
                string filePath = SelectedSaveFolder.SaveFolderPath + "\\" + UnsortedCheckpoints[i].CheckpointName + ".bin";
                if (!File.Exists(filePath)) throw new Exception("File didn't exist at path: " + filePath);

                FileInfo fileInfo = new FileInfo(filePath);
                fileInfo.LastWriteTime = DateTimesToAssign[i];


            }

            for (int i = 0; i < UnsortedCheckpoints.Count; i++)
            {
                Log.Verbose(UnsortedCheckpoints[i].CheckpointName + ", " + UnsortedCheckpoints[i].LevelName);
            }


        }
    }


    class SortAlphabetical : NullComparer, ICompareCheckpoints
    {
        public string Name { get; init; } = "Name of Checkpoint";

        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            if (NullCompare(x, y, out int nullint)) return nullint;

            return x.CheckpointName.CompareTo(y.CheckpointName);
        }
    }

    class SortVersion : NullComparer, ICompareCheckpoints
    {
        public string Name { get; init; } = "Game Version";

        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            if (NullCompare(x, y, out int nullint)) return nullint;
            if (NullCompare(x.GameVersion, y.GameVersion, out int nullint2)) return nullint2;

            return x.GameVersion.CompareTo(y.GameVersion);
        }
    }

    class SortDifficulty : NullComparer, ICompareCheckpoints
    {
        public string Name { get; init; } = "Difficulty";

        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            if (NullCompare(x, y, out int nullint)) return nullint;
            if (NullCompare(x.Difficulty, y.Difficulty, out int nullint2)) return nullint2;


            return x.Difficulty.Value.CompareTo(y.Difficulty.Value);
        }
    }

    class SortLevel : NullComparer,  ICompareCheckpoints
    {
        public HaloTabEnum gameIndex { get; set; }
        public string Name { get; init; } = "Level Order";
        
        //Here we're sorting by the actual level order in the campaign. SP missions have positive integer values, MP/unknown have value of -1
        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            Log.Verbose("SortLevel was called");
            if (NullCompare(x, y, out int nullint))
            {
                Log.Error("One of the actual checkpoints was null, x: " + x.ToString() + ", y: " + y.ToString());
                return nullint;
            }
                
            if (NullCompare(x.LevelName, y.LevelName, out int nullint2))
            {
                Log.Verbose("x.LevelName: " + x.LevelName);
                Log.Verbose("y.LevelName: " + y.LevelName);
                return nullint2;

            }
                
            int? xOrder = null;
            int? yOrder = null;

            Dictionary<string, Dictionaries.LevelInfo> stringMapper = Dictionaries.GameToLevelInfoDictionary[gameIndex];
            if (stringMapper.ContainsKey(x.LevelName))
            {
                xOrder = stringMapper[x.LevelName].LevelPosition;
            }
            else
            {
                Log.Error("Couldn't find levelposition x: " + x.LevelName + ", game: " + gameIndex);
            }
            if (stringMapper.ContainsKey(y.LevelName))
            {
                yOrder = stringMapper[y.LevelName].LevelPosition;
            }
            else
            {
                Log.Error("Couldn't find levelposition y: " + y.LevelName + ", game: " + gameIndex);
            }



            if (NullCompare(xOrder, yOrder, out int nullint3))
            {
                Log.Error("One of the orderings was null: x: " + xOrder + ", y: " + yOrder + ", x.LevelName: " + x.LevelName + ", y.Levelname: " + y.LevelName + ", x.LevelName.Length: " + x.LevelName.Length + ", y.LevelName.Length: " + y.LevelName.Length);
                return nullint3;
            }
                

            return (xOrder.Value.CompareTo(yOrder.Value));


        }
    }

    class SortCreationTime : NullComparer,  ICompareCheckpoints
    {
        public string Name { get; init; } = "File Creation Time";
        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            if (NullCompare(x, y, out int nullint)) return nullint;
            if (NullCompare(x.CreatedOn, y.CreatedOn, out int nullint2)) return nullint2;



            return x.CreatedOn.Value.CompareTo(y.CreatedOn.Value);
        }
    }

    class SortInGameTime : NullComparer,  ICompareCheckpoints
    {
        public string Name { get; init; } = "In Game Time";
        public int Compare(Checkpoint? x, Checkpoint? y)
        {


            if (NullCompare(x, y, out int nullint)) return nullint;
            if (NullCompare(x.GameTickCount, y.GameTickCount, out int nullint2)) return nullint2;

            Log.Verbose("SortIGT: x " + x.GameTickCount);
            Log.Verbose("SortIGT: y " + y.GameTickCount);

            return x.GameTickCount.Value.CompareTo(y.GameTickCount.Value);
        }
    }

    class SortLastWriteTime : NullComparer,  ICompareCheckpoints
    {
        public string Name { get; init; } = "Current Order";
        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            if (NullCompare(x, y, out int nullint)) return nullint;
            if (NullCompare(x.ModifiedOn, y.ModifiedOn, out int nullint2)) return nullint2;


            return x.ModifiedOn.Value.CompareTo(y.ModifiedOn.Value);
        }
    }

    public class SortNothing : IComparer<Checkpoint>, ICompareCheckpoints
    {
        public string Name { get; init; } = "None";
        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            return 0;
        }
    }

    abstract class NullComparer
    {
        public bool NullCompare(object? x, object? y, out int returnval)
        {
            returnval = 0;
            if (x == null && y == null)
            {
                return true;
            }

            if (x == null)
            {
                returnval = -1;
                return true;
            }

            if (y == null)
            {
                returnval = 1;
                return true;
            }

            return false;
        }
    }


    public interface ICompareCheckpoints : IComparer<Checkpoint>
    {
        public string Name { get; init; }
    }


}
