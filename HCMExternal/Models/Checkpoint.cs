using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace HCMExternal.Models
{
    public class Checkpoint : INotifyPropertyChanged
    {
        // TODO: might make these all nullable types eventually. Probably shouldn't though.
        // Ah but if checkpoint read error then it's either nullable props or don't display the checkpoint at all...
        private string _checkpointName;
        public string CheckpointName // Actual filename of the checkpoint.bin file.
        {
            get => _checkpointName;
            set
            {
                _checkpointName = value;
                OnPropertyChanged();
            }
        }

        public string CheckpointPath { get; set; }
        public string? LevelName { get; set; } // Name of the level as 3-letter-code
        public int? Difficulty { get; set; } // Difficulty of the game, 0 for Easy, 3 for Legendary
        public int? GameTickCount { get; set; } // How many ticks the game was along when the checkpoint was made
        public string? GameVersion { get; set; } // Version of the game, read from the end of the checkpoint file (where HCM writes it on dump)
        public DateTime? CreatedOn { get; set; } // Date the checkpoint was created (from file metadata)
        public DateTime? ModifiedOn { get; set; } // Used for sorting checkpoints




        public Checkpoint(string checkpointName, string checkpointPath, string? levelName, int? difficulty, int? gameTickCount, string? gameVersion, DateTime? createdOn, DateTime? modifiedOn)
        {
            _checkpointName = checkpointName;
            CheckpointPath = checkpointPath;
            LevelName = levelName;
            Difficulty = difficulty;
            GameTickCount = gameTickCount;
            GameVersion = gameVersion;
            CreatedOn = createdOn;
            ModifiedOn = modifiedOn;
        }
        public event PropertyChangedEventHandler? PropertyChanged;

        protected void OnPropertyChanged([CallerMemberName] string? name = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }
    }
}
