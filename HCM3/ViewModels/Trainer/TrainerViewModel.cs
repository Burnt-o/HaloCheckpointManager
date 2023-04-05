using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using HCM3.ViewModels.Commands;
using HCM3.Services.Cheats;
using BurntMemory;
using HCM3.Services;
using HCM3.Helpers;
using System.Diagnostics;
using HCM3.Services.HaloMemory;
using System.Collections.ObjectModel;
using HCM3.Views.Controls.Trainer.ButtonViews.ButtonOptions;



namespace HCM3.ViewModels
{
    internal class TrainerViewModel : Presenter
    {

        public ObservableDictionary<string, CheatViewModel> CheatViewModels { get; init; }

        //public GenericActionViewModel Button_InjectCheckpoint { get; set; }
        //public GenericActionViewModel Button_DumpCheckpoint { get; set; }
        //public GenericActionViewModel Button_ForceCheckpoint { get; set; }
        //public GenericActionViewModel Button_ForceRevert { get; set; }

        //public GenericActionViewModel Button_ForceDoubleRevert { get; set; }
        //public GenericActionViewModel Button_ForceCoreSave { get; set; }
        //public GenericActionViewModel Button_ForceCoreLoad { get; set; }
        //public TeleportViewModel Button_Teleport { get; set; }
        //public LaunchViewModel Button_Launch { get; set; }

        //public GenericToggleViewModel Button_ToggleInvuln { get; set; }
        //public SpeedhackViewModel Button_ToggleSpeedhack { get; set; }
        //public GenericToggleViewModel Button_ToggleMedusa { get; set; }
        //public GenericToggleViewModel Button_ToggleBool { get; set; }

        //public GenericActionViewModel Button_SkullAcrophobia { get; set; }
        //public GenericActionViewModel Button_SkullBandana { get; set; }
        //public GenericActionViewModel Button_SkullBlind { get; set; }
        //public GenericToggleViewModel Button_ToggleNaturals { get; set; }
        //public DisplayInfoViewModel Button_ToggleInfo { get; set; }

        //public GenericToggleViewModel Button_TogglePanCam { get; set; }

        //public GenericToggleViewModel Button_ToggleFlyHack { get; set; }

        //public GenericToggleViewModel Button_ToggleSprintMeter { get; set; }
        //public GenericToggleViewModel Button_ToggleOHK { get; set; }
        //public GenericToggleViewModel Button_ToggleTheaterFriend { get; set; }

        public List<ActionToggleBase> ListOfButtons { get; set; } = new();


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



        public CheatManagerService? CheatManagerService { get; init; }

        public DataPointersService? DataPointersService { get; init; }

        public HaloMemoryService? HaloMemoryService { get; init; }

