using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace HCM3.Model
{
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
