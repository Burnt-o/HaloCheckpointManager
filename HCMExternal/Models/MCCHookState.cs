using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using NtApiDotNet;

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
            get { return _state; }
            set { _state = value; OnPropertyChanged(); }
        }



        // Handle to valid MCC process with PROCESS_QUERY_LIMITED_INFORMATION
        private NtProcess? _MCCProcess = null;
        public NtProcess? MCCProcess
        {
            get { return _MCCProcess; }
             set { _MCCProcess = value; OnPropertyChanged(); }
        }

        // What version of MCC are we connected to? Null if no MCC or unable to detect version. 10 char string of format "X.XXXX.X.X"
        private string? _MCCVersion = null;
        public string? MCCVersion
        {
            get { return _MCCVersion; }
             set { _MCCVersion = value; OnPropertyChanged(); }
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        protected void OnPropertyChanged([CallerMemberName] string? name = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }

    }
}
