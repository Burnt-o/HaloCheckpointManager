using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using System.Diagnostics;
using HCM3.Models;
using HCM3.Helpers;

namespace HCM3.Services.HaloMemory
{
    public static class HaloStateEvents
    {
        // Similiar to HaloStateChangedEvent, but only called on the FIRST HaloState update after we attach to MCC, ie if the previous haloState was null or unattached.
        public class AttachedHaloStateEventArgs : EventArgs
        {
            public AttachedHaloStateEventArgs(HaloState firstHaloState)
            {
                FirstHaloState = firstHaloState;
            }
            public HaloState FirstHaloState { get; init; }
        }

        public static event EventHandler<AttachedHaloStateEventArgs>? ATTACHEDHALOSTATE_EVENT;

        public static void ATTACHEDHALOSTATE_EVENT_INVOKE(object? sender, AttachedHaloStateEventArgs e)
        {
            EventHandler<AttachedHaloStateEventArgs>? handler = ATTACHEDHALOSTATE_EVENT;
            if (handler != null)
            {
                handler(sender, e);
            }
        }




        // HaloStateChangedEvent - invoked when HaloState changes (ie user changed which MCC game, attach or detach, or level changed)
        public class HaloStateChangedEventArgs : EventArgs
        {
            public HaloStateChangedEventArgs(HaloState newHaloState)
            {
                NewHaloState = newHaloState;
            }
            public HaloState NewHaloState { get; init; }
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

        // DisableOverlayChangedEvent - invoked when user toggles the DisableOverlay setting via the Settings tab
        public class DisableOverlayChangedEventArgs : EventArgs
        {
            public bool NewDisableOverlaySetting;
            public DisableOverlayChangedEventArgs(bool newDisableOverlaySetting)
            { 
            this.NewDisableOverlaySetting = newDisableOverlaySetting;
            }
        }

        public static event EventHandler<DisableOverlayChangedEventArgs>? DISABLEOVERLAYCHANGED_EVENT;

        public static void DISABLEOVERLAYCHANGED_EVENT_INVOKE(object? sender, DisableOverlayChangedEventArgs e)
        {
            EventHandler<DisableOverlayChangedEventArgs>? handler = DISABLEOVERLAYCHANGED_EVENT;
            if (handler != null)
            {
                handler(sender, e);
            }
        }


        public static event EventHandler<EventArgs>? REMOVEALLTOGGLECHEATSEVENT;
        public static void REMOVEALLTOGGLECHEATSEVENT_INVOKE(object sender, EventArgs e)
        {
            EventHandler<EventArgs>? handler = REMOVEALLTOGGLECHEATSEVENT;
            if (handler != null)
            {
                handler(sender, e);
            }
        }
    }

    public class HaloMemoryManager : BurntMemory.AttachState
    {

        private DataPointersService DataPointersService { get; init; }
        private HaloMemoryService HaloMemoryService { get; init; } // ref to parent

        private HotkeyManager HotkeyManager { get; init; }

        private bool firstCheck = true;
        public HaloMemoryManager(DataPointersService dataPointersService, HaloMemoryService haloMemoryService, HotkeyManager hotkeyManager)
        {
            var servicecollection = new Microsoft.Extensions.DependencyInjection.ServiceCollection();
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
            this.HotkeyManager = hotkeyManager;


            //Init HaloState to unattached
            CurrentHaloState = new HaloState(GameStateEnum.Unattached, null, null);


            // Need to subscribe to AttachStates onAttach/onDetach events to update Halo State
            BurntMemory.Events.ATTACH_EVENT += HaloStateEvents_ATTACH_EVENT;
            BurntMemory.Events.DEATTACH_EVENT += HaloStateEvents_DEATTACH_EVENT;

            //setup currentHaloState updater timer
            UpdateCurrentHaloStateTimer.Elapsed += new System.Timers.ElapsedEventHandler(this.UpdateHaloStateTimerElapsed);
            UpdateCurrentHaloStateTimer.Interval = 1000;
            UpdateCurrentHaloStateTimer.Enabled = true;

            //setup gamepad input reader
            gamepadUpdateTimer.Elapsed += GamepadUpdateTimer_Elapsed;
            gamepadUpdateTimer.Interval = 30;
            gamepadUpdateTimer.Enabled = false;




        }