        // for design mode
        [Obsolete]
        public TrainerViewModel()
        {
            _userControlToShow = "H1";

            CheatViewModels = new();
            CheatViewModels.Add("InjectCheckpoint", new CheatViewModel("Inject Killme"));
            CheatViewModels.Add("DumpCheckpoint", new CheatViewModel("Dump Checkpoint"));

            //this.ListOfButtons.Add(new GenericActionViewModel("Inject Checkpoint", null, null));
            //this.ListOfButtons.Add(new GenericActionViewModel("Dump Checkpoint", null, null));
            //this.ListOfButtons.Add(new GenericActionViewModel("Force Checkpoint", null, null));
            //this.ListOfButtons.Add(new GenericActionViewModel("Force Revert", null, null));
            //this.ListOfButtons.Add(new GenericActionViewModel("Force Double Revert", null, null));
            //this.ListOfButtons.Add(new GenericActionViewModel("Force Core save", null, null));
            //this.ListOfButtons.Add(new GenericActionViewModel("Force Core load", null, null));
            //this.ListOfButtons.Add(new TeleportViewModel("Teleport", null, null));
            //this.ListOfButtons.Add(new LaunchViewModel("Launch", null, null));
            //this.ListOfButtons.Add(new GenericToggleViewModel("Invulnerability", "Invulnerability", null, null));
            //this.ListOfButtons.Add(new SpeedhackViewModel("Speedhack", null, null));
            //this.ListOfButtons.Add(new GenericToggleViewModel("Cheat Medusa", "CheatMedusa", null, null));
            //this.ListOfButtons.Add(new GenericToggleViewModel("BOOL practice mode", "BOOLmode", null, null));
            //this.ListOfButtons.Add(new GenericActionViewModel("Acrophobia Skull", null, null));
            //this.ListOfButtons.Add(new GenericActionViewModel("Bandana Skull", null, null));
            //this.ListOfButtons.Add(new GenericActionViewModel("Blind Skull", null, null));
            //this.ListOfButtons.Add(new GenericToggleViewModel("Block Natural CPs", "BlockCPs", null, null));
            //this.ListOfButtons.Add(new DisplayInfoViewModel("Display Info", null, null));
            //this.ListOfButtons.Add(new GenericToggleViewModel("PanCam", "PanCam", null, null));
            //this.ListOfButtons.Add(new GenericToggleViewModel("Fly Hack", "FlyHack", null, null));
            //this.ListOfButtons.Add(new GenericToggleViewModel("Sprint Meter", "SprintMeter", null, null));
            //this.ListOfButtons.Add(new GenericToggleViewModel("One Hit Kill", "One Hit Kill", null, null));
            //this.ListOfButtons.Add(new GenericToggleViewModel("Theater Friend", "Theater Friend", null, null));
        }

        private void HaloStateEvents_HALOSTATECHANGED_EVENT(object? sender, HaloStateEvents.HaloStateChangedEventArgs e)
        {

            HaloState haloState = e.NewHaloState;


            this.UserControlToShow = haloState.GameState.To2Letters();

            List<string> buttonsToShow = new List<string>();
            try
            {
                buttonsToShow = (List<string>)this.DataPointersService.GetPointer($"{haloState.GameState.To2Letters()}_BRP", haloState.MCCVersion);
            }
            catch { }

            foreach (ActionToggleBase atb in ListOfButtons)
            {
                if (buttonsToShow == null)
                {
                    atb.IsEnabled = false;
                    continue;
                }
                atb.IsEnabled = buttonsToShow.Contains(atb.NameOfBinding);
            }
        }


