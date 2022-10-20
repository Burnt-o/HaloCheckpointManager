using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace HCM3.Services.Trainer
{
    //for members of PersistentCheatManager
    public interface IPersistentCheatService : INotifyPropertyChanged
    {
        public bool IsChecked { get; set; } // for binding by ToggleControlViewModel

        public void ToggleCheat();


        public bool ApplyCheat();

        public void RemoveCheat();

        // Used by PersistentCheatManager's recheck timer too see if hook still exists (/needs reapplying)
        public bool IsCheatApplied();
    }
}
