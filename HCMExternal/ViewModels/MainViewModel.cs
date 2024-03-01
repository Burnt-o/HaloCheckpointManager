using HCMExternal.ViewModels.Interfaces;
using Serilog;

namespace HCMExternal.ViewModels
{
    public class MainViewModel : Presenter
    {


        public CheckpointViewModel CheckpointViewModel { get; init; }

        public MCCHookStateViewModel MCCHookStateViewModel { get; init; }

        public MainViewModel(CheckpointViewModel checkpointViewModel, MCCHookStateViewModel hookState) 
        {
            Log.Verbose("MainViewModel constructing");
            this.CheckpointViewModel = checkpointViewModel;
            this.MCCHookStateViewModel = hookState;
        }

    }
}
