using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using System.Windows.Input;
using HCM3.ViewModels.Commands;

namespace HCM3.ViewModels
{
    public class GenericActionViewModel : Presenter, IControlWithHotkey
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

        //public bool IsEnabled { get; set; } //only used by toggle controls



        public GenericActionViewModel()
        {
            //parameterless constructor for design view
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }

        public GenericActionViewModel(string hotkeyText, string effectText, ICommand? executeCommand) 
        {
            this.EffectText = effectText;
            this.HotkeyText = hotkeyText;
            this.ExecuteCommand = executeCommand;
            this.ChangeHotkeyCommand = new(this);
            //this.IsEnabled = false;
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
