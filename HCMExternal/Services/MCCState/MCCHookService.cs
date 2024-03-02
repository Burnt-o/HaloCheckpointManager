using HCMExternal.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using HCMExternal.Services.InterprocServiceNS;
using System.Runtime.CompilerServices;
using Serilog;
using System.Windows;
using HCMExternal.ViewModels;

namespace HCMExternal.Services.MCCHookService
{
    /// <summary>
    /// A state machine - reads MCCHookState, and updates it based on action took.
    /// Tries to find MCC process - then inject HCMInternal.dll into it, and confirm successful injection.
    /// </summary>
    public class MCCHookService
    {

        // store of current state (in MCCHookStateViewModel)
        private MCCHookState MCCHookState { 
            get { return mMCCHookStateViewModel.State; } 
            set { mMCCHookStateViewModel.State = value; }
        
        }

        // last injection error
        private string? lastInjectionError;

        // injected service
        private InterprocService mInterprocService { get; init; }
        private MCCHookStateViewModel mMCCHookStateViewModel { get; init; }
        private ErrorDialogViewModel mErrorDialogViewModel { get; init; }

        // Timer to loop State machine
        private System.Threading.Timer StateMachineLoopTimer;


        // Constructor
        public MCCHookService(InterprocService ips, MCCHookStateViewModel vm, ErrorDialogViewModel errorDialogVM)
        {
            mInterprocService = ips;
            mMCCHookStateViewModel = vm;
            vm.ShowErrorEvent += ShowHCMInternalErrorDialog;
            
            mErrorDialogViewModel = errorDialogVM;

            // Timer does not start immediately; only when BeginStateMachineLoop is called.
            StateMachineLoopTimer = new System.Threading.Timer(StateMachineLoopEventHandler, null, System.Threading.Timeout.Infinite, 1000);
        }

        // starts timer, called at end of application startup
        public void BeginStateMachineLoop() { StateMachineLoopTimer.Change(0, 1000); }


        private int stateMachineIsBusy = 0;
        // discards args, interlocks to prevent multiple StateMachineLoops running simultaneously
        private void StateMachineLoopEventHandler(object? nullarg) 
        {
            if (System.Threading.Interlocked.CompareExchange(ref this.stateMachineIsBusy, 1, 0) == 1)
            {
                return;
            }
            try
            {
                StateMachineLoop();
            }
            finally
            {
                this.stateMachineIsBusy = 0;
            }

        }

        // Just used during HCMInternal initialising - reported to us by interproc service
        private enum InternalStatusFlag
        {
            Initialising = 0,
            AllGood = 1,
            Error = 2,
            Shutdown = 3,
        }



        // main loop parsing MCCHookState and advancing the state machine
        private void StateMachineLoop()
        {
            switch (MCCHookState.State)
            {
                case MCCHookStateEnum.MCCNotFound:
                    
                    if (AnticheatIsEnabled())
                    {
                        lastInjectionError = "HCM detected EasyAntiCheat running - you must run MCC with easy anti-cheat disabled.";
                        AdvanceStateMachine(MCCHookStateEnum.MCCEACError);
                        ShowHCMInternalErrorDialog();
                        return;
                    }
                    
                    MCCHookState.MCCProcess = GetMCCProcess(); // try to find the mcc process
                    if (MCCHookState.MCCProcess == null) // failed, try again next time
                        break;

                    try
                    {
                        MCCHookState.MCCVersion = MCCHookState.MCCProcess.MainModule?.FileVersionInfo;
                    }
                    catch (Exception ex)
                    {
                        lastInjectionError = "HCM failed to access the MCC process\nIf Steam/MCC is running as admin, then HCM must be run as admin too.\n(But better to run both as non-admin)\nNerdy details:\n" + ex.Message;
                        AdvanceStateMachine(MCCHookStateEnum.MCCAccessError);
                        ShowHCMInternalErrorDialog();
                        return;
                    }


                    // Subscribe to process exit (to reset state back to MCCNotFound)
                    MCCHookState.MCCProcess.EnableRaisingEvents = true;
                    MCCHookState.MCCProcess.Exited += (o, i) =>
                    {
                        Log.Information("MCC process exited!");
                        MCCHookState.MCCVersion = null;
                        MCCHookState.MCCProcess = null;

                        AdvanceStateMachine(MCCHookStateEnum.MCCNotFound);
                        return;
                    };


                    AdvanceStateMachine(MCCHookStateEnum.InternalInjecting); // advance to next state (inject HCMInternal)
                    break;

                case MCCHookStateEnum.MCCAccessError:
                    // Do nothing
                    break;

                case MCCHookStateEnum.MCCEACError:
                    // Do nothing
                    break;

                case MCCHookStateEnum.InternalInjecting:
                    (bool successFlag, string errorString) injectionResult = mInterprocService.Setup();
                    

                    if (injectionResult.successFlag)
                    {
                        AdvanceStateMachine(MCCHookStateEnum.InternalInitialising);
                    }
                    else
                    {
                        AdvanceStateMachine(MCCHookStateEnum.InternalInjectError);
                        lastInjectionError = "HCM failed to inject its internal module into the game! \nMore info in HCMExternal.log file. Error message: \n" + injectionResult.errorString;
                        ShowHCMInternalErrorDialog(); 
                    }
                    break;

                case MCCHookStateEnum.InternalInitialising:
                    InternalStatusFlag currentInternalStateInit = (InternalStatusFlag)InterprocService.getHCMInternalStatusFlag();
                    switch (currentInternalStateInit)
                    {
                        case InternalStatusFlag.Initialising:
                            break; // do nothing (keep waiting)

                        case InternalStatusFlag.Error:
                            lastInjectionError = "HCMInternal had an exception during intialisation - see MCC window (or HCMInternal.log) for more details";
                            AdvanceStateMachine(MCCHookStateEnum.InternalException);
                            ShowHCMInternalErrorDialog();
                            break;


                        case InternalStatusFlag.AllGood:
                            AdvanceStateMachine(MCCHookStateEnum.InternalSuccess);
                            break;

                    }
                    break;


                case MCCHookStateEnum.InternalInjectError:
                    // Do nothing
                    break;

                case MCCHookStateEnum.InternalException:
                    // Do nothing
                    break;

                case MCCHookStateEnum.InternalSuccess:
                    // Check status flag to make sure no errors have popped up, otherwise do nothing
                    InternalStatusFlag currentInternalStateSuccess = (InternalStatusFlag)InterprocService.getHCMInternalStatusFlag();
                    Log.Verbose("InternalState: " + currentInternalStateSuccess.ToString());
                    if (currentInternalStateSuccess == InternalStatusFlag.Error)
                    {
                        lastInjectionError = "HCMInternal had an exception - see MCC window (or HCMInternal.log) for more details";
                        AdvanceStateMachine(MCCHookStateEnum.InternalException);
                        //ShowHCMInternalErrorDialog(); // Not necessary to show - HCMInternal has it's own dialog
                        return;
                    }

                    if (currentInternalStateSuccess == InternalStatusFlag.Shutdown)
                    {
                        MCCHookState.MCCProcess?.WaitForExit(3000);
                        AdvanceStateMachine(MCCHookStateEnum.MCCNotFound);
                        return;
                    }
                    break;

            }
        }

