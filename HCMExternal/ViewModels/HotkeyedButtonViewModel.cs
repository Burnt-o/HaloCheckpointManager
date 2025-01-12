using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using HCMExternal.Services.Hotkeys;

namespace HCMExternal.ViewModels
{
    public class HotkeyedButtonViewModel
    {

        public string EffectText { get; init;}
        public ICommand ExecuteCommand { get; init; }

        public HotkeyViewModel HotkeyViewModel { get; init; }

        public HotkeyedButtonViewModel(string effectText, IHotkeyManager hotkeyManager, HotkeyEnum hotkeyEnum, ICommand executeCommand)
        {
            EffectText = effectText;

            ExecuteCommand = executeCommand;
            HotkeyViewModel = hotkeyManager.RegisterHotkey(hotkeyEnum);
            HotkeyViewModel.HotkeyTriggered += HotkeyTriggerEvent;

        }

        private void HotkeyTriggerEvent(object? sender, EventArgs e)
        {
            ExecuteCommand.Execute(null);
        }
    }
}
