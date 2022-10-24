using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using System.Windows.Input;
using HCM3.ViewModels.Commands;
using HCM3.Services.Trainer;

namespace HCM3.ViewModels
{
    public class SpeedHackControlViewModel : Presenter, IControlWithHotkey
    {

        private string _hotkeyText;
        public string HotkeyText
        {
            get { return _hotkeyText; }
            set
            {
                _hotkeyText = value;
                OnPropertyChanged(nameof(HotkeyText));
            }
        }
        public string EffectText { get; set; }

        public ChangeHotkeyCommand? ChangeHotkeyCommand { get; init; }

        public ICommand? ExecuteCommand { get; init; }

        public ICommand? ApplyCommand { get; init; }

        public PC_Speedhack PC_Speedhack { get; init; }


        public SpeedHackControlViewModel()
        {
            //parameterless constructor for design view
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }

        //instead of sending the command, we send the object property within PersistentCheatService, so we can bind IsChecked to it's isChecked
        public SpeedHackControlViewModel(string hotkeyText, string effectText, PC_Speedhack pC_Speedhack)
        {
            this.PC_Speedhack = pC_Speedhack; //for binding to IsChecked
            this.EffectText = effectText;
            this.HotkeyText = hotkeyText;
            this.ExecuteCommand = new RelayCommand(o => PC_Speedhack.ToggleCheat(), o => true);
            this.ApplyCommand = new RelayCommand(o => PC_Speedhack.ApplySpeed(), o => true);
            this.ChangeHotkeyCommand = new(this);

            

        }

        public void OnHotkeyPress()
        {
            if (ExecuteCommand != null && ExecuteCommand.CanExecute(null))
            {
                ExecuteCommand.Execute(null);
            }
        }
    }
}
