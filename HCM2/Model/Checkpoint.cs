using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System;
using System.ComponentModel;
using System.Runtime;

namespace HCM.Model
{
    public class Checkpoint : INotifyPropertyChanged
    {

        #region Data
        string _checkpointName;
        DateTime _modifiedDateTime;
        readonly string _levelName;
        readonly uint _difficulty;
        readonly TimeSpan _levelTime;
        readonly DateTime _createdDateTime;


        #endregion // Data

        #region Constructor
        public Checkpoint(string checkpointName, DateTime modifiedDateTime, string levelName, uint difficulty, TimeSpan levelTime, DateTime createdDateTime)
        {
            _checkpointName = checkpointName;
            _modifiedDateTime = modifiedDateTime;   
            _levelName = levelName;
            _difficulty = difficulty;
            _levelTime = levelTime;
            _createdDateTime = createdDateTime;
            
        }
        #endregion // Constructor

        #region Properties

        public string CheckpointName
        {
            get { return _checkpointName; }
            set
            {
                _checkpointName = value;
                this.OnPropertyChanged("CheckpointName");
            }
        }

        public DateTime ModifiedDateTime
        { 
            get { return _modifiedDateTime; } 
        }

        public string LevelName
        {
            get { return _levelName; }
        }

        public uint Difficulty
        {
            get { return _difficulty; }
        }

        public TimeSpan LevelTime
        {
            get { return _levelTime; }
        }

        public DateTime CreatedDateTime
        {
            get { return _createdDateTime; }
        }

        #endregion // Properties

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler? PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler? handler = this.PropertyChanged;
            if (handler != null)
                handler(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion // INotifyPropertyChanged Members

    }
}
