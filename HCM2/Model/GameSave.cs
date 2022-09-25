using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Runtime;

namespace HCM.Model
{
    public class GameSave : TreeItem
    {

        #region Data
        DateTime _modifiedDateTime;
        readonly string _levelName;
        readonly uint _difficulty;
        readonly TimeSpan _levelTime;
        readonly DateTime _createdDateTime;


        #endregion // Data

        #region Constructor
        public GameSave(string gameSaveName, DateTime modifiedDateTime, string levelName, uint difficulty, TimeSpan levelTime, DateTime createdDateTime) : base(gameSaveName)
        {
            _modifiedDateTime = modifiedDateTime;   
            _levelName = levelName;
            _difficulty = difficulty;
            _levelTime = levelTime;
            _createdDateTime = createdDateTime;
            
        }
        #endregion // Constructor

        #region Properties



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


    }
}