        public TrainerViewModel(CheatManagerService cheatManagerService, CheckpointViewModel checkpointViewModel, HotkeyManager hotkeyManager, DataPointersService dataPointersService, HaloMemoryService haloMemoryService)
        {
            this.HaloMemoryService = haloMemoryService;
            this.CheatManagerService = cheatManagerService;
            this.DataPointersService = dataPointersService;
            SelectedGame = 0;


            CheatViewModels = new();
            CheatViewModels.Add("DumpCheckpoint", new CheatViewModel("Dump Checkpoint", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("DumpCheckpoint"), null, null, null, cheatManagerService));
            CheatViewModels.Add("InjectCheckpoint", new CheatViewModel("Inject Checkpoint", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("InjectCheckpoint"), null, null, null, cheatManagerService));
            CheatViewModels.Add("ForceCheckpoint", new CheatViewModel("Force Checkpoint", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("ForceCheckpoint"), null, null, null, cheatManagerService));
            CheatViewModels.Add("ForceRevert", new CheatViewModel("Force Revert", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("ForceRevert"), null, null, null, cheatManagerService));
            CheatViewModels.Add("ForceCoreSave", new CheatViewModel("Force Core Save", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("ForceCoreSave"), null, null, null, cheatManagerService));
            CheatViewModels.Add("ForceCoreLoad", new CheatViewModel("Force Core Load", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("ForceCoreLoad"), null, null, null, cheatManagerService));

            var forceTeleportSettings = new List<Tuple<string, bool>> { new Tuple<string, bool>("TeleportModeForward", false), new Tuple<string, bool>("TeleportIgnoreZ", false), new Tuple<string, bool>("TeleportX", false), new Tuple<string, bool>("TeleportY", false), new Tuple<string, bool>("TeleportZ", false), new Tuple<string, bool>("TeleportLength", false) };
            CheatViewModels.Add("ForceTeleport", new CheatViewModel("Teleport FIX ME", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("ForceTeleport"), forceTeleportSettings, null, typeof(TeleportOptionsView), cheatManagerService));

            var forceLaunchSettings = new List<Tuple<string, bool>> { new Tuple<string, bool>("LaunchSpeed", false) };
            CheatViewModels.Add("ForceLaunch", new CheatViewModel("Boost Player", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("ForceLaunch"), forceLaunchSettings, null, typeof(LaunchOptionsView), cheatManagerService));

            CheatViewModels.Add("Invulnerability", new CheatViewModel("Invulnerability", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("Invulnerability"), null, null, null, cheatManagerService));

            var speedhackSettings = new List<Tuple<string, bool>>  { new Tuple<string, bool>("SpeedHackValue", false) };
            CheatViewModels.Add("Speedhack", new CheatViewModel("Speedhack", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("Speedhack"), speedhackSettings, null, null, cheatManagerService));
            CheatViewModels.Add("CheatMedusa", new CheatViewModel("Cheat Medusa", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("CheatMedusa"), null, null, null, cheatManagerService));

            CheatViewModels.Add("BOOLmode", new CheatViewModel("BOOLmode", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("BOOLmode"), null, null, null, cheatManagerService));
            CheatViewModels.Add("Acrophobia", new CheatViewModel("Acrophobia", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("Acrophobia"), null, null, null, cheatManagerService));
            CheatViewModels.Add("Bandana", new CheatViewModel("Bandana", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("Bandana"), null, null, null, cheatManagerService));
            CheatViewModels.Add("Blind", new CheatViewModel("Blind", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("Blind"), null, null, null, cheatManagerService));
            CheatViewModels.Add("BlockCPs", new CheatViewModel("Block Natural Checkpoints", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("BlockCPs"), null, null, null, cheatManagerService));

            var displayInfohSettings = new List<Tuple<string, bool>> { new Tuple<string, bool>("DIFontSize", false), new Tuple<string, bool>("DIScreenX", false), new Tuple<string, bool>("DIScreenY", false), new Tuple<string, bool>("DISigDig", false) };
            CheatViewModels.Add("DisplayInfo", new CheatViewModel("Display Info", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("DisplayInfo"), displayInfohSettings, null, typeof(DisplayInfoOptionsView), cheatManagerService));
            CheatViewModels.Add("TriggerOverlay", new CheatViewModel("Trigger Overlay", hotkeyManager, cheatManagerService.ListOfCheats.GetValueOrDefault("TriggerOverlay"), null, null, null, cheatManagerService));

            UserControlToShow = "LD";
            HaloStateEvents.HALOSTATECHANGED_EVENT += HaloStateEvents_HALOSTATECHANGED_EVENT;
            hotkeyManager.KB_ReloadHotkeys();

        }

        //        public TrainerViewModel(CheatManagerService cheatManagerService, CheckpointViewModel checkpointViewModel, HotkeyManager hotkeyManager, DataPointersService dataPointersService, HaloMemoryService haloMemoryService)
        //        {
        //            this.HaloMemoryService = haloMemoryService;
        //            this.DataPointersService = dataPointersService;
        //            this.CheatManagerService = cheatManagerService;
        //            SelectedGame = 0;




        //            this.Button_InjectCheckpoint = new GenericActionViewModel(
        //                "Inject",
        //                checkpointViewModel.Inject,
        //                hotkeyManager
        //                );
        //            this.Button_InjectCheckpoint.ToolTip = "Injects the currently selected checkpoint into the game. If you enable the \"Auto-Revert on Inject\" option in the settings, then this will also force a revert immediately afterward";
        //            ListOfButtons.Add(this.Button_InjectCheckpoint);


        //            this.Button_DumpCheckpoint = new GenericActionViewModel(
        //    "Dump",
        //    checkpointViewModel.Dump,
        //    hotkeyManager
        //    );

