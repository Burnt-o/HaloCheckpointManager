using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Helpers;
using System.Diagnostics;
using HCM3.Services.HaloMemory;
using HCM3.Helpers;
using BurntMemory;
using HCM3.ViewModels;
using HCM3.Services.Internal;


namespace HCM3.Services.Cheats
{
    public class ServicesGroup
    {
        public HaloMemoryService HaloMemoryService { get; init; }
        public InternalServices InternalServices { get; init; }
        public DataPointersService DataPointersService { get; init; }
        public CheatManagerService CheatManagerService { get; init; }

        public ServicesGroup(HaloMemoryService haloMemoryService, InternalServices internalServices, DataPointersService dataPointersService, CheatManagerService cheatManagerService)
        { 
        this.HaloMemoryService = haloMemoryService;
            this.InternalServices = internalServices;
            this.DataPointersService = dataPointersService;
            this.CheatManagerService = cheatManagerService;
        }
    }

    public class CheatManagerService
    {
        private HaloMemoryService HaloMemoryService { get; init; }
        private InternalServices InternalServices { get; init; }
        private DataPointersService DataPointersService { get; init; }



        public Dictionary<string, ICheat> ListOfCheats { get; set; }

        public List<string> ListOfActiveCheats = new List<string>();

        public CheatManagerService(HaloMemoryService haloMemoryService, InternalServices internalServices, DataPointersService dataPointersService)
        {
            this.HaloMemoryService = haloMemoryService;
            this.InternalServices = internalServices;
            this.DataPointersService = dataPointersService;


            // Group the services to give to the cheats together just to make things neater
            ServicesGroup servicesGroup = new(haloMemoryService, internalServices, dataPointersService, this);

            //HaloStateEvents.HALOSTATECHANGED_EVENT += HaloStateChanged; // Subscribe so we can update toggle cheat status when halo changes state (ie change game or change level)

            NeedToSendActiveString_Event += SendFirstActiveCheatMessage; // Invoked by Internal Services when the overlay is first enabled so that the current ActiveCheat string can be updated.

            HaloStateEvents.DISABLEOVERLAYCHANGED_EVENT += HaloStateEvents_DISABLEOVERLAYCHANGED_EVENT;

            ListOfCheats = new();
            //Inject &dump - these two need a reference to CheckpointViewModel to grab selectedSaveFolder and selectedCheckpoint
            ListOfCheats.Add("InjectCheckpoint", new InjectCheckpoint(servicesGroup));
            ListOfCheats.Add("DumpCheckpoint", new DumpCheckpoint(servicesGroup));

            // Set bit - cheats that just set a single bit to a specific value
            ListOfCheats.Add("ForceCheckpoint", new GenericSetBit("ForceCheckpoint", "Checkpoint forced.", false, servicesGroup));
            ListOfCheats.Add("ForceRevert", new GenericSetBit("ForceRevert", "Revert forced.", false, servicesGroup));
            ListOfCheats.Add("ForceCoreSave", new GenericSetBit("ForceCoreSave", "Core save forced.", false, servicesGroup));
            ListOfCheats.Add("ForceCoreLoad", new GenericSetBit("ForceCoreLoad", "Core load forced.", false, servicesGroup));

            // Flip bit cheats, mostly skulls
            ListOfCheats.Add("Acrophobia", new GenericFlipBit("SkullAcrophobia", "Acrophobia", false, servicesGroup));
            ListOfCheats.Add("Bandana", new GenericFlipBit("SkullBandana", "Bandana", false, servicesGroup));
            ListOfCheats.Add("Blind", new GenericFlipBit("SkullBlind", "Blind", false, servicesGroup));

            // Need to change the message for this bad boy
            var doubleRevert = new GenericFlipBit("DoubleRevertFlag", "Double Revert", false, servicesGroup);
            doubleRevert.EnabledMessage = "Double Reverted";
            doubleRevert.DisabledMessage = "Double Reverted";
            ListOfCheats.Add("DoubleRevert", doubleRevert);

            // Toggle cheats
            ListOfCheats.Add("BlockCPs", new GenericToggleBit("NaturalCheckpointCode", "Block Checkpoints", true, servicesGroup));
            ListOfCheats.Add("CheatMedusa", new GenericToggleBit("Medusa", "Cheat Medusa", false, servicesGroup));
            ListOfCheats.Add("Invulnerability", new GenericDetourCheat(new List<string> { "Invuln_DetourInfo" }, "Invulnerability enabled.", "Invulnerability disabled", "Invulnerability", servicesGroup));
            ListOfCheats.Add("One Hit Kill", new GenericDetourCheat(new List<string> { "OHK_DetourInfo" }, "One-hit-kill enabled.", "One-hit-kill disabled.", "One-Hit-Kill mode", servicesGroup));

            // More complicated actions
            ListOfCheats.Add("ForceTeleport", new ForceTeleport(servicesGroup));
            ListOfCheats.Add("ForceLaunch", new ForceLaunch(servicesGroup));

            // More complicated toggles
            ListOfCheats.Add("DisplayInfo", new DisplayInfo(servicesGroup));
            ListOfCheats.Add("TriggerOverlay", new TriggerOverlay(servicesGroup));
            ListOfCheats.Add("Speedhack", new Speedhack(servicesGroup));
            //TODO: bool mode

        }

