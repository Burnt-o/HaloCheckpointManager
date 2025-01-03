using HCMExternal.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.Services.Interproc
{
    public interface IInterprocService
    {
        public class StatusMachineStatusChangedEventArgs : EventArgs
        {
            public required MCCHookStateEnum hookState { get; init; }
        }
        public event EventHandler<StatusMachineStatusChangedEventArgs> StateMachineStatusChangedHandler;


        public class InterprocErrorEventArgs : EventArgs
        {
            public required string errorMessage { get; init; }
        }
        public event EventHandler<InterprocErrorEventArgs> InterprocErrorHandler;


        public void initializeSharedMemory(Checkpoint? initialCheckpoint, SaveFolder initialSaveFolder, HaloGame initialGame);
        public void resetStateMachineEx();

        public void UpdateSharedMemCheckpoint(HaloGame game, Checkpoint? checkpoint);
        public void UpdateSharedMemSaveFolder(HaloGame game, SaveFolder checkpoint);
        public void UpdateSharedMemQueueInjectCommand();
    }
}