        // Safely advances the state machine
        private void AdvanceStateMachine(MCCHookStateEnum newState)
        {
            // Safety check! if new state and old state are same, we would enter an infinite loop
            if (MCCHookState.State == newState)
            {
                Log.Error("Tried advancing state machine to same state!");
                return;
            }
            else
            {
                MCCHookState.State = newState;
                StateMachineLoop(); // call StateMachineLoop to process new state
            }
        }

        // Reset state to MCCNotFound (for after InternalInjectError or InternalException)
        public void RetryHookFromError()
        {
            AdvanceStateMachine(MCCHookStateEnum.MCCNotFound);
        }

        // Dialog tells user about the error and askes them if they want to retry injection
        public void ShowHCMInternalErrorDialog()
        {
            Log.Verbose("ShowHCMInternalErrorDialog");
            var result = mErrorDialogViewModel.RaiseShowErrorDialogEvent(lastInjectionError);
            if (result == false)
            {
                // User doesn't want to retry injection, do nothing
            }
            else
            {
                RetryHookFromError();
            }
        }


        private Process? GetMCCProcess()
        {
            try
            {
             string[] MCCProcessNames = { "MCC-Win64-Shipping", "MCC-Win64-Winstore-Shipping", "MCCWinstore-Win64-Shipping" };

                foreach (string procName in MCCProcessNames)
                {
                    foreach (Process process in Process.GetProcesses())
                    {
                        if (String.Equals(process.ProcessName, procName,
                         StringComparison.OrdinalIgnoreCase) && !process.HasExited)
                        {
                            // We don't want to inject while MCC is booting up since LoadLibrary is occupied
                            Log.Verbose("Found MCC");
                            TimeSpan MCCProcessAge = DateTime.Now - process.StartTime;
                            Log.Verbose("MCC age: " + MCCProcessAge);
                            if (MCCProcessAge < TimeSpan.FromSeconds(3))
                            {
                                // We don't want to attach on young MCC because of weird issues with LoadLibrary if it's called from multiple threads (within the same process) at once
                                // TODO: Make this less dumb than just picking 3 seconds since some peoples computers are slower/faster than that.
                                Log.Verbose("MCC process too young: " + MCCProcessAge);
                                continue;
                            }
                            else
                            {
                                Log.Verbose("Found mature MCC process, PID: " + process.Id + ", Name: " + process.ProcessName + ", Age: " + MCCProcessAge);
                                return process;
                            }


                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex.Message);
                return null;
            }
            return null;
        }

        // Just check process list for EasyAntiCheat
        private bool AnticheatIsEnabled()
        {
            foreach (Process process in Process.GetProcesses())
            {
                if (String.Equals(process.ProcessName, "EasyAntiCheat",
                 StringComparison.OrdinalIgnoreCase))
                {
                    return true;
                }
            }
            return false;
        }



    }
}
