using HCMExternal.ViewModels.Interfaces;
using Serilog;

namespace HCMExternal.ViewModels
{
    public class MainViewModel : Presenter
    {


        public CheckpointViewModel CheckpointViewModel { get; init; }

        public StatusBar StatusBar { get; init; }

        public MainViewModel(CheckpointViewModel checkpointViewModel, StatusBar statusBar) 
        {
            Log.Verbose("MainViewModel constructing");
            this.CheckpointViewModel = checkpointViewModel;
            this.StatusBar = statusBar;
        }

    }
}
