using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace HCM.Model
{
    public class GameSaveFolder : TreeItem
    {

        public GameSaveFolder(string folderName, GameSaveCollection gameSaveCollection) : base(folderName)
        { 
            _gameSaveCollection = gameSaveCollection;
        }

        private GameSaveCollection _gameSaveCollection;

        public GameSaveCollection GameSaveCollection
        {
            get
            { 
                return _gameSaveCollection;  
            }
        }

        


    }
}
