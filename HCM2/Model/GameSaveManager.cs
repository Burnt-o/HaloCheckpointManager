using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Windows.Controls;
namespace HCM.Model
{


    public class GameSaveManager
    {
        public GameSaveManager()
        {
            GameSaveCollection testSaveCollection = new GameSaveCollection();
            testSaveCollection.Add(new GameSave("My First Checkpoint", DateTime.Now, "a10", 3, TimeSpan.FromSeconds(6), DateTime.Now));
            GameSaveFolder folder1 = new GameSaveFolder("folder1", testSaveCollection);


            

            TreeItems = new ObservableCollection<TreeItem>();
            TreeViewItem item = folder1;
            TreeItems.Add(folder1);
            

        }




        public ObservableCollection<TreeItem> TreeItems { get; set; }
    }
}
