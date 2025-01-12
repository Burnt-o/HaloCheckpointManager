using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using HCMExternal.Services.Hotkeys;
using HCMExternal.ViewModels.Interfaces;

namespace HCMExternal.ViewModels
{
    public class HotkeyedToggleViewModel : Presenter
    {

        public string EffectText { get; init; }
        public HotkeyViewModel HotkeyViewModel { get; init; }

        private bool _toggleValue { get; set; }
        public bool ToggleValue
        {
            get => _toggleValue;
            set
            {
                _toggleValue = value;
                OnPropertyChanged(nameof(ToggleValue)); // update ui
            }
        }

        public HotkeyedToggleViewModel(bool initialValue, string effectText, IHotkeyManager hotkeyManager, HotkeyEnum hotkeyEnum)
        {
            _toggleValue = initialValue;
            EffectText = effectText;

            HotkeyViewModel = hotkeyManager.RegisterHotkey(hotkeyEnum);
            HotkeyViewModel.HotkeyTriggered += HotkeyTriggerEvent;

        }

        private void HotkeyTriggerEvent(object? sender, EventArgs e)
        {
            ToggleValue = !ToggleValue;
        }
    }
}
