using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Runtime;

namespace HCM.Model
{
    public class GameSave : INotifyPropertyChanged
    {

        #region Data
        string _gameSaveName;
        DateTime _modifiedDateTime;
        readonly string _levelName;
        readonly uint _difficulty;
        readonly TimeSpan _levelTime;
        readonly DateTime _createdDateTime;


        #endregion // Data

        #region Constructor
        public GameSave(string gameSaveName, DateTime modifiedDateTime, string levelName, uint difficulty, TimeSpan levelTime, DateTime createdDateTime)
        {
            _gameSaveName = gameSaveName;
            _modifiedDateTime = modifiedDateTime;   
            _levelName = levelName;
            _difficulty = difficulty;
            _levelTime = levelTime;
            _createdDateTime = createdDateTime;
            
        }
        #endregion // Constructor

        #region Properties

        public string GameSaveName
        {
            get { return _gameSaveName; }
            set
            {
                _gameSaveName = value;
                this.OnPropertyChanged("GameSaveName");
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
