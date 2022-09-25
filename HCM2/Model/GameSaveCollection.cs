using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;

namespace HCM.Model
{
    public class GameSaveCollection : ObservableCollection<GameSave>
    {
        public GameSaveCollection()
        { 
            this.GameSaves = new ObservableCollection<GameSave>();
        }

        public ObservableCollection<GameSave> GameSaves { get; set; }



    }
}