        private void HaloStateEvents_DISABLEOVERLAYCHANGED_EVENT(object? sender, HaloStateEvents.DisableOverlayChangedEventArgs e)
        {
            if (e.NewDisableOverlaySetting == false)
            {
                string message = GetToggleCheatString();
                InternalReturn returnVal = this.InternalServices.CallInternalFunction("ChangeActiveMessage", message);
                if (returnVal != InternalReturn.True) ErrorMessage.Show("Error setting up overlay message, list of active strings might be wrong");
            }
            else
            {
                HaloStateEvents.REMOVEALLTOGGLECHEATSEVENT_INVOKE(this, EventArgs.Empty);
            }
        }






        // Figures out what the message should be based on the AbstractToggleCheat and overlay status,
        // Then handles sending an in-game message (if overlay disabled) or telling Internal to display message (overlay enabled)
        // Returns true if the message was successfully displayed


        internal bool SendToggleCheatMessage(AbstractToggleCheat toggleCheat, HaloState? haloState = null, bool toggledOn = true)
        {
            if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();


        
            if (toggledOn)
            {
                if (!ListOfActiveCheats.Contains(toggleCheat.ActiveMessage)) 
                ListOfActiveCheats.Add(toggleCheat.ActiveMessage);
            }
            else
            {
                if (ListOfActiveCheats.Contains(toggleCheat.ActiveMessage))
                    ListOfActiveCheats.Remove(toggleCheat.ActiveMessage);
            }

            // Can't do shit if unattached
            if (haloState.GameState == GameStateEnum.Unattached) return false;

           

                if (Properties.Settings.Default.DisableOverlay) // Overlay disabled, so send message via in-game-hud-text
            {

                if (haloState.GameState == GameStateEnum.Menu) return true; // Don't bother printing messages in menu, there's no action cheats there anyway

                SendMessage(toggledOn ? toggleCheat.EnabledMessage : toggleCheat.DisabledMessage, haloState);
                return true;

            }
            else // Overlay enabled, so tell internal to display a different message
            {
                // We need to update internals persistent cheat string
                string message = GetToggleCheatString();
                InternalReturn returnVal = this.InternalServices.CallInternalFunction("ChangeActiveMessage", message);
                Trace.WriteLine("SendToggleCheatMessage called ChangeDisplayText, returnVal: " + returnVal + ", message: " + message + ", toggledOn? " + toggledOn);
                return returnVal == InternalReturn.True; //returncode of True means overlay is working and text is displaying
            }
        }

        // Similiar to above but for arbitary messages (one-off messages only)
        public void SendMessage(string message, HaloState? haloState = null)
        {
            if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();

            if (Properties.Settings.Default.DisableOverlay) // Overlay disabled, so send message via in-game-hud-text
            {
                // If game is multiplayer, then we be silent - trick jumpers don't need the messages
                if (haloState.IsMultiplayer() == true)
                {
                    return;
                }

                    bool needToDisableCheckpointText = message == "Checkpoint forced.";
                try
                {
                    PrintHUDMessage(message, haloState, needToDisableCheckpointText);
                }
                catch (Exception ex)
                {
                    Trace.WriteLine("Error printing hud message: message; " + message + ", needToDisableCheckpointText;" + needToDisableCheckpointText);
                }


                
            }

            else // Overlay enabled, so tell internal to display a different message
            {
                // For one off messages, like forcing a checkpoint
                InternalReturn returnVal = this.InternalServices.CallInternalFunction("AddTemporaryMessage", message);
                if (returnVal != InternalReturn.True) throw new Exception("Failed to PrintTemporaryMessageInternal when printing message: " + message + ", returnVal: " + returnVal);
            }

        }

