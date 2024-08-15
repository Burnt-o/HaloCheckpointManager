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
using NtApiDotNet;
using System.IO;
using HCMExternal.Services.DataPointersServiceNS;

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
        private bool debugPrivilegeEnabled = false;

        // injected service
        private DataPointersService mDataPointersService { get; init; }
        private InterprocService mInterprocService { get; init; }
        private MCCHookStateViewModel mMCCHookStateViewModel { get; init; }
        private ErrorDialogViewModel mErrorDialogViewModel { get; init; }

        // Timer to loop State machine
        private System.Threading.Timer StateMachineLoopTimer;


        // Constructor
        public MCCHookService(InterprocService ips, MCCHookStateViewModel vm, ErrorDialogViewModel errorDialogVM, DataPointersService dps)
        {
            mDataPointersService = dps;
            mInterprocService = ips;
            mMCCHookStateViewModel = vm;
            vm.ShowErrorEvent += ShowHCMInternalErrorDialog;
            
            mErrorDialogViewModel = errorDialogVM;

            // Timer does not start immediately; only when BeginStateMachineLoop is called.
            StateMachineLoopTimer = new System.Threading.Timer(StateMachineLoopEventHandler, null, System.Threading.Timeout.Infinite, 1000);

            debugPrivilegeEnabled = NtToken.EnableDebugPrivilege();
            Log.Information("Debug privilege: " + debugPrivilegeEnabled);
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
            catch(Exception ex)
            {
                lastInjectionError = "HCM had an internal exception in it's MCC Hook State Machine! \n\n" + "Exception details: \n" + ex.Message + "\n" + ex.Source + "\n\n" + "See HCMExternal.log for more info";
                AdvanceStateMachine(MCCHookStateEnum.StateMachineException);
                ShowHCMInternalErrorDialog();
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

        private StringWriter _mccAccessInfo = new();


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
                    {
                        break;
                    }

                    string? mccVersionTemp = null;
                    try
                    {
                        var mccFileName = MCCHookState.MCCProcess.GetImageFilePath(false);
                        Log.Verbose("Getting mcc version information from filepath: "
                            + mccFileName
                            );
                        mccVersionTemp = FileVersionInfo.GetVersionInfo(mccFileName)?.FileVersion;
                        Log.Debug("Detected MCC Version: " + mccVersionTemp);


                        // If winstore, the GetVersionInfo will probably fail to retrieve the FileVersion. But winstore can't downpatch, so we know it's the latest MCC version.
                        if (mccVersionTemp == null && checkProcessType(MCCHookState.MCCProcess) == HaloProcessType.MCCWinstore)
                        {
                            Log.Verbose("WinStore detected, setting mcc version to latest: " + mDataPointersService.HighestSupportedMCCVersion);
                            mccVersionTemp = mDataPointersService.HighestSupportedMCCVersion;
                        }


                    }
                    catch (FileNotFoundException ex)
                    {
                        Log.Error("Could not get file version info of MCC, proceeding anyway. Error: \n" 
                            + ex.Message + "\n" + ex.Source + "\n" + ex.StackTrace);
                    }

                    var mccInit = isMCCInitialised(MCCHookState.MCCProcess, mccVersionTemp);

                    if (mccInit == false)
                    {
                        Log.Verbose("MCC is not properly initialised yet, waiting..");
                    }
                    else if (mccInit == true)
                    {
                        Log.Verbose("MCC is initialised");
                        MCCHookState.MCCVersion = mccVersionTemp;
                        logAccessInformation(MCCHookState.MCCProcess, _mccAccessInfo);
                        AdvanceStateMachine(MCCHookStateEnum.InternalInjecting); // advance to next state (inject HCMInternal)
                    }
                    else
                    {
                        Log.Error("Failed to check MCC initialisation state");
#if !HCM_DEBUG
                        Log.Verbose("But continuing anyway");
                        MCCHookState.MCCVersion = mccVersionTemp;
                        logAccessInformation(MCCHookState.MCCProcess, _mccAccessInfo);                       
                        AdvanceStateMachine(MCCHookStateEnum.InternalInjecting); // advance to next state (inject HCMInternal)
#endif
                    }
                    break;


                case MCCHookStateEnum.MCCAccessError:
                    // Do nothing
                    break;

                case MCCHookStateEnum.MCCEACError:
                    // Do nothing
                    break;

                case MCCHookStateEnum.StateMachineException:
                    // Do nothing
                    break;

                case MCCHookStateEnum.InternalInjecting:
                    var pid = MCCHookState.MCCProcess?.ProcessId;
                    if (pid == null)
                    {
                        lastInjectionError = "HCM failed to inject its internal module into the game! \nMore info in HCMExternal.log file. Error message: \n" + "Process ID was null!"
                        + "\nAccess Rights Debugging: \n" + _mccAccessInfo;
                        AdvanceStateMachine(MCCHookStateEnum.InternalInjectError);
                        ShowHCMInternalErrorDialog();
                        break;
                    }

                    (bool successFlag, string errorString) injectionResult = mInterprocService.Setup((UInt32)pid);
                    

                    if (injectionResult.successFlag)
                    {
                        AdvanceStateMachine(MCCHookStateEnum.InternalInitialising);
                    }
                    else
                    {
                        lastInjectionError = "HCM failed to inject its internal module into the game! \nMore info in HCMExternal.log file. Error message: \n" + injectionResult.errorString
                         + "\nAccess Rights Debugging: \n" + _mccAccessInfo;
                        AdvanceStateMachine(MCCHookStateEnum.InternalInjectError);
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

                    if (currentInternalStateSuccess == InternalStatusFlag.Shutdown || (MCCHookState.MCCProcess == null || MCCHookState.MCCProcess.IsDeleting))
                    {
                        _lastMCCExit = DateTime.Now;
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
            Log.Error(lastInjectionError ?? "No error info to display, for some reason?!");
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

        private NtProcess? GetMCCProcess()
        {
            try
            {

                bool filterToValidMCC(NtProcess process)
                {
                    string[] MCCProcessNames = { "MCC-Win64-Shipping.exe", "MCC-Win64-Winstore-Shipping.exe", "MCCWinstore-Win64-Shipping.exe" };
                    foreach (string desiredProcessName in MCCProcessNames)
                    {

     
                        
                        if (String.Equals(process.Name, desiredProcessName, StringComparison.OrdinalIgnoreCase)) // must be mcc
                        {
                            if (process.IsDeleting)
                            {
                                Log.Debug("Skipping zombie (or terminating) MCC at process ID: " + process.ProcessId);
                                continue;
                            }

                            if (DateTime.Now - process.CreateTime < TimeSpan.FromSeconds(3))
                            {
                                // We don't want to attach on young MCC because of weird issues with LoadLibrary if it's called from multiple threads (within the same process) at once
                                // TODO: Make this less dumb than just picking 3 seconds since some peoples computers are slower/faster than that.
                                Log.Verbose("Skipping super young mcc at id " + process.ProcessId + ", age: " + (DateTime.Now - process.CreateTime));
                                continue;
                            }

                            if (MCCExitedTooRecently())
                                continue;

                            return true;
                        }
                    }
                    return false;
                }


                var validMCCprocesses = NtProcess.GetProcesses(ProcessAccessRights.QueryLimitedInformation).Where(filterToValidMCC);


                if (validMCCprocesses.Any())
                {
                    if (validMCCprocesses.Count() == 1)
                    {
                        return validMCCprocesses.First();
                    }
                    else // more than one, so we want to sort by creation date and grab the youngest.
                    {
                        return validMCCprocesses.OrderByDescending(process => process.CreateTime).First();
                    }
                }
                else
                {
                    Log.Verbose("No process found");
                    return null;
                }
  
            }
            catch (Exception ex)
            {
                Log.Error("Error getting MCC process: " + ex.Message + "\nStacktrace:\n" + ex.StackTrace);
                return null;
            }
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

        // Just logs stuff about the MCC process that might help diagnose Access Denied issues
        private void logAccessInformation(NtProcess mccHandle, TextWriter writer)
        {
            try
            {

                writer.WriteLine("hcm Debug privilege enabled: " + debugPrivilegeEnabled);
                writer.WriteLine("MCC Protected: " + mccHandle.Protected);
                writer.WriteLine("MCC Protected Access: " + NtProcess.TestProtectedAccess(NtProcess.Current, mccHandle));
                writer.WriteLine("MCC Protection.Level: " + mccHandle.Protection.Level);
                writer.WriteLine("MCC Protection.Audit: " + mccHandle.Protection.Audit);

                var mccIntegrity = mccHandle.GetIntegrityLevel(false);
                var hcmIntegrity = NtProcess.Current.GetIntegrityLevel(false);

                writer.WriteLine("hcm IntegrityLevel: " + (hcmIntegrity.IsSuccess ? hcmIntegrity.Result : "Could not read integrity level"));
                writer.WriteLine("MCC IntegrityLevel: " + (mccIntegrity.IsSuccess ? mccIntegrity.Result : "Could not read integrity level"));

                var mccHandleReadControl = NtProcess.Open(mccHandle.ProcessId, ProcessAccessRights.ReadControl, false);
                if (mccHandleReadControl.IsSuccess)
                    writer.WriteLine("MCC maximum access: " + mccHandleReadControl.Result.GetMaximumAccess());
                else
                    writer.WriteLine("Could not check MCC maximum access: " + mccHandleReadControl.Status);

                
            }
            catch (NtException ex)
            {
                writer.WriteLine("error logging access information: " + ex.Message + "\n" + ex.StackTrace);
            }

        }

        private DateTime? _lastMCCExit = null;
        private bool MCCExitedTooRecently()
        {
            if (_lastMCCExit == null) return false;

            Log.Verbose("MCC last exit time was " + (DateTime.Now - _lastMCCExit) + " seconds ago");
            return (DateTime.Now - _lastMCCExit) < TimeSpan.FromSeconds(6);
        }

        private enum HaloProcessType
        {
            MCCSteam,
            MCCWinstore,
            Other,
        };

        private HaloProcessType checkProcessType(NtProcess process)
        {
            switch (process.Name.ToLower())
            {
                case "mcc-win64-shipping.exe":
                    return HaloProcessType.MCCSteam;

                case "mcc-win64-winstore-shipping.exe":
                case "mccwinstore-win64-shipping.exe":
                    return HaloProcessType.MCCWinstore;

                default:
                    return HaloProcessType.Other;
            }
        }


        private bool? isMCCInitialised(NtProcess mccProcess, string? mccVersion)
        {

            string MCCInitFlagPointerString = "MCCInitialisationFlagOffset";
            switch (checkProcessType(mccProcess))
            {
                case HaloProcessType.MCCSteam:
                    MCCInitFlagPointerString = "Steam_" + MCCInitFlagPointerString;
                    break;

                case HaloProcessType.MCCWinstore:
                    MCCInitFlagPointerString = "WinStore_" + MCCInitFlagPointerString;
                    break;

                default:
                    Log.Verbose("Non-MCC Halo process type, assuming initialised");
                    return true;
            }


            if (mccVersion == null)
            {
                Log.Error("Lacked mcc version info, skipping mcc initialising check");
                return null;
            }

            int? MCCInitFlagOffset = (int?)mDataPointersService.GetPointer(MCCInitFlagPointerString, mccVersion);

            if (MCCInitFlagOffset == null)
            {
                Log.Error("Could not resolve mcc initialisation flag");
                return null;
            }

            try
            {
                // need to elevate from QueryLimitedInformation to Read
                using (var procRead = NtProcess.Open(mccProcess.ProcessId, ProcessAccessRights.VmRead | ProcessAccessRights.QueryLimitedInformation))
                {
                    Log.Debug("MCC base address: 0x" + procRead.ImageBaseAddress.ToString("X"));
                    Log.Debug("MCCInitFlagOffset: 0x" + MCCInitFlagOffset.Value.ToString("X"));
                    System.Byte flagValue = procRead.ReadMemory<System.Byte>((long)((ulong)procRead.ImageBaseAddress + (ulong)MCCInitFlagOffset.Value));


                    if (flagValue == 0)
                        return false;
                    if (flagValue == 1)
                        return true;

                    Log.Error("Invalid MCC init flag value: " + flagValue.ToString());
                    return null;
                }
               
            }
            catch(Exception e) 
            { 
            Log.Error("Exception while reading MCC initialisation flag: "+ e.Message);
                return null;
            }



        }

    }
}