        //            this.Button_DumpCheckpoint.ToolTip = "Backup (\"dump\") the games current checkpoint, saving it to the list on the left. If \"Auto-Checkpoint on Dump\" is enabled, this will force a checkpoint immediately beforehand";
        //            ListOfButtons.Add(this.Button_DumpCheckpoint);

        //            this.Button_ForceCheckpoint = new GenericActionViewModel(
        //                "Force Checkpoint", 
        //                new RelayCommand(o => { TrainerServices.ForceCheckpoint(); }, o => true),
        //                hotkeyManager
        //                );

        //            this.Button_ForceCheckpoint.ToolTip = "Force the game to immediately create a checkpoint";
        //            ListOfButtons.Add(this.Button_ForceCheckpoint);

        //            this.Button_ForceRevert = new GenericActionViewModel(
        //                "Force Revert",
        //    new RelayCommand(o => { TrainerServices.ForceRevert(); }, o => true),
        //                    hotkeyManager
        //    );
        //            this.Button_ForceRevert.ToolTip = "Forcefully reverts to the games last checkpoint";
        //            ListOfButtons.Add(this.Button_ForceRevert);

        //            this.Button_ForceCoreSave = new GenericActionViewModel(
        //                "Force Core save",
        //new RelayCommand(o => { TrainerServices.ForceCoreSave(); }, o => true),
        //                hotkeyManager
        //);
        //            this.Button_ForceCoreSave.ToolTip = "Force the game to make a Core Save (which are very like Checkpoints, but seperate)";
        //            ListOfButtons.Add(this.Button_ForceCoreSave);


        //            this.Button_ForceCoreLoad = new GenericActionViewModel( 
        //                "Force Core load",
        //new RelayCommand(o => { TrainerServices.ForceCoreLoad(); }, o => true),
        //                hotkeyManager
        //);

        //            this.Button_ForceCoreLoad.ToolTip = "Force the game to revert to the last Core Save";
        //            ListOfButtons.Add(this.Button_ForceCoreLoad);

        //            this.Button_ForceDoubleRevert = new GenericActionViewModel(
        //                "Force Double Revert",
        //new RelayCommand(o => { TrainerServices.FlipDoubleRevert(); TrainerServices.ForceRevert(); }, o => true),
        //                hotkeyManager
        //);
        //            this.Button_ForceDoubleRevert.ToolTip = "Revert to the checkpoint that was created BEFORE the last checkpoint you created";
        //            ListOfButtons.Add(this.Button_ForceDoubleRevert);


        //            this.Button_Teleport = new TeleportViewModel("Teleport", trainerServices, hotkeyManager);
        //            this.Button_Teleport.ToolTip = "Teleports the player character. Click the \"...\" to configure teleport settings";
        //            ListOfButtons.Add(this.Button_Teleport);

        //            this.Button_Launch = new LaunchViewModel("Launch", trainerServices, hotkeyManager);
        //            this.Button_Launch.ToolTip = "Launch the player character forward in the direction they're facing. Click the \"...\" to configure launch velocity settings";
        //            ListOfButtons.Add(this.Button_Launch);


        //            this.Button_ToggleInvuln = new GenericToggleViewModel("Invulnerability", PersistentCheatManager.PC_Invulnerability, hotkeyManager);
        //            this.Button_ToggleInvuln.ToolTip = "Toggles invulnerability cheat. This will also make vehicles you enter invulnerable (which includes hijacking!)";
        //            ListOfButtons.Add(this.Button_ToggleInvuln);

        //            this.Button_ToggleOHK = new GenericToggleViewModel("One Hit Kill", PersistentCheatManager.PC_OneHitKill, hotkeyManager);
        //            this.Button_ToggleOHK.ToolTip = "Makes any amount of damage, both to you and other NPCs, instantly kill the character. Bit of a meme, but good for testing wave fights";
        //            ListOfButtons.Add(this.Button_ToggleOHK);


        //            this.Button_ToggleSpeedhack = new SpeedhackViewModel("Speedhack", PersistentCheatManager.PC_Speedhack, hotkeyManager);
        //            this.Button_ToggleSpeedhack.ToolTip = "Speedhack the game to play at the associated speed. Can cause crashes with very low values";
        //            ListOfButtons.Add(this.Button_ToggleSpeedhack);

