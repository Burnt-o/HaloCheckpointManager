using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using HCM3.ViewModels.Commands;
using HCM3.Services.Trainer;
using BurntMemory;
using HCM3.Services;
using HCM3.Helpers;
using System.Diagnostics;
namespace HCM3.ViewModels
{
    internal class TrainerViewModel : Presenter
    {

        public GenericActionViewModel Button_InjectCheckpoint { get; set; }
        public GenericActionViewModel Button_DumpCheckpoint { get; set; }
        public GenericActionViewModel Button_ForceCheckpoint { get; set; }
        public GenericActionViewModel Button_ForceRevert { get; set; }

        public GenericActionViewModel Button_ForceDoubleRevert { get; set; }
        public GenericActionViewModel Button_ForceCoreSave { get; set; }
        public GenericActionViewModel Button_ForceCoreLoad { get; set; }
        public TeleportViewModel Button_Teleport { get; set; }
        public LaunchViewModel Button_Launch { get; set; }

        public GenericToggleViewModel Button_ToggleInvuln { get; set; }
        public SpeedhackViewModel Button_ToggleSpeedhack { get; set; }
        public GenericToggleViewModel Button_ToggleMedusa { get; set; }
        public GenericToggleViewModel Button_ToggleBool { get; set; }

        public GenericActionViewModel Button_SkullAcrophobia { get; set; }
        public GenericActionViewModel Button_SkullBandana { get; set; }
        public GenericActionViewModel Button_SkullBlind { get; set; }
        public GenericToggleViewModel Button_ToggleNaturals { get; set; }
        public DisplayInfoViewModel Button_ToggleInfo { get; set; }

        public GenericToggleViewModel Button_TogglePanCam { get; set; }

        public GenericToggleViewModel Button_ToggleFlyHack { get; set; }

        public GenericToggleViewModel Button_ToggleSprintMeter { get; set; }
        public GenericToggleViewModel Button_ToggleOHK { get; set; }

        List<ActionToggleBase> listOfButtons { get; set; }

        private string _userControlToShow;
        public string UserControlToShow
        { 
        get { return _userControlToShow; }
            set
            {
                _userControlToShow = value;
                OnPropertyChanged(nameof(UserControlToShow));
            }
        }

        public int SelectedGame { get; set; }
        public bool SelectedGameSameAsActualGame { get; set; }

        public TrainerServices TrainerServices { get; init; }

        public PersistentCheatService PersistentCheatManager { get; init; }

        public DataPointersService DataPointersService { get; init; }

        public HaloMemoryService HaloMemoryService { get; init; }

        // for design mode
        [Obsolete]
        public TrainerViewModel()
        {
            this.Button_InjectCheckpoint = new GenericActionViewModel("Inject Checkpoint", null, null);
            this.Button_DumpCheckpoint = new GenericActionViewModel("Dump Checkpoint", null, null);

            this.Button_ForceCheckpoint = new GenericActionViewModel("Force Checkpoint", null, null);
            this.Button_ForceRevert = new GenericActionViewModel("Force Revert", null, null);
            this.Button_ForceDoubleRevert = new GenericActionViewModel("Force Double Revert", null, null);
            this.Button_ForceCoreSave = new GenericActionViewModel("Force Core save", null, null);
            this.Button_ForceCoreLoad = new GenericActionViewModel("Force Core load", null, null);
            this.Button_Teleport = new TeleportViewModel("Teleport", null, null);
            this.Button_Launch = new LaunchViewModel("Launch", null, null);
            this.Button_ToggleInvuln = new GenericToggleViewModel("Invulnerability", null, null);
            this.Button_ToggleSpeedhack = new SpeedhackViewModel("Speedhack", null, null);
            this.Button_ToggleMedusa = new GenericToggleViewModel("Cheat Medusa", null, null);
            this.Button_ToggleBool = new GenericToggleViewModel("BOOL practice mode", null, null);
            this.Button_SkullAcrophobia = new GenericActionViewModel("Acrophobia Skull", null, null);
            this.Button_SkullBandana = new GenericActionViewModel("Bandana Skull", null, null);
            this.Button_SkullBlind = new GenericActionViewModel("Blind Skull", null, null);
            this.Button_ToggleNaturals = new GenericToggleViewModel("Block Natural CPs", null, null);
            this.Button_ToggleInfo = new DisplayInfoViewModel("Display Info", null, null);
            this.Button_TogglePanCam = new GenericToggleViewModel("PanCam", null, null);
            this.Button_ToggleFlyHack = new GenericToggleViewModel("Fly Hack", null, null);
            this.Button_ToggleSprintMeter = new GenericToggleViewModel("Sprint Meter", null, null);
            this.Button_ToggleOHK = new GenericToggleViewModel("One Hit Kill", null, null);
        }

