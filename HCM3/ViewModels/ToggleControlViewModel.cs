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
    public class ToggleControlViewModel : Presenter, IControlWithHotkey
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

        private bool _isChecked;
        public bool IsChecked //only used by toggle controls
        {
            get { return _isChecked; }
            set
            {
                _isChecked = value;
                OnPropertyChanged(nameof(IsChecked));
            }
        }


        public ToggleControlViewModel()
        {
            //parameterless constructor for design view
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }

        //instead of sending the command, we send the object property within PersistentCheatService, so we can bind IsChecked to it's isChecked
        public ToggleControlViewModel(string hotkeyText, string effectText, IPersistentCheatService? persistentCheatService)
        {
            this.EffectText = effectText;
            this.HotkeyText = hotkeyText;
            this.ExecuteCommand = new RelayCommand(o => persistentCheatService.ToggleCheat(), o => true);
            this.ChangeHotkeyCommand = new(this);
            this.IsChecked = false;
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
