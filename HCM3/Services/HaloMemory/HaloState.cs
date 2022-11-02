using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using System.Diagnostics;
using HCM3.Models;
using HCM3.Helpers;

namespace HCM3.Services
{
    public class HaloStateEvents : BurntMemory.Events
    {
        public class HaloStateChangedEventArgs : EventArgs
        {
            public HaloStateChangedEventArgs(int newHaloState)
            {
                NewHaloState = newHaloState;
            }
            public int NewHaloState { get; init; }
        }



        public static event EventHandler<HaloStateChangedEventArgs>? HALOSTATECHANGED_EVENT;

        public static void HALOSTATECHANGED_EVENT_INVOKE(object? sender, HaloStateChangedEventArgs e)
        {
            EventHandler<HaloStateChangedEventArgs>? handler = HALOSTATECHANGED_EVENT;
            if (handler != null)
            {
                handler(sender, e);
            }
        }
    }

    public class HaloState : BurntMemory.AttachState
    {

        private DataPointersService DataPointersService { get; init; }
        private HaloMemoryService HaloMemoryService { get; init; } //self ref? 
        public HaloState(DataPointersService dataPointersService, HaloMemoryService haloMemoryService)
        {
            var servicecollection = new Microsoft.Extensions.DependencyInjection.ServiceCollection();
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
            _currentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;


            // Need to subscribe to AttachStates onAttach/onDetach events to update Halo State
            HaloStateEvents.ATTACH_EVENT += HaloStateEvents_ATTACH_EVENT;
            HaloStateEvents.DEATTACH_EVENT += HaloStateEvents_DEATTACH_EVENT;

            //setup currentHaloState updater timer
            UpdateCurrentHaloStateTimer.Elapsed += new System.Timers.ElapsedEventHandler(this.UpdateHaloState);
            UpdateCurrentHaloStateTimer.Interval = 1000;
            UpdateCurrentHaloStateTimer.Enabled = true;
        }

        // Timer that will periodically update CurrentHaloState
        private static readonly System.Timers.Timer _updateCurrentHaloStateTimer = new System.Timers.Timer();
        public System.Timers.Timer UpdateCurrentHaloStateTimer
        {
            get { return _updateCurrentHaloStateTimer; }
        }

        private void HaloStateEvents_DEATTACH_EVENT(object? sender, EventArgs e)
        {
            MCCType = null;
            CurrentAttachedMCCVersion = null;
            Trace.WriteLine("MainModel detected BurntMemory DEtach; Set current MCC version to null");
            UpdateHaloState();
            this.HaloMemoryService.HaloState.TryToAttachTimer.Enabled = true;

        }

        private void LoadModulePointers()
        {
            List<int> supportedGames = new() { 0, 1, 2, 3, 4, 5};

            foreach (int supportedGame in supportedGames)
            {
                try
                {
                    string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[supportedGame];
                    string gameDLLname = Dictionaries.GameToDLLname[supportedGame];

                    ReadWrite.Pointer? modulePointer = (ReadWrite.Pointer?)this.DataPointersService.GetPointer($"{gameAs2Letters}_Module{MCCType}", CurrentAttachedMCCVersion);
                    if (modulePointer == null)
                    {
                        Trace.WriteLine("module pointer was null somehow? " + gameAs2Letters);
                        continue;
                    }
                    this.SetModulePointer(gameDLLname, modulePointer);
                }
                catch (Exception ex)
                { 
                Trace.WriteLine("Loading a module pointer failed: " + ex.Message);
                    continue;
                }

            }
        
        }


        private void HaloStateEvents_ATTACH_EVENT(object? sender, Events.AttachedEventArgs e)
        {
            Trace.WriteLine("name of attached processsssss: " + nameOfAttachedProcess);
            if (nameOfAttachedProcess == "MCC-Win64-Shipping")
            {
                MCCType = "Steam";
                string? potentialVersion = this.HaloMemoryService.HaloState.ProcessVersion;
                if (potentialVersion != null && potentialVersion.StartsWith("1."))
                {
                    CurrentAttachedMCCVersion = potentialVersion;
                }
                else
                {
                    CurrentAttachedMCCVersion = null;
                }
                Trace.WriteLine("MainModel detected BurntMemory attach; Set current MCC version to " + CurrentAttachedMCCVersion);


            }
            else
            {
                MCCType = "WinStore";
                CurrentAttachedMCCVersion = DataPointersService.HighestSupportedMCCVersion;
            }
            LoadModulePointers();
            UpdateHaloState();


        }


        public string? MCCType { get; set; }


        private int _currentHaloState;
        public int CurrentHaloState
        {
            get
            {
                return _currentHaloState;
            }
            set
            {

                if (_currentHaloState != value)
                {
                    _currentHaloState = value;
                    // Raise event
                    HaloStateEvents.HALOSTATECHANGED_EVENT_INVOKE(this, new HaloStateEvents.HaloStateChangedEventArgs(_currentHaloState));
                }
                else
                {
                    _currentHaloState = value;
                }




            }
        }

        public string? CurrentAttachedMCCVersion { get; set; }



        public int GetHaloState(bool updateBeforeGet = true)
        {
            if (updateBeforeGet)
            {
                UpdateHaloState();
            }
            return CurrentHaloState;
        }

        public void UpdateHaloState(object? sender, System.Timers.ElapsedEventArgs? args)
        {
            //Don't care about those args
            UpdateHaloState();
        }
        public void UpdateHaloState()
        {
            // If not attached.
            if (!this.Attached || MCCType == null)
            {
                Trace.WriteLine("1");
                CurrentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
                return;
            }

            
            ReadWrite.Pointer? gameIndicatorPointer = (ReadWrite.Pointer?)DataPointersService.GetPointer($"MCC_GameIndicator_{MCCType}", this.CurrentAttachedMCCVersion);
            ReadWrite.Pointer? stateIndicatorPointer = (ReadWrite.Pointer?)DataPointersService.GetPointer($"MCC_StateIndicator_{MCCType}", this.CurrentAttachedMCCVersion);
            ReadWrite.Pointer? menuIndicatorPointer = (ReadWrite.Pointer?)DataPointersService.GetPointer($"MCC_MenuIndicator_{MCCType}", this.CurrentAttachedMCCVersion);

            if (gameIndicatorPointer == null || stateIndicatorPointer == null || menuIndicatorPointer == null)
            {
                Trace.WriteLine("AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH");
                CurrentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
                return;
            }

            byte? gameIndicator = this.HaloMemoryService.ReadWrite.ReadByte(gameIndicatorPointer);
            byte? stateIndicator = this.HaloMemoryService.ReadWrite.ReadByte(stateIndicatorPointer);
            byte? menuIndicator = this.HaloMemoryService.ReadWrite.ReadByte(menuIndicatorPointer);

            Trace.WriteLine("gameInd: " + gameIndicator.ToString());
            Trace.WriteLine("stateInd: " + stateIndicator.ToString());
            Trace.WriteLine("menuInd: " + menuIndicator.ToString());

            if (menuIndicator == 00 && stateIndicator != 44)
            {
                CurrentHaloState = (int)Dictionaries.HaloStateEnum.Menu;
            }
            else
            {
                switch (gameIndicator)
                {
                    case 0:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo1;
                        break;

                    case 1:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo2;
                        break;

                    case 2:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo3;
                        break;

                    case 3:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo4;
                        break;

                    case 4: // I think this is the mp version of h2 (specifically h2a, not h2c)
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo2;
                        break;

                    case 5:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo3ODST;
                        break;

                    case 6:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.HaloReach;
                        break;

                    default:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
                        break;

                }
                return;
            }

        }


    }
}

