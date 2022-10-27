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


        public ActionControlViewModel Button_ForceCheckpoint { get; set; }
        public ActionControlViewModel Button_ForceRevert { get; set; }

        public ActionControlViewModel Button_ForceDoubleRevert { get; set; }
        public ActionControlViewModel Button_ForceCoreSave { get; set; }
        public ActionControlViewModel Button_ForceCoreLoad { get; set; }
        public ActionControlViewModel Button_Teleport { get; set; }
        public ActionControlViewModel Button_Launch { get; set; }

        public ToggleControlViewModel Button_ToggleInvuln { get; set; }
        public SpeedHackControlViewModel Button_ToggleSpeedhack { get; set; }
        public ToggleControlViewModel Button_ToggleMedusa { get; set; }
        public ToggleControlViewModel Button_ToggleBool { get; set; }

        public ToggleControlViewModel Button_ToggleAcro { get; set; }
        public ToggleControlViewModel Button_ToggleNaturals { get; set; }
        public ToggleControlViewModel Button_ToggleInfo { get; set; }

        public ToggleControlViewModel Button_TogglePanCam { get; set; }

        public ToggleControlViewModel Button_ToggleFlyHack { get; set; }

        public ToggleControlViewModel Button_ToggleSprintMeter { get; set; }



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

        // for design mode
        [Obsolete]
        public TrainerViewModel()
        {

            this.Button_ForceCheckpoint = new ActionControlViewModel("none", "Force Checkpoint", null);
            this.Button_ForceRevert = new ActionControlViewModel("none", "Force Revert", null);
            this.Button_ForceDoubleRevert = new ActionControlViewModel("none", "Force Double Revert", null);
            this.Button_ForceCoreSave = new ActionControlViewModel("none", "Force Core save", null);
            this.Button_ForceCoreLoad = new ActionControlViewModel("none", "Force Core load", null);
            this.Button_Teleport = new ActionControlViewModel("none", "Teleport", null);
            this.Button_Launch = new ActionControlViewModel("none", "Launch", null);
            this.Button_ToggleInvuln = new ToggleControlViewModel("none", "Invulnerability", null);
            this.Button_ToggleSpeedhack = new SpeedHackControlViewModel("none", "Speedhack", null);
            this.Button_ToggleMedusa = new ToggleControlViewModel("none", "Cheat Medusa", null);
            this.Button_ToggleBool = new ToggleControlViewModel("none", "BOOL practice mode", null);
            this.Button_ToggleAcro = new ToggleControlViewModel("none", "Acrophobia", null);
            this.Button_ToggleNaturals = new ToggleControlViewModel("none", "Block Natural CPs", null);
            this.Button_ToggleInfo = new ToggleControlViewModel("none", "Display Info", null);
            this.Button_TogglePanCam = new ToggleControlViewModel("none", "PanCam", null);
            this.Button_ToggleFlyHack = new ToggleControlViewModel("none", "Fly Hack", null);
            this.Button_ToggleSprintMeter = new ToggleControlViewModel("none", "Sprint Meter", null);
        }

        private void HaloStateEvents_HALOSTATECHANGED_EVENT(object? sender, HaloStateEvents.HaloStateChangedEventArgs e)
        {
            //this.UserControlToShow = "H1";
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[e.NewHaloState];


                this.UserControlToShow = gameAs2Letters;
        }

        public TrainerViewModel(TrainerServices trainerServices, PersistentCheatService persistentCheatManager)
        {


            this.TrainerServices = trainerServices;
            this.PersistentCheatManager = persistentCheatManager;
            SelectedGame = 0;

            this.Button_ForceCheckpoint = new ActionControlViewModel("none", "Force Checkpoint", 
                new RelayCommand(o => { TrainerServices.ForceCheckpoint(); }, o => true));
            this.Button_ForceRevert = new ActionControlViewModel("none", "Force Revert",
    new RelayCommand(o => { TrainerServices.ForceRevert(); }, o => true));

            this.Button_ForceCoreSave = new ActionControlViewModel("none", "Force Core save",
new RelayCommand(o => { TrainerServices.ForceCoreSave(); }, o => true));


            this.Button_ForceCoreLoad = new ActionControlViewModel("none", "Force Core load",
new RelayCommand(o => { TrainerServices.ForceCoreLoad(); }, o => true));

            this.Button_ForceDoubleRevert = new ActionControlViewModel("none", "Force Double Revert", null);

            this.Button_Teleport = new ActionControlViewModel("none", "Teleport", null);
            this.Button_Launch = new ActionControlViewModel("none", "Launch", null);


            this.Button_ToggleInvuln = new ToggleControlViewModel("none", "Invulnerability", PersistentCheatManager.PC_Invulnerability);
            this.Button_ToggleSpeedhack = new SpeedHackControlViewModel("none", "Speedhack", PersistentCheatManager.PC_Speedhack);
            this.Button_ToggleMedusa = new ToggleControlViewModel("none", "Cheat Medusa", null);
            this.Button_ToggleBool = new ToggleControlViewModel("none", "BOOL practice mode", null);
            this.Button_TogglePanCam = new ToggleControlViewModel("none", "PanCam", null);
            this.Button_ToggleAcro = new ToggleControlViewModel("none", "Acrophobia", null);
            this.Button_ToggleNaturals = new ToggleControlViewModel("none", "Block Natural CPs", null);
            this.Button_ToggleInfo = new ToggleControlViewModel("none", "Display Info", null);
            this.Button_ToggleFlyHack = new ToggleControlViewModel("none", "Fly Hack", null);
            this.Button_ToggleSprintMeter = new ToggleControlViewModel("none", "Sprint Meter", null);
            UserControlToShow = "LD";
            HaloStateEvents.HALOSTATECHANGED_EVENT += HaloStateEvents_HALOSTATECHANGED_EVENT;
        }
    }




}
