using HCMExternal.Models;
using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Navigation;
using System.Xaml.Permissions;

namespace HCMExternal.CheckpointSorting
{
    internal class SortAlphabetical : NullComparer, ICompareCheckpoints
    {
        public string Name { get; init; } = "Name of Checkpoint";

        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            if (NullCompare(x, y, out int nullint))
            {
                return nullint;
            }

            return x.CheckpointName.CompareTo(y.CheckpointName);
        }
    }

    internal class SortVersion : NullComparer, ICompareCheckpoints
    {
        public string Name { get; init; } = "Game Version";

        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            if (NullCompare(x, y, out int nullint))
            {
                return nullint;
            }

            if (NullCompare(x.GameVersion, y.GameVersion, out int nullint2))
            {
                return nullint2;
            }

            return x.GameVersion.CompareTo(y.GameVersion);
        }
    }

    internal class SortDifficulty : NullComparer, ICompareCheckpoints
    {
        public string Name { get; init; } = "Difficulty";

        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            if (NullCompare(x, y, out int nullint))
            {
                return nullint;
            }

            if (NullCompare(x.Difficulty, y.Difficulty, out int nullint2))
            {
                return nullint2;
            }

            return x.Difficulty.Value.CompareTo(y.Difficulty.Value);
        }
    }

    internal class SortLevel : NullComparer, ICompareCheckpoints
    {
        public HaloGame gameEnum { get; set; }
        public string Name { get; init; } = "Level Order";

        //Here we're sorting by the actual level order in the campaign. SP missions have positive integer values, MP/unknown have value of -1
        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            try 
            { 
                Log.Verbose("SortLevel was called");
                if (NullCompare(x, y, out int nullint))
                {
                    Log.Error("One of the actual checkpoints was null, x: " + x.ToString() + ", y: " + y.ToString());
                    return nullint;
                }

                if (NullCompare(x?.LevelName, y?.LevelName, out int nullint2))
                {
                    Log.Verbose("x.LevelName: " + x.LevelName);
                    Log.Verbose("y.LevelName: " + y.LevelName);
                    return nullint2;
                }


                int xLevel = new LevelInfo(gameEnum, x.LevelName).LevelPosition;
                int yLevel = new LevelInfo(gameEnum, y.LevelName).LevelPosition;

                return (xLevel.CompareTo(yLevel));
            }
            catch(Exception ex)
            {
                Log.Error("an error occured sorting: " + ex.Message + "\n" + ex.Source);
                return 0;
            }

        }
    }

    internal class SortCreationTime : NullComparer, ICompareCheckpoints
    {
        public string Name { get; init; } = "File Creation Time";
        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            if (NullCompare(x, y, out int nullint))
            {
                return nullint;
            }

            if (NullCompare(x.CreatedOn, y.CreatedOn, out int nullint2))
            {
                return nullint2;
            }

            return x.CreatedOn.Value.CompareTo(y.CreatedOn.Value);
        }
    }

    internal class SortInGameTime : NullComparer, ICompareCheckpoints
    {
        public string Name { get; init; } = "In Game Time";
        public int Compare(Checkpoint? x, Checkpoint? y)
        {


            if (NullCompare(x, y, out int nullint))
            {
                return nullint;
            }

            if (NullCompare(x.GameTickCount, y.GameTickCount, out int nullint2))
            {
                return nullint2;
            }

            Log.Verbose("SortIGT: x " + x.GameTickCount);
            Log.Verbose("SortIGT: y " + y.GameTickCount);

            return x.GameTickCount.Value.CompareTo(y.GameTickCount.Value);
        }
    }

    internal class SortLastWriteTime : NullComparer, ICompareCheckpoints
    {
        public string Name { get; init; } = "Current Order";
        public int Compare(Checkpoint? x, Checkpoint? y)
        {
            if (NullCompare(x, y, out int nullint))
            {
                return nullint;
            }

            if (NullCompare(x.ModifiedOn, y.ModifiedOn, out int nullint2))
            {
                return nullint2;
            }

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

    internal abstract class NullComparer
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
