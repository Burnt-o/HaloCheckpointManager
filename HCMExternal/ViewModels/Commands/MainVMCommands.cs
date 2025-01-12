using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.ViewModels.Commands
{
    public class MainVMCommands
    {
        public RelayCommand DeleteCheckpoint { get; init; }
        public RelayCommand RenameCheckpoint { get; init; }
        public RelayCommand ReVersionCheckpoint { get; init; }
        public RelayCommand SortCheckpoint { get; init; }
        public RelayCommand OpenInExplorer { get; init; }
        public RelayCommand RenameFolder { get; init; }
        public RelayCommand DeleteFolder { get; init; }
        public RelayCommand NewFolder { get; init; }
        public RelayCommand ForceCheckpoint { get; init; }
        public RelayCommand ForceRevert { get; init; }
        public RelayCommand ForceDoubleRevert { get; init; }
        public RelayCommand DumpCheckpoint { get; init; }
        public RelayCommand InjectCheckpoint { get; init; }
        public RelayCommand DisableCheckpoint { get; init; }
    }
}
