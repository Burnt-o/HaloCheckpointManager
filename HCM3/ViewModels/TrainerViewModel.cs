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
namespace HCM3.ViewModels
{
    internal class TrainerViewModel : Presenter
    {


        public ActionControlViewModel Button_ForceCheckpoint { get; set; }
        public ActionControlViewModel Button_ForceRevert { get; set; }
        public ActionControlViewModel Button_ForceCoreSave { get; set; }
        public ActionControlViewModel Button_ForceCoreLoad { get; set; }
        public ToggleControlViewModel Button_ToggleInvuln { get; set; }
        public ActionControlViewModel Button_ToggleSpeedhack { get; set; }
        public ActionControlViewModel Button_ToggleMedusa { get; set; }
        public ActionControlViewModel Button_ToggleBool { get; set; }


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
            this.Button_ForceCoreSave = new ActionControlViewModel("none", "Force Core save", null);
            this.Button_ForceCoreLoad = new ActionControlViewModel("none", "Force Core load", null);
            this.Button_ToggleInvuln = new ToggleControlViewModel("none", "Invulnerability", null);
            this.Button_ToggleSpeedhack = new ActionControlViewModel("none", "Speedhack", null);
            this.Button_ToggleMedusa = new ActionControlViewModel("none", "Cheat Medusa", null);
            this.Button_ToggleBool = new ActionControlViewModel("none", "BOOL practice mode", null);
        }

        private void HaloStateEvents_HALOSTATECHANGED_EVENT(object? sender, HaloStateEvents.HaloStateChangedEventArgs e)
        {
            //this.UserControlToShow = "H1";
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[e.NewHaloState];

            if (gameAs2Letters == "H1" || gameAs2Letters == "H2")
                this.UserControlToShow = gameAs2Letters;
            else
                this.UserControlToShow = "H1";
        }

        public TrainerViewModel(TrainerServices trainerServices, PersistentCheatService persistentCheatManager)
        {


            this.TrainerServices = trainerServices;
            this.PersistentCheatManager = persistentCheatManager;
            SelectedGame = 0;
            this.Button_ForceCheckpoint = new ActionControlViewModel("none", "Force Checkpoint", 
                new RelayCommand(o => { TrainerServices.ForceCheckpoint(); }, o => true));
            this.Button_ForceRevert = new ActionControlViewModel("none", "Force Revert", new ForceRevertCommand(this, this.TrainerServices));
            this.Button_ForceCoreSave = new ActionControlViewModel("none", "Force Core save", new ForceCoreSaveCommand(this, this.TrainerServices));
            this.Button_ForceCoreLoad = new ActionControlViewModel("none", "Force Core load", new ForceCoreLoadCommand(this, this.TrainerServices));
            this.Button_ToggleInvuln = new ToggleControlViewModel("none", "Invulnerability", PersistentCheatManager.PC_ToggleInvuln);
            this.Button_ToggleSpeedhack = new ActionControlViewModel("none", "Speedhack", null);
            this.Button_ToggleMedusa = new ActionControlViewModel("none", "Cheat Medusa", null);
            this.Button_ToggleBool = new ActionControlViewModel("none", "BOOL practice mode", null);

                        UserControlToShow = "H1";
            HaloStateEvents.HALOSTATECHANGED_EVENT += HaloStateEvents_HALOSTATECHANGED_EVENT;
        }
    }




}