        private void HaloStateEvents_HALOSTATECHANGED_EVENT(object? sender, HaloStateEvents.HaloStateChangedEventArgs e)
        {

            //this.UserControlToShow = "H1";
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[e.NewHaloState];


                this.UserControlToShow = gameAs2Letters;
            //update IsEnabled of buttons
            List<string> buttonsToShow = new List<string>();
            try
            {
                buttonsToShow = (List<string>)this.DataPointersService.GetPointer($"{gameAs2Letters}_BRP", this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion);
            }
            catch { }

            foreach (ActionToggleBase atb in listOfButtons)
            {
                if (buttonsToShow == null)
                {
                    atb.IsEnabled = false;
                    continue;
                }
                atb.IsEnabled = buttonsToShow.Contains(atb.NameOfBinding);
            }
        }

        public TrainerViewModel(TrainerServices trainerServices, PersistentCheatService persistentCheatManager, CheckpointViewModel checkpointViewModel, HotkeyManager hotkeyManager, DataPointersService dataPointersService, HaloMemoryService haloMemoryService)
        {
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
            this.TrainerServices = trainerServices;
            this.PersistentCheatManager = persistentCheatManager;
            SelectedGame = 0;

            listOfButtons = new();

            

            this.Button_InjectCheckpoint = new GenericActionViewModel(
                "Inject",
                checkpointViewModel.Inject,
                hotkeyManager
                );
            this.Button_InjectCheckpoint.ToolTip = "Injects the currently selected checkpoint into the game. If you enable the \"Auto-Revert on Inject\" option in the settings, then this will also force a revert immediately afterward";
            listOfButtons.Add(this.Button_InjectCheckpoint);


            this.Button_DumpCheckpoint = new GenericActionViewModel(
    "Dump",
    checkpointViewModel.Dump,
    hotkeyManager
    );

            this.Button_DumpCheckpoint.ToolTip = "Backup (\"dump\") the games current checkpoint, saving it to the list on the left. If \"Auto-Checkpoint on Dump\" is enabled, this will force a checkpoint immediately beforehand";
            listOfButtons.Add(this.Button_DumpCheckpoint);

            this.Button_ForceCheckpoint = new GenericActionViewModel(
                "Force Checkpoint", 
                new RelayCommand(o => { TrainerServices.ForceCheckpoint(); }, o => true),
                hotkeyManager
                );

            this.Button_ForceCheckpoint.ToolTip = "Force the game to immediately create a checkpoint";
            listOfButtons.Add(this.Button_ForceCheckpoint);

            this.Button_ForceRevert = new GenericActionViewModel(
                "Force Revert",
    new RelayCommand(o => { TrainerServices.ForceRevert(); }, o => true),
                    hotkeyManager
    );
            this.Button_ForceRevert.ToolTip = "Forcefully reverts to the games last checkpoint";
            listOfButtons.Add(this.Button_ForceRevert);

            this.Button_ForceCoreSave = new GenericActionViewModel(
                "Force Core save",
new RelayCommand(o => { TrainerServices.ForceCoreSave(); }, o => true),
                hotkeyManager
);
            this.Button_ForceCoreSave.ToolTip = "Force the game to make a Core Save (which are very like Checkpoints, but seperate)";
            listOfButtons.Add(this.Button_ForceCoreSave);


            this.Button_ForceCoreLoad = new GenericActionViewModel( 
                "Force Core load",
new RelayCommand(o => { TrainerServices.ForceCoreLoad(); }, o => true),
                hotkeyManager
);

            this.Button_ForceCoreLoad.ToolTip = "Force the game to revert to the last Core Save";
            listOfButtons.Add(this.Button_ForceCoreLoad);

            this.Button_ForceDoubleRevert = new GenericActionViewModel(
                "Force Double Revert",
new RelayCommand(o => { TrainerServices.FlipDoubleRevert(); TrainerServices.ForceRevert(); }, o => true),
                hotkeyManager
);
            this.Button_ForceDoubleRevert.ToolTip = "Revert to the checkpoint that was created BEFORE the last checkpoint you created";
            listOfButtons.Add(this.Button_ForceDoubleRevert);


            this.Button_Teleport = new TeleportViewModel("Teleport", trainerServices, hotkeyManager);
            this.Button_Teleport.ToolTip = "Teleports the player character. Click the \"...\" to configure teleport settings";
            listOfButtons.Add(this.Button_Teleport);

            this.Button_Launch = new LaunchViewModel("Launch", trainerServices, hotkeyManager);
            this.Button_Launch.ToolTip = "Launch the player character forward in the direction they're facing. Click the \"...\" to configure launch velocity settings";
            listOfButtons.Add(this.Button_Launch);


            this.Button_ToggleInvuln = new GenericToggleViewModel("Invulnerability", PersistentCheatManager.PC_Invulnerability, hotkeyManager);
            this.Button_ToggleInvuln.ToolTip = "Toggles invulnerability cheat. This will also make vehicles you enter invulnerable (which includes hijacking!)";
            listOfButtons.Add(this.Button_ToggleInvuln);

            this.Button_ToggleOHK = new GenericToggleViewModel("One Hit Kill", PersistentCheatManager.PC_OneHitKill, hotkeyManager);
            this.Button_ToggleOHK.ToolTip = "Makes any amount of damage, both to you and other NPCs, instantly kill the character. Bit of a meme, but good for testing wave fights";
            listOfButtons.Add(this.Button_ToggleOHK);


            this.Button_ToggleSpeedhack = new SpeedhackViewModel("Speedhack", PersistentCheatManager.PC_Speedhack, hotkeyManager);
            this.Button_ToggleSpeedhack.ToolTip = "Speedhack the game to play at the associated speed. Can cause crashes with very low values";
            listOfButtons.Add(this.Button_ToggleSpeedhack);

            this.Button_ToggleNaturals = new GenericToggleViewModel("Block Natural CPs", PersistentCheatManager.PC_BlockCPs, hotkeyManager);
            this.Button_ToggleNaturals.ToolTip = "Toggles the ability for the game to give you it's own, normal checkpoints";
            listOfButtons.Add(this.Button_ToggleNaturals);



         this.Button_ToggleMedusa = new GenericToggleViewModel("Cheat Medusa", PersistentCheatManager.PC_Medusa, hotkeyManager);
            this.Button_ToggleMedusa.ToolTip = "Toggles cheat_medusa, which makes all enemies instantly die when they become aware of the player";
            listOfButtons.Add(this.Button_ToggleMedusa);

            this.Button_SkullAcrophobia = new GenericActionViewModel("Acrophobia Skull", new RelayCommand(o => { TrainerServices.FlipSkull("Acrophobia"); }, o => true), hotkeyManager);
            this.Button_SkullAcrophobia.ToolTip = "Toggles the Acrophobia skull (jetpack/flyhack)";
            listOfButtons.Add(this.Button_SkullAcrophobia);

            this.Button_SkullBandana = new GenericActionViewModel("Bandana Skull", new RelayCommand(o => { TrainerServices.FlipSkull("Bandana"); }, o => true), hotkeyManager);
            this.Button_SkullBandana.ToolTip = "Toggles the Bandana skull (infinite ammo/grenades)";
            listOfButtons.Add(this.Button_SkullBandana);

            this.Button_SkullBlind = new GenericActionViewModel("Blind Skull", new RelayCommand(o => { TrainerServices.FlipSkull("Blind"); }, o => true), hotkeyManager);
            this.Button_SkullBlind.ToolTip = "Toggles the Blind skull (hide first-person weapon and UI)";
            listOfButtons.Add(this.Button_SkullBlind);

            this.Button_ToggleBool = new GenericToggleViewModel("BOOL practice mode", null, null);
            this.Button_ToggleBool.ToolTip = "Toggle BOOL (Banshee-Out-Of-Level) practice mode for the level Two Betrayals";
            listOfButtons.Add(this.Button_ToggleBool);

            this.Button_TogglePanCam = new GenericToggleViewModel("PanCam", null, null);
            this.Button_TogglePanCam.ToolTip = "Toggle the games \"PanCam\" function (displays co-ordinates)";
            listOfButtons.Add(this.Button_TogglePanCam);


            this.Button_ToggleInfo = new DisplayInfoViewModel("Display Info", PersistentCheatManager.PC_DisplayInfo, hotkeyManager);
            this.Button_ToggleInfo.ToolTip = "Displays useful info about the current gamestate. Click the \"...\" to configure settings";
            listOfButtons.Add(this.Button_ToggleInfo);


            this.Button_ToggleFlyHack = new GenericToggleViewModel("Fly Hack", null, null);
            listOfButtons.Add(this.Button_ToggleFlyHack);

            this.Button_ToggleSprintMeter = new GenericToggleViewModel("Sprint Meter", null, null);
            listOfButtons.Add(this.Button_ToggleSprintMeter );

            UserControlToShow = "LD";
            HaloStateEvents.HALOSTATECHANGED_EVENT += HaloStateEvents_HALOSTATECHANGED_EVENT;
            hotkeyManager.KB_ReloadHotkeys();
        }
    }




}
