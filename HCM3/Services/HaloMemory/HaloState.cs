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

        private HotkeyManager HotkeyManager { get; init; }

        private bool firstCheck = true;
        public HaloState(DataPointersService dataPointersService, HaloMemoryService haloMemoryService, HotkeyManager hotkeyManager)
        {
            var servicecollection = new Microsoft.Extensions.DependencyInjection.ServiceCollection();
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
            this.HotkeyManager = hotkeyManager;

            _currentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;


            // Need to subscribe to AttachStates onAttach/onDetach events to update Halo State
            HaloStateEvents.ATTACH_EVENT += HaloStateEvents_ATTACH_EVENT;
            HaloStateEvents.DEATTACH_EVENT += HaloStateEvents_DEATTACH_EVENT;

            //setup currentHaloState updater timer
            UpdateCurrentHaloStateTimer.Elapsed += new System.Timers.ElapsedEventHandler(this.UpdateHaloState);
            UpdateCurrentHaloStateTimer.Interval = 1000;
            UpdateCurrentHaloStateTimer.Enabled = true;

            //setup gamepad input reader
            gamepadUpdateTimer.Elapsed += GamepadUpdateTimer_Elapsed;
            gamepadUpdateTimer.Interval = 30;
            gamepadUpdateTimer.Enabled = false;




        }

        private void GamepadUpdateTimer_Elapsed(object? sender, System.Timers.ElapsedEventArgs e)
        {
            this.HotkeyManager.gamepad.Update();
            if (!this.HotkeyManager.gamepad.IsConnected)
            {
                this.HotkeyManager.gamepad.Device = XInputium.XInput.XInputDevice.GetFirstConnectedDevice();
            }
        }

        // Timer that will periodically update CurrentHaloState
        private static readonly System.Timers.Timer _updateCurrentHaloStateTimer = new System.Timers.Timer();
        public System.Timers.Timer UpdateCurrentHaloStateTimer
        {
            get { return _updateCurrentHaloStateTimer; }
        }

        private static readonly System.Timers.Timer gamepadUpdateTimer = new System.Timers.Timer();


        private void HaloStateEvents_DEATTACH_EVENT(object? sender, EventArgs e)
        {
            MCCType = null;
            CurrentAttachedMCCVersion = null;
            Trace.WriteLine("MainModel detected BurntMemory DEtach; Set current MCC version to null");
            UpdateHaloState();
            this.HaloMemoryService.HaloState.TryToAttachTimer.Enabled = true;
            gamepadUpdateTimer.Enabled = false;

        }

        private void LoadModulePointers()
        {
            List<int> supportedGames = new() { 0, 1, 2, 3, 4, 5};

            foreach (int supportedGame in supportedGames)
            {
                Trace.WriteLine("attempting to load module pointer for game: " + supportedGame);
                try
                {
                    string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[supportedGame];
                    string gameDLLname = Dictionaries.GameToDLLname[supportedGame];
                    Trace.WriteLine("gameAs2Letters: " + gameAs2Letters + ", gameDLLname: " + gameDLLname);

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
                Trace.WriteLine("Loading a module pointer failed: " + ex.ToString());
                    continue;
                }

            }
        
        }


        private void HaloStateEvents_ATTACH_EVENT(object? sender, Events.AttachedEventArgs e)
        {
            Trace.WriteLine("name of attached processsssss: " + nameOfAttachedProcess);
            if (nameOfAttachedProcess.Equals("MCC-Win64-Shipping", StringComparison.OrdinalIgnoreCase))
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
            gamepadUpdateTimer.Enabled = true;

        }


        public string? MCCType { get; set; }

        private string? _currentLevelCode;
            public string? CurrentLevelCode
        {
            get { return _currentLevelCode; }
            set
            {
                if (_currentLevelCode != value)
                {
                    _currentLevelCode = value;
                    // Raise event
                    Trace.WriteLine("Raising HaloStateChanged on level code changing");
                    HaloStateEvents.HALOSTATECHANGED_EVENT_INVOKE(this, new HaloStateEvents.HaloStateChangedEventArgs(_currentHaloState));
                }
                else
                {
                    _currentLevelCode = value;
                }
            }

        }

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

        public bool OverlayHooked { get; set; } = false;

        public void UpdateHaloState(object? sender, System.Timers.ElapsedEventArgs? args)
        {

            //need to communicate to some other stuff that subscribe to the event the initial state 
            if (firstCheck)
            {
                firstCheck = false;

                this.ForceAttach();
                UpdateHaloState();

                HaloStateEvents.HALOSTATECHANGED_EVENT_INVOKE(this, new HaloStateEvents.HaloStateChangedEventArgs(_currentHaloState));
            }
            else
            {
                UpdateHaloState();
            }


            

        }
        public void UpdateHaloState()
        {
            // If not attached.
            if (!this.Attached || MCCType == null)
            {
                Trace.WriteLine("1");
                CurrentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
                CurrentLevelCode = null;
                return;
            }

            
            ReadWrite.Pointer? gameIndicatorPointer = (ReadWrite.Pointer?)DataPointersService.GetPointer($"MCC_GameIndicator_{MCCType}", this.CurrentAttachedMCCVersion);
            ReadWrite.Pointer? stateIndicatorPointer = (ReadWrite.Pointer?)DataPointersService.GetPointer($"MCC_StateIndicator_{MCCType}", this.CurrentAttachedMCCVersion);
            ReadWrite.Pointer? menuIndicatorPointer = (ReadWrite.Pointer?)DataPointersService.GetPointer($"MCC_MenuIndicator_{MCCType}", this.CurrentAttachedMCCVersion);

            if (gameIndicatorPointer == null || stateIndicatorPointer == null || menuIndicatorPointer == null)
            {
                Trace.WriteLine("one of the three state indicators was null! That's usually not good.");
                CurrentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
                CurrentLevelCode = null;
                return;
            }

            byte? gameIndicator = this.HaloMemoryService.ReadWrite.ReadByte(gameIndicatorPointer);
            byte? stateIndicator = this.HaloMemoryService.ReadWrite.ReadByte(stateIndicatorPointer);
            byte? menuIndicator = this.HaloMemoryService.ReadWrite.ReadByte(menuIndicatorPointer);

            Trace.WriteLine("gameInd: " + gameIndicator.Value.ToString("X"));
            Trace.WriteLine("statInd: " + stateIndicator.Value.ToString("X"));
            Trace.WriteLine("menuInd: " + menuIndicator.Value.ToString("X"));

            if (menuIndicator == 00 && stateIndicator != 44)
            {
                CurrentHaloState = (int)Dictionaries.HaloStateEnum.Menu;
                CurrentLevelCode = null;
            }
            else
            {
                switch (gameIndicator)
                {
                    case 0:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo1;
                        CurrentLevelCode = GetLevelCode(CurrentHaloState);
                        break;

                    case 1:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo2;
                        CurrentLevelCode = GetLevelCode(CurrentHaloState);
                        break;

                    case 2:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo3;
                        CurrentLevelCode = GetLevelCode(CurrentHaloState);
                        break;

                    case 3:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo4;
                        CurrentLevelCode = GetLevelCode(CurrentHaloState);
                        break;

                    case 4: // I think this is the mp version of h2 (specifically h2a, not h2c)
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo2;
                        CurrentLevelCode = GetLevelCode(CurrentHaloState);
                        break;

                    case 5:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo3ODST;
                        CurrentLevelCode = GetLevelCode(CurrentHaloState);
                        break;

                    case 6:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.HaloReach;
                        CurrentLevelCode = GetLevelCode(CurrentHaloState);
                        break;

                    default:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
                        CurrentLevelCode = null;
                        break;

                }
                return;
            }

        }

        public string? GetLevelCode(int game)
        {
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[game];
            try
            {
                ReadWrite.Pointer levelNamePointer = (ReadWrite.Pointer)DataPointersService.GetPointer($"{gameAs2Letters}_LevelCode", this.CurrentAttachedMCCVersion);
                string levelCode = this.HaloMemoryService.ReadWrite.ReadString(levelNamePointer, 64, false);

                while (levelCode.Contains(@"\"))
                {
                    levelCode = levelCode.Substring(levelCode.LastIndexOf(@"\") + 1);
                }
               // Trace.WriteLine("LEVEL CODE: " + levelCode);
                //Trace.WriteLine("Levelcode length: " + levelCode.Length);

                //removes whitespace much stricter than what string.trim does
                levelCode = System.Text.RegularExpressions.Regex.Replace(levelCode, @"[\s\0]+$", "");

               // Trace.WriteLine("LEVEL CODE2: " + levelCode);
                //Trace.WriteLine("Levelcode length2: " + levelCode.Length);

                

                return levelCode;
                
            }
            catch
            {
                return null;

            }
            
        }
    }
}

