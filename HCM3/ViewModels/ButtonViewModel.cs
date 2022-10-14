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
    internal class ButtonViewModel : Presenter
    {
        private string _hotkeyText;
        public string HotkeyText { 
            get { return _hotkeyText; }
            set 
            { 
            _hotkeyText = value;
                OnPropertyChanged(nameof(HotkeyText));
            } 
        }
        public string EffectText { get; set; }
        public ICommand? ExecuteCommand { get; init; }
        public ChangeHotkeyCommand? ChangeHotkeyCommand { get; init; }



        [Obsolete("Only for design data", true)]
        public ButtonViewModel()
        {
            if (!IsInDesignModeStatic)
            {
                throw new Exception("Use only for design mode");
            }
            this.HotkeyText = "HotkeyText";
            this.EffectText = "EffectText";
        }

        public ButtonViewModel(string hotkeyText, string effectText, ICommand? executeCommand)
        {
            this.EffectText = effectText;
            this.HotkeyText = hotkeyText;
            this.ExecuteCommand = executeCommand;
            this.ChangeHotkeyCommand = new(this);
        }
    }
}
