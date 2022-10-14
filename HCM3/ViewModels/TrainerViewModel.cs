using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using HCM3.ViewModels.Commands;
using HCM3.Services.Trainer;
namespace HCM3.ViewModels
{
    internal class TrainerViewModel : Presenter
    {
        public ButtonViewModel Button_ForceCheckpoint { get; set; }
        public ButtonViewModel Button_ForceRevert { get; set; }
        public ButtonViewModel Button_ForceCoreSave { get; set; }
        public ButtonViewModel Button_ForceCoreLoad { get; set; }
        public ButtonViewModel Button_ToggleInvuln { get; set; }
        public ButtonViewModel Button_ToggleSpeedhack { get; set; }
        public ButtonViewModel Button_ToggleMedusa { get; set; }
        public ButtonViewModel Button_ToggleBool { get; set; }



        public int SelectedGame { get; set; }
        public bool SelectedGameSameAsActualGame { get; set; }

        public TrainerServices TrainerServices { get; init; }


        public TrainerViewModel()
        {
            this.Button_ForceCheckpoint = new ButtonViewModel("none", "Force Checkpoint", null);
            this.Button_ForceRevert = new ButtonViewModel("none", "Force Revert", null);
            this.Button_ForceCoreSave = new ButtonViewModel("none", "Force Core save", null);
            this.Button_ForceCoreLoad = new ButtonViewModel("none", "Force Core load", null);
            this.Button_ToggleInvuln = new ButtonViewModel("none", "Invulnerability", null);
            this.Button_ToggleSpeedhack = new ButtonViewModel("none", "Speedhack", null);
            this.Button_ToggleMedusa = new ButtonViewModel("none", "Cheat Medusa", null);
            this.Button_ToggleBool = new ButtonViewModel("none", "BOOL practice mode", null);
        }
        public TrainerViewModel(TrainerServices trainerServices)
        {
            this.TrainerServices = trainerServices;
            SelectedGame = 0;
            this.Button_ForceCheckpoint = new ButtonViewModel("none", "Force Checkpoint", new ForceCheckpointCommand(this, this.TrainerServices));
            this.Button_ForceRevert = new ButtonViewModel("none", "Force Revert", null);
            this.Button_ForceCoreSave = new ButtonViewModel("none", "Force Core save", null);
            this.Button_ForceCoreLoad = new ButtonViewModel("none", "Force Core load", null);
            this.Button_ToggleInvuln = new ButtonViewModel("none", "Invulnerability", null);
            this.Button_ToggleSpeedhack = new ButtonViewModel("none", "Speedhack", null);
            this.Button_ToggleMedusa = new ButtonViewModel("none", "Cheat Medusa", null);
            this.Button_ToggleBool = new ButtonViewModel("none", "BOOL practice mode", null);

        }
    }




}
