using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using System.Windows.Input;
using HCM3.ViewModels.Commands;

using System.Diagnostics;
using HCM3.Services;
using XInputium.XInput;


namespace HCM3.ViewModels
{
    public class GenericActionViewModel : ActionToggleBase
    {

        public GenericActionViewModel()
        {
            if (!IsInDesignModeStatic) throw new Exception("This should only be run in design mode");

            //parameterless constructor for design view
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }

        public GenericActionViewModel(string effectText, ICommand? executeCommand, HotkeyManager? hotkeyManager)
        {
            this.EffectText = effectText;
            this.ExecuteCommand = executeCommand;
            this.NameOfBinding = effectText.Replace(" ", "");


            if (hotkeyManager == null)
            {
                this.HotkeyText = "N/A";
                return;
            }


            HotkeyManager = hotkeyManager;
            this.OpenHotkeyWindowCommand = new RelayCommand(o => { OpenHotkeyWindow(); }, o => { return true; });


            // Load from deserialised bindings
            if (HotkeyManager.RegisteredKBbindings.ContainsKey(NameOfBinding))
            {
                Tuple<int?, Action?> KBbinding = HotkeyManager.RegisteredKBbindings[NameOfBinding];
                HotkeyManager.KB_InitHotkey(NameOfBinding, KBbinding.Item1, OnHotkeyPress);
                this.KBhotkey = KBbinding.Item1;
            }

            if (HotkeyManager.RegisteredGPbindings.ContainsKey(NameOfBinding))
            {
                Tuple<XInputButton?, Action?> GPbinding = HotkeyManager.RegisteredGPbindings[NameOfBinding];
                HotkeyManager.GP_InitHotkey(NameOfBinding, GPbinding.Item1, OnHotkeyPress);
                this.GPhotkey = GPbinding.Item1;
            }


            SetHotkeyText();
        }







    }
}