        private string GetToggleCheatString()
        {
            //List<string> activeCheats = new List<string>();
            //foreach (KeyValuePair<string, ICheat> entry in ListOfCheats)
            //{
            //    // We only care about the toggle cheats for this
            //    if (entry.Value.GetType().BaseType != typeof(AbstractToggleCheat)) continue;

            //    AbstractToggleCheat toggleCheat = entry.Value as AbstractToggleCheat;
            //    if (toggleCheat.IsCheatApplied(haloState) || toggleCheat == newToggleCheatToDisplay)
            //    {
            //        activeCheats.Add((string)entry.Key);
            //    }
            //}

            
            string activeCheatsString;
            if (ListOfActiveCheats.Count == 0)
            {
                activeCheatsString = "HCM";
            }
            else if (ListOfActiveCheats.Count == 1)
            {
                activeCheatsString = "HCM Active Cheats: " + ListOfActiveCheats.First();
            }
            else
            {
                activeCheatsString = "HCM Active Cheats: " + string.Join(", ", ListOfActiveCheats);
            }

            return activeCheatsString;
        }


        

        //private void HaloStateChanged(object sender, HaloStateEvents.HaloStateChangedEventArgs e)
        //{
        //    HaloState haloState = e.NewHaloState;


        //    // Not attached, therefore no cheats are applied - go through and uncheck them all.
        //    if (haloState.GameState == GameStateEnum.Unattached)
        //    {
        //        foreach (var cheat in this.ListOfCheats)
        //        {
        //            if (cheat.Value != typeof(AbstractToggleCheat)) continue;
        //            AbstractToggleCheat toggleCheat = cheat.Value as AbstractToggleCheat;
        //            toggleCheat.IsChecked = false;
        //        }
        //        return;
        //    }

        //    // Check that the "IsCheatApplied" and "IsChecked" members have matching values - if not, take steps to rectify.
        //    foreach (var cheat in this.ListOfCheats)
        //    {
        //        if (cheat.Value != typeof(AbstractToggleCheat)) continue;
        //        AbstractToggleCheat toggleCheat = cheat.Value as AbstractToggleCheat;

        //        bool cheatApplied = toggleCheat.IsCheatApplied(haloState); // So we only call IsCheatApplied once, as it's somewhat expensive

        //        if (cheatApplied != toggleCheat.IsChecked) // Uh oh! Mismatch between what user wants and the cheat being applied or not
        //        {
        //            if (toggleCheat.IsChecked) // User wants the cheat applied, but it isn't!
        //            {
        //                // Try to reenable the cheat
        //                if (SendToggleCheatMessage(toggleCheat, haloState, true))
        //                {
        //                    toggleCheat.ApplyCheat(haloState);
        //                }
        //                else
        //                {
        //                    // Failed! Try to disable the cheat and let user know
        //                    try { toggleCheat.RemoveCheat(haloState); } catch { }
        //                    toggleCheat.IsChecked = toggleCheat.IsCheatApplied(haloState);
        //                    SendToggleCheatMessage(toggleCheat, haloState, toggleCheat.IsChecked);
        //                }
        //            }
        //            else // Cheat is turned on, even though the user wants it turned off!
        //            {
        //                try { toggleCheat.RemoveCheat(haloState); } catch { }
        //                toggleCheat.IsChecked = toggleCheat.IsCheatApplied(haloState);
        //                SendToggleCheatMessage(toggleCheat, haloState, toggleCheat.IsChecked);
        //            }
        //        }

        //    }
        //}

