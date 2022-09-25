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
            this.GameSaves = this.Load();
        }

        public string Title { get; set; }
        public ObservableCollection<GameSave> GameSaves { get; set; }

        public GameSaveCollection Load()
        {
            this.Title = "AAAAAAAAAAAAAAH";
            this.GameSaves.Add(new GameSave("My First Checkpoint", DateTime.Now, "a10", 3, TimeSpan.FromSeconds(6), DateTime.Now)) ;
            this.GameSaves.Add(new GameSave("My Second Checkpoint", DateTime.Now, "a10", 0, TimeSpan.FromSeconds(12), DateTime.Now));
            this.GameSaves.Add(new GameSave("My Third Checkpoint", DateTime.Now, "a50", 3, TimeSpan.FromSeconds(78), DateTime.Now));
            return this;
        }

    }
}