        //            this.Button_ToggleNaturals = new GenericToggleViewModel("Block Natural CPs", PersistentCheatManager.PC_BlockCPs, hotkeyManager);
        //            this.Button_ToggleNaturals.ToolTip = "Toggles the ability for the game to give you it's own, normal checkpoints";
        //            ListOfButtons.Add(this.Button_ToggleNaturals);



        //         this.Button_ToggleMedusa = new GenericToggleViewModel("Cheat Medusa", PersistentCheatManager.PC_Medusa, hotkeyManager);
        //            this.Button_ToggleMedusa.ToolTip = "Toggles cheat_medusa, which makes all enemies instantly die when they become aware of the player";
        //            ListOfButtons.Add(this.Button_ToggleMedusa);

        //            this.Button_SkullAcrophobia = new GenericActionViewModel("Acrophobia Skull", new RelayCommand(o => { TrainerServices.FlipSkull("Acrophobia"); }, o => true), hotkeyManager);
        //            this.Button_SkullAcrophobia.ToolTip = "Toggles the Acrophobia skull (jetpack/flyhack)";
        //            ListOfButtons.Add(this.Button_SkullAcrophobia);

        //            this.Button_SkullBandana = new GenericActionViewModel("Bandana Skull", new RelayCommand(o => { TrainerServices.FlipSkull("Bandana"); }, o => true), hotkeyManager);
        //            this.Button_SkullBandana.ToolTip = "Toggles the Bandana skull (infinite ammo/grenades)";
        //            ListOfButtons.Add(this.Button_SkullBandana);

        //            this.Button_SkullBlind = new GenericActionViewModel("Blind Skull", new RelayCommand(o => { TrainerServices.FlipSkull("Blind"); }, o => true), hotkeyManager);
        //            this.Button_SkullBlind.ToolTip = "Toggles the Blind skull (hide first-person weapon and UI)";
        //            ListOfButtons.Add(this.Button_SkullBlind);

        //            this.Button_ToggleBool = new GenericToggleViewModel("BOOL practice mode", null, null);
        //            this.Button_ToggleBool.ToolTip = "Toggle BOOL (Banshee-Out-Of-Level) practice mode for the level Two Betrayals";
        //            ListOfButtons.Add(this.Button_ToggleBool);

        //            this.Button_TogglePanCam = new GenericToggleViewModel("PanCam", null, null);
        //            this.Button_TogglePanCam.ToolTip = "Toggle the games \"PanCam\" function (displays co-ordinates)";
        //            ListOfButtons.Add(this.Button_TogglePanCam);


        //            this.Button_ToggleInfo = new DisplayInfoViewModel("Display Info", PersistentCheatManager.PC_DisplayInfo, hotkeyManager);
        //            this.Button_ToggleInfo.ToolTip = "Displays useful info about the current gamestate. Click the \"...\" to configure settings";
        //            ListOfButtons.Add(this.Button_ToggleInfo);

        //            this.Button_ToggleTheaterFriend = new GenericToggleViewModel("Theater Friend", PersistentCheatManager.PC_TheaterFriend, hotkeyManager);
        //            this.Button_ToggleTheaterFriend.ToolTip = "Makes the camera more powerful in Theater, allowing it to travel out-of-bounds and through walls";
        //            ListOfButtons.Add(this.Button_ToggleTheaterFriend);


        //            this.Button_ToggleFlyHack = new GenericToggleViewModel("Fly Hack", null, null);
        //            ListOfButtons.Add(this.Button_ToggleFlyHack);

        //            this.Button_ToggleSprintMeter = new GenericToggleViewModel("Sprint Meter", null, null);
        //            ListOfButtons.Add(this.Button_ToggleSprintMeter );

        //            UserControlToShow = "LD";
        //            HaloStateEvents.HALOSTATECHANGED_EVENT += HaloStateEvents_HALOSTATECHANGED_EVENT;
        //            hotkeyManager.KB_ReloadHotkeys();
        //        }
    }




}
