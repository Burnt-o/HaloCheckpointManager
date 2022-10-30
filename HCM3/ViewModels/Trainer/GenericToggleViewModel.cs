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
    public class GenericToggleViewModel : Presenter, IControlWithHotkey
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



        public IPersistentCheat PersistentCheatService { get; init; }


        public GenericToggleViewModel()
        {
            //parameterless constructor for design view
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }

        //instead of sending the command, we send the object property within PersistentCheatService, so we can bind IsChecked to it's isChecked
        public GenericToggleViewModel(string hotkeyText, string effectText, IPersistentCheat? persistentCheatService)
        {
            this.EffectText = effectText;
            this.HotkeyText = hotkeyText;
            this.ExecuteCommand = new RelayCommand(o => persistentCheatService.ToggleCheat(), o => true);
            this.ChangeHotkeyCommand = new(this);

            //for binding to IsChecked
            this.PersistentCheatService = persistentCheatService;
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