        private bool firstGamePadUpdateError = true;
        private void GamepadUpdateTimer_Elapsed(object? sender, System.Timers.ElapsedEventArgs e)
        {
            try
            {
                this.HotkeyManager.gamepad.Update();
                if (!this.HotkeyManager.gamepad.IsConnected)
                {
                    this.HotkeyManager.gamepad.Device = XInputium.XInput.XInputDevice.GetFirstConnectedDevice();
                }
            }
            catch (Exception ex)
            {
                // I don't want this to spam the log with error messages if it fails every 30ms
                if (firstGamePadUpdateError)
                {
                    firstGamePadUpdateError = false;
                    Trace.WriteLine("An error occured trying to update the state of the gamepad; " + ex.ToString());
                }
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
            this.HaloMemoryService.HaloMemoryManager.TryToAttachTimer.Enabled = true;
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

                    string gameDLLname = Dictionaries.GameToDLLname[supportedGame];
                    Trace.WriteLine("game: " + (GameStateEnum)supportedGame + ", gameDLLname: " + gameDLLname);

                    ReadWrite.Pointer? modulePointer = (ReadWrite.Pointer?)this.DataPointersService.GetPointer($"{((GameStateEnum)supportedGame).To2Letters()}_Module{MCCType}", CurrentAttachedMCCVersion);
                    if (modulePointer == null)
                    {
                        Trace.WriteLine("module pointer was null somehow? " + (GameStateEnum)supportedGame + ", MCCType: " + MCCType + ", CurrentAttachedMCCVersion: " + CurrentAttachedMCCVersion);
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
                string? potentialVersion = this.HaloMemoryService.HaloMemoryManager.ProcessVersion;
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



       

        public string? CurrentAttachedMCCVersion { get; set; }


        public void UpdateHaloStateTimerElapsed(object? sender, System.Timers.ElapsedEventArgs? args)
        {

            //need to communicate to some other stuff that subscribe to the event the initial state 
            if (firstCheck)
            {
                firstCheck = false;

                this.ForceAttach();

                // Force invoke this event just the first time
                Trace.WriteLine("Invoking HaloStateChangedEvent for first time. This should only run once.");
                HaloStateEvents.HALOSTATECHANGED_EVENT_INVOKE(this, new HaloStateEvents.HaloStateChangedEventArgs(UpdateHaloState()));
            }
            else
            {
                UpdateHaloState();
            }


            

        }


        private byte? lastgameIndicator;
        private byte? laststateIndicator;
        private byte? lastmenuIndicator;

        private HaloState _currentHaloState = new HaloState(GameStateEnum.Unattached, null, null);
        public HaloState CurrentHaloState
        {
            get
            {
                return _currentHaloState;
            }
            set
            {

                if (!_currentHaloState.Equals(value))
                {
                    HaloState oldHaloState = _currentHaloState;
                    Trace.WriteLine("HaloState changed! Invoking change event. Old gamestate: " + _currentHaloState?.GameState + ", New gamestate: " + value?.GameState + "\nOld levelcode: " + _currentHaloState?.LevelInfo?.LevelCode + ", New levelcode: " + value?.LevelInfo?.LevelCode);

                    _currentHaloState = value;
                    // If previous state was unattached, raise the AttachedHaloState event
                    if (oldHaloState?.GameState == GameStateEnum.Unattached)
                    {
                        HaloStateEvents.ATTACHEDHALOSTATE_EVENT_INVOKE(this, new HaloStateEvents.AttachedHaloStateEventArgs(_currentHaloState));
                    }

                    // Raise event
                    HaloStateEvents.HALOSTATECHANGED_EVENT_INVOKE(this, new HaloStateEvents.HaloStateChangedEventArgs(_currentHaloState));
                }
                else
                {
                    _currentHaloState = value;
                }
            }
        }

        public HaloState UpdateHaloState()
        {


            // If not attached.
            if (!this.Attached || MCCType == null)
            {
                return CurrentHaloState = new HaloState(GameStateEnum.Unattached, null, null);
            }
            
            ReadWrite.Pointer? gameIndicatorPointer = (ReadWrite.Pointer?)DataPointersService.GetPointer($"MCC_GameIndicator_{MCCType}", this.CurrentAttachedMCCVersion);
            ReadWrite.Pointer? stateIndicatorPointer = (ReadWrite.Pointer?)DataPointersService.GetPointer($"MCC_StateIndicator_{MCCType}", this.CurrentAttachedMCCVersion);
            ReadWrite.Pointer? menuIndicatorPointer = (ReadWrite.Pointer?)DataPointersService.GetPointer($"MCC_MenuIndicator_{MCCType}", this.CurrentAttachedMCCVersion);

            if (gameIndicatorPointer == null || stateIndicatorPointer == null || menuIndicatorPointer == null)
            {
                Trace.WriteLine("Couldn't get pointers for the gameState indicators");
                return CurrentHaloState = new HaloState(GameStateEnum.Unattached, null, null);
            }

            byte? gameIndicator = this.HaloMemoryService.ReadWrite.ReadByte(gameIndicatorPointer);
            byte? stateIndicator = this.HaloMemoryService.ReadWrite.ReadByte(stateIndicatorPointer);
            byte? menuIndicator = this.HaloMemoryService.ReadWrite.ReadByte(menuIndicatorPointer);

            if (gameIndicator == null || stateIndicator == null || menuIndicator == null)
            {
                Trace.WriteLine("Couldn't read the values for the gameState indicators");
                return CurrentHaloState = new HaloState(GameStateEnum.Unattached, null, null);
            }

            // Only log this info if there's actually been a change
            if (lastgameIndicator != gameIndicator || laststateIndicator != stateIndicator || lastmenuIndicator != menuIndicator)
            {
                Trace.WriteLine("gameInd: " + gameIndicator.Value.ToString("X"));
                Trace.WriteLine("statInd: " + stateIndicator.Value.ToString("X"));
                Trace.WriteLine("menuInd: " + menuIndicator.Value.ToString("X"));
            }

            lastgameIndicator = gameIndicator;
            laststateIndicator = stateIndicator;
            lastmenuIndicator = menuIndicator;


            // MCC is in the main menu
            if (menuIndicator == 00 && stateIndicator != 44)
            {
                return CurrentHaloState = new HaloState(GameStateEnum.Menu, null, this.CurrentAttachedMCCVersion);
            }
            else
            {
                switch (gameIndicator)
                {
                    case 0:
                        return CurrentHaloState = new HaloState(GameStateEnum.Halo1, GetLevelInfo(GameStateEnum.Halo1), this.CurrentAttachedMCCVersion);

                        break;

                    case 1:
                        return CurrentHaloState = new HaloState(GameStateEnum.Halo2, GetLevelInfo(GameStateEnum.Halo2), this.CurrentAttachedMCCVersion);
                        break;

                    case 2:
                        return CurrentHaloState = new HaloState(GameStateEnum.Halo3, GetLevelInfo(GameStateEnum.Halo3), this.CurrentAttachedMCCVersion);
                        break;

                    case 3:
                        return CurrentHaloState = new HaloState(GameStateEnum.Halo4, GetLevelInfo(GameStateEnum.Halo4), this.CurrentAttachedMCCVersion);
                        break;

                    case 4: // I think this is the mp version of h2 (specifically h2a, not h2c)
                        return CurrentHaloState = new HaloState(GameStateEnum.Halo2, GetLevelInfo(GameStateEnum.Halo2), this.CurrentAttachedMCCVersion);
                        break;

                    case 5:
                        return CurrentHaloState = new HaloState(GameStateEnum.Halo3ODST, GetLevelInfo(GameStateEnum.Halo3ODST), this.CurrentAttachedMCCVersion);
                        break;

                    case 6:
                        return CurrentHaloState = new HaloState(GameStateEnum.HaloReach, GetLevelInfo(GameStateEnum.HaloReach), this.CurrentAttachedMCCVersion);
                        break;

                    default:
                        return CurrentHaloState = new HaloState(GameStateEnum.Unattached, null, null);
                        break;

                }
            }

        }

        public LevelInfo? GetLevelInfo(GameStateEnum gameState)
        {
            try
            {
                ReadWrite.Pointer levelNamePointer = (ReadWrite.Pointer)DataPointersService.GetPointer($"{gameState.To2Letters()}_LevelCode", this.CurrentAttachedMCCVersion);
                string levelCode = this.HaloMemoryService.ReadWrite.ReadString(levelNamePointer, 64, false);

                while (levelCode.Contains(@"\"))
                {
                    levelCode = levelCode.Substring(levelCode.LastIndexOf(@"\") + 1);

                    // Removes whitespace (and null characters) much stricter than what string.trim does
                    levelCode = System.Text.RegularExpressions.Regex.Replace(levelCode, @"[\s\0]+$", "");
                }

                // There's a different one for each game, just to make my life easier
                Dictionary<string, LevelInfo> levelInfoGetter = Dictionaries.GameToLevelInfoDictionary[(int)gameState];

                if (levelInfoGetter.ContainsKey(levelCode))
                {
                    return levelInfoGetter[levelCode];
                }
                else
                {
                    if (levelCode.Length == 0 || !levelCode.Any(char.IsLetterOrDigit)) return null; // empty levelcode so return null. Often happens when in loading screen. 
                    // Better than nothing
                    return new LevelInfo(levelCode, null, null);
                }

            }
            catch (Exception ex)
            {
                Trace.WriteLine("Couldn't read the games level info: " + ex.ToString());
                throw ex;
                return null;
            }
        }

    }
}

