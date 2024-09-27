using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace HCMExternal.Models
{
    /// <summary>
    /// Tracks the state of MCC (process handle, version of MCC, and an enum tracking internal injection progress).
    /// Updated by MCCHookService.
    /// </summary>


    public enum MCCHookStateEnum
    {
        MCCNotFound, // MCC process not found
        MCCAccessError, // Couldn't access MCC process - Admin priv issue
        MCCEACError, // EAC (easy anti-cheat) was running, user needs to launch MCC with eac disabled.
        StateMachineException, // Exceptions in the state machine
        InternalInjecting, // HCMExternal is currently trying to inject HCMInternal
        InternalInjectError, // Something went wrong injecting HCMInternal
        InternalInitialising, // HCMExternal is waiting for HCMInternal to initialise
        InternalException, // HCMInternal threw an error (during initialisation or otherwise)
        InternalSuccess // Everything went fine with HCMInternal injection & initialisation
    }


    public class MCCHookState : INotifyPropertyChanged
    {
        /// Simple enum flag that raises onProperyChanged, tracking the status of HCM hooking to MCC (injecting internal etc)

        private MCCHookStateEnum _state = MCCHookStateEnum.MCCNotFound;
        public MCCHookStateEnum State
        {
            get => _state;
            set { _state = value; OnPropertyChanged(); }
        }


        public event PropertyChangedEventHandler? PropertyChanged;

        protected void OnPropertyChanged([CallerMemberName] string? name = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }

    }
}
