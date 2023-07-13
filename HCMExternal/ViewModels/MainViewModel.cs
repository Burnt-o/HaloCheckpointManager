using HCMExternal.ViewModels.Interfaces;
using Serilog;

namespace HCMExternal.ViewModels
{
    public class MainViewModel : Presenter
    {


        public CheckpointViewModel CheckpointViewModel { get; init; }


        public MainViewModel(CheckpointViewModel checkpointViewModel) 
        {
            Log.Verbose("MainViewModel constructing");
            this.CheckpointViewModel = checkpointViewModel;
        }

    }
}
