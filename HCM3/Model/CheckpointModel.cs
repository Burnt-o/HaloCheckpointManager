using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Collections.ObjectModel;


namespace HCM3.Model
{
    internal sealed class CheckpointModel
    {
        
        public HaloMemory HaloMemory;
        public ObservableCollection<Checkpoint> CheckpointCollection { get; set; }
        public CheckpointModel(HaloMemory haloMemory, ObservableCollection<Checkpoint> checkpointCollection)
        { 
        HaloMemory = haloMemory;
            CheckpointCollection = checkpointCollection;
        }
        public void PrintText()
        {
            Trace.WriteLine("Hello from CheckpointModel!");
        }
    }

    public class Checkpoint
    {
        public string CheckpointName { get; set; }
        public string LevelName { get; set; }
        public int Difficulty { get; set; }
        public int GameTickCount { get; set; }
        public string GameVersion { get; set; }
        public DateTime CreatedOn { get; set; }

        public Checkpoint(string levelName, string checkpointName = "test", int difficulty = 3, int gameTickCount = 1615, string gameVersion = "1.2645.0.0", DateTime createdOn = new DateTime())
        {
            CheckpointName = checkpointName;
            LevelName = levelName;
            Difficulty = difficulty;
            GameTickCount = gameTickCount;
            GameVersion = gameVersion;
            CreatedOn = createdOn;
        }



    }

}
