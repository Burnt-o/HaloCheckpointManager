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


        public GenericActionViewModel Button_ForceCheckpoint { get; set; }
        public GenericActionViewModel Button_ForceRevert { get; set; }

        public GenericActionViewModel Button_ForceDoubleRevert { get; set; }
        public GenericActionViewModel Button_ForceCoreSave { get; set; }
        public GenericActionViewModel Button_ForceCoreLoad { get; set; }
        public TeleportViewModel Button_Teleport { get; set; }
        public GenericActionViewModel Button_Launch { get; set; }

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

            this.Button_ForceCheckpoint = new GenericActionViewModel("none", "Force Checkpoint", null);
            this.Button_ForceRevert = new GenericActionViewModel("none", "Force Revert", null);
            this.Button_ForceDoubleRevert = new GenericActionViewModel("none", "Force Double Revert", null);
            this.Button_ForceCoreSave = new GenericActionViewModel("none", "Force Core save", null);
            this.Button_ForceCoreLoad = new GenericActionViewModel("none", "Force Core load", null);
            this.Button_Teleport = new TeleportViewModel("none", "Teleport", null);
            this.Button_Launch = new GenericActionViewModel("none", "Launch", null);
            this.Button_ToggleInvuln = new GenericToggleViewModel("none", "Invulnerability", null);
            this.Button_ToggleSpeedhack = new SpeedhackViewModel("none", "Speedhack", null);
            this.Button_ToggleMedusa = new GenericToggleViewModel("none", "Cheat Medusa", null);
            this.Button_ToggleBool = new GenericToggleViewModel("none", "BOOL practice mode", null);
            this.Button_ToggleAcro = new GenericToggleViewModel("none", "Acrophobia", null);
            this.Button_ToggleNaturals = new GenericToggleViewModel("none", "Block Natural CPs", null);
            this.Button_ToggleInfo = new GenericToggleViewModel("none", "Display Info", null);
            this.Button_TogglePanCam = new GenericToggleViewModel("none", "PanCam", null);
            this.Button_ToggleFlyHack = new GenericToggleViewModel("none", "Fly Hack", null);
            this.Button_ToggleSprintMeter = new GenericToggleViewModel("none", "Sprint Meter", null);
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

            this.Button_ForceCheckpoint = new GenericActionViewModel("none", "Force Checkpoint", 
                new RelayCommand(o => { TrainerServices.ForceCheckpoint(); }, o => true));
            this.Button_ForceRevert = new GenericActionViewModel("none", "Force Revert",
    new RelayCommand(o => { TrainerServices.ForceRevert(); }, o => true));

            this.Button_ForceCoreSave = new GenericActionViewModel("none", "Force Core save",
new RelayCommand(o => { TrainerServices.ForceCoreSave(); }, o => true));


            this.Button_ForceCoreLoad = new GenericActionViewModel("none", "Force Core load",
new RelayCommand(o => { TrainerServices.ForceCoreLoad(); }, o => true));

            this.Button_ForceDoubleRevert = new GenericActionViewModel("none", "Force Double Revert",
new RelayCommand(o => { TrainerServices.FlipDoubleRevert(); TrainerServices.ForceRevert(); }, o => true));

/*            this.Button_Teleport = new ActionControlViewModel("none", "Teleport",
new RelayCommand(o => { TrainerServices.TeleportForward(5, false); }, o => true));*/

            this.Button_Teleport = new TeleportViewModel("none", "Teleport", trainerServices);

            this.Button_Launch = new GenericActionViewModel("none", "Launch",
new RelayCommand(o => { TrainerServices.BoostForward((float)10); }, o => true));



            this.Button_ToggleInvuln = new GenericToggleViewModel("none", "Invulnerability", PersistentCheatManager.PC_Invulnerability);
            this.Button_ToggleSpeedhack = new SpeedhackViewModel("none", "Speedhack", PersistentCheatManager.PC_Speedhack);
            this.Button_ToggleNaturals = new GenericToggleViewModel("none", "Block Natural CPs", PersistentCheatManager.PC_BlockCPs);
            this.Button_ToggleMedusa = new GenericToggleViewModel("none", "Cheat Medusa", null);
            this.Button_ToggleBool = new GenericToggleViewModel("none", "BOOL practice mode", null);
            this.Button_TogglePanCam = new GenericToggleViewModel("none", "PanCam", null);
            this.Button_ToggleAcro = new GenericToggleViewModel("none", "Acrophobia", null);
            
            this.Button_ToggleInfo = new GenericToggleViewModel("none", "Display Info", null);
            this.Button_ToggleFlyHack = new GenericToggleViewModel("none", "Fly Hack", null);
            this.Button_ToggleSprintMeter = new GenericToggleViewModel("none", "Sprint Meter", null);
            UserControlToShow = "LD";
            HaloStateEvents.HALOSTATECHANGED_EVENT += HaloStateEvents_HALOSTATECHANGED_EVENT;
        }
    }




}
