
using HCMExternal.ViewModels.Interfaces;
using Serilog;

namespace HCMExternal.ViewModels
{
    public class MainViewModel : Presenter
    {


        public CheckpointViewModel CheckpointViewModel { get; init; }
        public MCCHookStateViewModel MCCHookStateViewModel { get; init; }

        public MainViewModel(CheckpointViewModel cpvm, MCCHookStateViewModel hookState)
        {
            Log.Verbose("MainViewModel constructing");
            CheckpointViewModel = cpvm;
            MCCHookStateViewModel = hookState;
        }

    }
}
