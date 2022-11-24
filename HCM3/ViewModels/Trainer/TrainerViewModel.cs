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

        public GenericToggleViewModel Button_ToggleAcro { get; set; }
        public GenericToggleViewModel Button_ToggleNaturals { get; set; }
        public GenericToggleViewModel Button_ToggleInfo { get; set; }

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
            this.Button_ToggleAcro = new GenericToggleViewModel("Acrophobia", null, null);
            this.Button_ToggleNaturals = new GenericToggleViewModel("Block Natural CPs", null, null);
            this.Button_ToggleInfo = new GenericToggleViewModel("Display Info", null, null);
            this.Button_TogglePanCam = new GenericToggleViewModel("PanCam", null, null);
            this.Button_ToggleFlyHack = new GenericToggleViewModel("Fly Hack", null, null);
            this.Button_ToggleSprintMeter = new GenericToggleViewModel("Sprint Meter", null, null);
            this.Button_ToggleOHK = new GenericToggleViewModel("One Hit Kill", null, null);
        }

        private void HaloStateEvents_HALOSTATECHANGED_EVENT(object? sender, HaloStateEvents.HaloStateChangedEventArgs e)
        {
            Trace.WriteLine("TRAINERVIEWMODEL HALOSTATECHANGEDEVENT BEEN TRIGGERED");
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

            listOfButtons.Add(this.Button_InjectCheckpoint);

            this.Button_DumpCheckpoint = new GenericActionViewModel(
    "Dump",
    checkpointViewModel.Dump,
    hotkeyManager
    );

            listOfButtons.Add(this.Button_DumpCheckpoint);

            this.Button_ForceCheckpoint = new GenericActionViewModel(
                "Force Checkpoint", 
                new RelayCommand(o => { TrainerServices.ForceCheckpoint(); }, o => true),
                hotkeyManager
                );

            listOfButtons.Add(this.Button_ForceCheckpoint);

            this.Button_ForceRevert = new GenericActionViewModel(
                "Force Revert",
    new RelayCommand(o => { TrainerServices.ForceRevert(); }, o => true),
                    hotkeyManager
    );

            listOfButtons.Add(this.Button_ForceRevert);

            this.Button_ForceCoreSave = new GenericActionViewModel(
                "Force Core save",
new RelayCommand(o => { TrainerServices.ForceCoreSave(); }, o => true),
                hotkeyManager
);
            listOfButtons.Add(this.Button_ForceCoreSave);


            this.Button_ForceCoreLoad = new GenericActionViewModel( 
                "Force Core load",
new RelayCommand(o => { TrainerServices.ForceCoreLoad(); }, o => true),
                hotkeyManager
);

            listOfButtons.Add(this.Button_ForceCoreLoad);

            this.Button_ForceDoubleRevert = new GenericActionViewModel(
                "Force Double Revert",
new RelayCommand(o => { TrainerServices.FlipDoubleRevert(); TrainerServices.ForceRevert(); }, o => true),
                hotkeyManager
);
            listOfButtons.Add(this.Button_ForceDoubleRevert);


            this.Button_Teleport = new TeleportViewModel("Teleport", trainerServices, hotkeyManager);
            listOfButtons.Add(this.Button_Teleport);

            this.Button_Launch = new LaunchViewModel("Launch", trainerServices, hotkeyManager);
            listOfButtons.Add(this.Button_Launch);


            this.Button_ToggleInvuln = new GenericToggleViewModel("Invulnerability", PersistentCheatManager.PC_Invulnerability, hotkeyManager);
            listOfButtons.Add(this.Button_ToggleInvuln);

            this.Button_ToggleOHK = new GenericToggleViewModel("One Hit Kill", PersistentCheatManager.PC_OneHitKill, hotkeyManager);
            listOfButtons.Add(this.Button_ToggleOHK);


            this.Button_ToggleSpeedhack = new SpeedhackViewModel("Speedhack", PersistentCheatManager.PC_Speedhack, hotkeyManager);
            listOfButtons.Add(this.Button_ToggleSpeedhack);
            this.Button_ToggleNaturals = new GenericToggleViewModel("Block Natural CPs", PersistentCheatManager.PC_BlockCPs, hotkeyManager);
            listOfButtons.Add(this.Button_ToggleNaturals);



         this.Button_ToggleMedusa = new GenericToggleViewModel("Cheat Medusa", PersistentCheatManager.PC_Medusa, hotkeyManager);
            listOfButtons.Add(this.Button_ToggleMedusa);

            this.Button_ToggleAcro = new GenericToggleViewModel("Acrophobia", PersistentCheatManager.PC_Acrophobia, hotkeyManager);
            listOfButtons.Add(this.Button_ToggleAcro);

            this.Button_ToggleBool = new GenericToggleViewModel("BOOL practice mode", null, null);
            listOfButtons.Add(this.Button_ToggleBool);
            this.Button_TogglePanCam = new GenericToggleViewModel("PanCam", null, null);
            listOfButtons.Add(this.Button_TogglePanCam);


            this.Button_ToggleInfo = new GenericToggleViewModel("Display Info", null, null);
            listOfButtons.Add(this.Button_ToggleInfo);
            this.Button_ToggleFlyHack = new GenericToggleViewModel("Fly Hack", null, null);
            listOfButtons.Add(this.Button_ToggleFlyHack);
            this.Button_ToggleSprintMeter = new GenericToggleViewModel("Sprint Meter", null, null);
            listOfButtons.Add(this.Button_ToggleSprintMeter );
            UserControlToShow = "LD";
            //Trace.WriteLine("CURRENTHALOSTATE at start: " + this.HaloMemoryService.HaloState.CurrentHaloState);
            HaloStateEvents.HALOSTATECHANGED_EVENT += HaloStateEvents_HALOSTATECHANGED_EVENT;
            hotkeyManager.KB_ReloadHotkeys();
        }
    }




}
