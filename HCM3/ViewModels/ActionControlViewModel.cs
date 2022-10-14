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
    public class ActionControlViewModel : Presenter
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

        public ITrainerCommand? ExecuteCommand { get; init; }

        public bool IsEnabled { get; set; } //only used by toggle controls



        public ActionControlViewModel()
        {
            //parameterless constructor for design view
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }

        public ActionControlViewModel(string hotkeyText, string effectText, ITrainerCommand? executeCommand) 
        {
            this.EffectText = effectText;
            this.HotkeyText = hotkeyText;
            this.ExecuteCommand = executeCommand;
            if (this.ExecuteCommand != null) this.ExecuteCommand.ActionControlViewModel = this;

            this.ChangeHotkeyCommand = new(this);
            this.IsEnabled = false;
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
