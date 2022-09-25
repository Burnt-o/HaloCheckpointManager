using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCM.Model
{
    public class GameSaveFolder
    {
        public GameSaveFolder(string folderName, GameSaveCollection gameSaveCollection)
        { 
        _folderName = folderName;
            _gameSaveCollection = gameSaveCollection;
        }

        private string _folderName;
        private GameSaveCollection _gameSaveCollection;
        public string FolderName 
        { 
            get 
            { 
                return _folderName; 
            } 
        }

        public GameSaveCollection GameSaveCollection
        {
            get
            { 
                return _gameSaveCollection;  
            }
        }



    }
}
