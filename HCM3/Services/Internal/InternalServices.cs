using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Services.HaloMemory;
using System.Threading;
using System.Diagnostics;
using BurntMemory;
using System.Windows;
using HCM3.Helpers;


namespace HCM3.Services.Internal
{

    public partial class InternalServices
    {
        // Used by LoadInternalData
        private UInt64 ReadPointer(string pointerName, HaloState haloState)
        {
            ReadWrite.Pointer rw_pointer = (ReadWrite.Pointer?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_{pointerName}", haloState) ?? throw new Exception($"Couldn't get pointer to {pointerName}");
            IntPtr pPointer = this.HaloMemoryService.ReadWrite.ResolvePointer(rw_pointer) ?? throw new Exception($"Couldn't read address of {pointerName}");
            return (UInt64)pPointer;
        }



        public Dictionary<string, IntPtr> HCMInternalFunctionPointers { get; set; } = new();
        private List<string> HCMInternalFunctions { get; init; }
        private IntPtr? HCMInternalHandle { get; set; }

        public HaloMemoryService HaloMemoryService { get; init; }
        public DataPointersService DataPointersService { get; init; }

        // Public bools for other services to access to know the state of Internal. Updated by our Handle_Event methods. 
        public bool InternalDLLInjected = false;
        public bool InternalOverlayEnabled = false;

        //Constructor
        public InternalServices(HaloMemoryService haloMemoryService, DataPointersService dataPointersService)
        {
            // Store services needed
            this.HaloMemoryService = haloMemoryService;
            this.HaloMemoryService.InternalServices = this; // halomemory needs a reference to us to grab internal function pointers for some detours (see detourAssemble)
            this.DataPointersService = dataPointersService;

            // Setup the list of HCMInternal functions that we want to call & need to get pointers for
            HCMInternalFunctions = new()
            {
                "InitHCMInternal", // Tell HCM to initialize, passed a string path to where to write it's log file
                "ChangeActiveMessage", // Used to change the active cheat message, like "HCM Active Cheats: Invulnerability"
                "AddTemporaryMessage", // Used to print temporary message to the overlay, like "Checkpoint forced."
                "EnableOverlay", // Used to enable the overlay
                "DisableOverlay", // Used to disable the overlay
                "hkPresent", // We won't call this, but want the games pointerToPresent to it to check if overlay is enabled

                "SetSpeedhack", // Used to change the speedhack's speed
                "SpeedIsUnmodified", // Used to check if the speedhack is "turned off" (speed value is 1.00)

                "ForceLaunch",
                "ForceTeleport",

                //Toggle Display Info cheat
                "EnableDisplayInfo", 
                "DisableDisplayInfo", 
                "IsDisplayInfoEnabled",

                //Toggle Trigger Overlay cheat
                "EnableTriggerOverlay",
                "DisableTriggerOverlay",
                "IsTriggerOverlayEnabled",

                // Load Player Data
                "hkEntityLoop", // Used to get player (or any entity) data
                "IsPlayerDataLoaded", // Tests if playerDataInfo struct is loaded and accurate
                "LoadPlayerData", // Loads playerDataInfo struct into internal

                // Load Trigger Data
                //"hkTriggerSearch", //TODO
                "LoadTriggerData",
                "IsTriggerDataLoaded",

                // Load Camera Data
                "LoadCameraData",
                "IsCameraDataLoaded",

                
            };

            // Subscribe to HaloMemoryService Attach & Detach, and the DisableOverlay settings property-changed
            HaloStateEvents.ATTACHEDHALOSTATE_EVENT += InternalServices_Handle_Attach;
            BurntMemory.Events.DEATTACH_EVENT += InternalServices_Handle_Detach;
            HaloStateEvents.DISABLEOVERLAYCHANGED_EVENT += InternalServices_Handle_DisableOverlayChanged;


        }

        private object InternalServices_Handle_Attach_lock = new object();
        private void InternalServices_Handle_Attach(object? sender, HaloStateEvents.AttachedHaloStateEventArgs e)
        {
            lock (InternalServices_Handle_Attach_lock)
            {
                HaloState haloState = e.FirstHaloState;
                Trace.WriteLine("InternalServices_Handle_Attach invoked, HaloStateInfo:" + haloState.GameState + ", " + haloState.MCCVersion + ", " + this.HaloMemoryService.HaloMemoryManager.MCCType);


                // Spawn new thread to handle all this, since it might take a while if things don't go smoothly
                var t = Task.Run(() =>
                {
                    // Assume nothing is loaded
                    this.HCMInternalHandle = null;
                    this.InternalOverlayEnabled = false;
                    this.InternalDLLInjected = false;
                    // Check if Internal DLL already injected 
                    try
                    {
                        Process MCCProcess = Process.GetProcessById((int)this.HaloMemoryService.HaloMemoryManager.ProcessID);
                        foreach (ProcessModule module in MCCProcess.Modules)
                        {
                            if (module.ModuleName == "HCMInternal.dll")
                            {
                                this.HCMInternalHandle = module.BaseAddress;
                                break;
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        Trace.WriteLine("Something went wrong trying to check if HCMInternal was already injected: " + ex.ToString());
                    }

                    // Try to inject, if not injected
                    int retryCount = 10;
                    while (HCMInternalHandle == null)
                    {
                        // Try to inject
                        try
                        {
                            Process MCCProcess = Process.GetProcessById((int)this.HaloMemoryService.HaloMemoryManager.ProcessID);
                            this.HCMInternalHandle = BurntMemory.DLLInjector.InjectDLL("HCMInternal.dll", MCCProcess);
                        }
                        catch (Exception ex)
                        {
                            Trace.WriteLine("Failed injecting HCMInternal: " + ex.ToString());
                        }

                        Thread.Sleep(50);
                        retryCount--;
                        if (retryCount == 0)
                        {
                            MessageBox.Show("Something went HORRIBLY wrong injecting HCMInternal, some cheat functions such as Speedhack will not be functional. See log file.");
                            return;
                        }
                    }



                    // Evaluate HCMInternal function pointers
                    retryCount = 10;
                    while (!FindInternalFunctions(this.HCMInternalHandle.Value))
                    {
                        Trace.WriteLine("Failed evaluating HCMInternal function pointers, see log file.");

                        Thread.Sleep(50);
                        retryCount--;
                        if (retryCount == 0)
                        {
                            MessageBox.Show("Something went HORRIBLY wrong evaluating HCMInternal function pointers, some cheat functions such as Speedhack will not be functional. See log file.");
                            return;
                        }
                    }

                    // Initialize HCMInternal - need to pass it a path to location of HCMExternal, so it can store log, get shader data etc
                    retryCount = 10;
                    string pathToExternal = System.IO.Directory.GetCurrentDirectory();
                    //pathToInternalLogFile = pathToInternalLogFile.Replace("\\", "/"); // freopen doesn't like forward slashes
                    InternalReturn returnVal = this.CallInternalFunction("InitHCMInternal", pathToExternal);
                    while (returnVal != InternalReturn.True)
                    {

                        Thread.Sleep(50);
                        retryCount--;
                        if (retryCount == 0)
                        {
                            MessageBox.Show("Something went HORRIBLY wrong initializing HCMInternal, some cheat functions such as Speedhack will not be functional. See log file. Path passed: " + pathToExternal + ", return val: " + returnVal);
                            return;
                        }
                        returnVal = this.CallInternalFunction("InitHCMInternal", pathToExternal);
                    }
                    this.InternalDLLInjected = true;

                    HandleOverlayState();

                });

            }

        }

        private void InternalServices_Handle_Detach(object? sender, EventArgs e)
        {
            this.HCMInternalHandle = null;
            this.InternalOverlayEnabled = false;
            this.InternalDLLInjected = false;
        }

        private void InternalServices_Handle_DisableOverlayChanged(object? sender, HaloStateEvents.DisableOverlayChangedEventArgs e)
        {
            HandleOverlayState();
        }

        private bool EnableOverlay()
        {
            Trace.WriteLine("EnableOverlay called");
            IntPtr? GamesPresentPointer = null;
            try
            {
                InternalReturn returnVal = this.CallInternalFunction("EnableOverlay", GetGamesPresentPointer());
                Trace.WriteLine("EnableOverlay returned " + returnVal);
                return returnVal == InternalReturn.True;
            }
            catch (Exception ex)
            {
                Trace.WriteLine("Something went wrong calling EnableOverlay: " + ex.ToString());
                try
                {
                    this.CallInternalFunction("DisableOverlay", GetGamesPresentPointer());
                }
                catch { }
                return false;
            }
        }

        private void DisableOverlay()
        {
            Trace.WriteLine("DisableOverlay called");
            if (this.CallInternalFunction("DisableOverlay", GetGamesPresentPointer()) != InternalReturn.True) throw new Exception("DisableOverlay returned false! We must have sent it an invalid PresentPointer");
        }

        private IntPtr GetGamesPresentPointer()
        {
            HaloState haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
            ReadWrite.Pointer? gamePresentPointerPointer = (ReadWrite.Pointer?)this.DataPointersService.GetRequiredPointers("PresentPointer_" + this.HaloMemoryService.HaloMemoryManager.MCCType, haloState);

            if (gamePresentPointerPointer == null) throw new Exception("Couldn't read pointer to game's Present pointer, HaloStateInfo:" + haloState.GameState + ", " + haloState.MCCVersion + ", " + this.HaloMemoryService.HaloMemoryManager.MCCType);

            IntPtr? gamePresentPointer = this.HaloMemoryService.ReadWrite.ResolvePointer(gamePresentPointerPointer);
            if (gamePresentPointer == null) throw new Exception("Couldn't read the games Present pointer");

            Trace.WriteLine("Passing the following pointer to EnableOverlay: " + gamePresentPointer.Value.ToString("X"));
            return (IntPtr)gamePresentPointer;
        }


        private void HandleOverlayState()
        {

            // If the user wants the overlay to be enabled, enable it!
            if (!Properties.Settings.Default.DisableOverlay)
            {
                bool OverlayEnabled = EnableOverlay();
                int retryCount = 10;
                while (!OverlayEnabled)
                {
                    Thread.Sleep(50);
                    OverlayEnabled = EnableOverlay();
                    retryCount--;
                    if (retryCount < 0) break;
                }

                InternalOverlayEnabled = OverlayEnabled;
                if (OverlayEnabled)
                { 
                    HCM3.Services.Cheats.CheatManagerService.NeedToSendActiveString_Event_INVOKE(this, EventArgs.Empty); // Tell CheatManagerService to call ChangeActiveMessage with whatever the current message should be
                }
                
            }
            else
            {
                try
                {
                    DisableOverlay();
                }
                catch (Exception ex)
                {
                    // We have a serious problem. We have no idea now what state the overlay in since our calls to HCMInternal are failing. For now we'll assume the overlay is disabled.
                    Trace.WriteLine("Tried to disable the overlay but something went seriously wrong! Undefined overlay state, assumed turned off. Ex: " + ex.ToString());
                }
                finally
                {
                    InternalOverlayEnabled = false;
                }
            }

            // Let the user know if something went wrong trying to turn on the overlay
            if (!Properties.Settings.Default.DisableOverlay && !InternalOverlayEnabled)
            {
                MessageBox.Show("Something went wrong trying to enable the overlay, details should be noted in the log file.");
                Properties.Settings.Default.DisableOverlay = true;
            }
        }

    }
}