        public bool PrintHUDMessage(string? message, HaloState haloState, bool needToDisableCheckpointText = false)
        {
            if (message == null) return false;
            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_CurrentTickCount");
            requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_PrintMessageText");
            requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_PrintMessageTickCount");
            requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_PrintMessageFlags");
            requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_PrintMessageFlagValues");

            Dictionary<string, object> requiredPointers = this.DataPointersService.GetRequiredPointers(requiredPointerNames, haloState);

            byte[] messageData = Encoding.Unicode.GetBytes(message.PadRight(64).Substring(0, 64));

            byte[]? currentTickCount = this.HaloMemoryService.ReadWrite.ReadBytes((ReadWrite.Pointer?)requiredPointers["CurrentTickCount"], 4);

            ReadWrite.Pointer[] printMessageFlagPointers = (ReadWrite.Pointer[])requiredPointers["PrintMessageFlags"];
            byte[] printMessageFlagValues = (byte[])requiredPointers["PrintMessageFlagValues"];

            if (printMessageFlagPointers.Length != printMessageFlagValues.Length)
            {
                throw new Exception("Couldn't print message, FlagPointerCount misaligned with values");
            }


            bool success = false;
            if (needToDisableCheckpointText)
            {
                ReadWrite.Pointer CPMessageCall = (ReadWrite.Pointer)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_CPMessageCall", haloState);
                int CPMessageCallLength = (int)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_CPMessageCallLength", haloState);

                byte[] ogBytes = this.HaloMemoryService.ReadWrite.ReadBytes(CPMessageCall, CPMessageCallLength);

                if (ogBytes == null) throw new Exception("ogBytes was null");

                if (ogBytes[0] != 0x90)
                {
                    byte[] nopBytes = new byte[CPMessageCallLength];
                    Array.Fill(nopBytes, (byte)0x90);
                    this.HaloMemoryService.ReadWrite.WriteBytes(CPMessageCall, nopBytes, true);
                }


                success = this.HaloMemoryService.ReadWrite.WriteBytes((ReadWrite.Pointer)requiredPointers["PrintMessageTickCount"], currentTickCount, false);
                success = success && this.HaloMemoryService.ReadWrite.WriteBytes((ReadWrite.Pointer)requiredPointers["PrintMessageText"], messageData, false);

                for (int i = 0; i < printMessageFlagPointers.Length; i++)
                {
                    success = success && this.HaloMemoryService.ReadWrite.WriteByte(printMessageFlagPointers[i], printMessageFlagValues[i], false);
                }


                new System.Threading.Thread(() =>
                {
                    System.Threading.Thread.CurrentThread.IsBackground = true;
                    System.Threading.Thread.Sleep(50);
                    byte? test = this.HaloMemoryService.ReadWrite.ReadByte(CPMessageCall);
                    if (test == 0x90)
                    {
                        this.HaloMemoryService.ReadWrite.WriteBytes(CPMessageCall, ogBytes, true);
                    }
                }).Start();

            }
            else
            {

                success = this.HaloMemoryService.ReadWrite.WriteBytes((ReadWrite.Pointer)requiredPointers["PrintMessageTickCount"], currentTickCount, false);
                success = success && this.HaloMemoryService.ReadWrite.WriteBytes((ReadWrite.Pointer)requiredPointers["PrintMessageText"], messageData, false);

                for (int i = 0; i < printMessageFlagPointers.Length; i++)
                {

                    success = success && this.HaloMemoryService.ReadWrite.WriteByte(printMessageFlagPointers[i], printMessageFlagValues[i], false);
                }
            }




            return success;

        }


        private void SendFirstActiveCheatMessage(object? sender, EventArgs e)
        {
            HaloState haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
            this.InternalServices.CallInternalFunction("ChangeActiveMessage", GetToggleCheatString());
        }

        // Invoked by Internal Services when the overlay is first enabled so that the current ActiveCheat string can be updated.
        //public class NeedToSendActiveStringArgs : EventArgs
        //{
        //    public NeedToSendActiveStringArgs(HaloState newHaloState) {  }
        //}

        public static event EventHandler<EventArgs>? NeedToSendActiveString_Event;

        public static void NeedToSendActiveString_Event_INVOKE(object sender, EventArgs e)
        {
            EventHandler<EventArgs>? handler = NeedToSendActiveString_Event;
            if (handler != null) handler(sender, e);
        }


    }



}
