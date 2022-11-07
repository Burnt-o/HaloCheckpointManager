using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using System.Windows.Input;
using HCM3.ViewModels.Commands;
using HCM3.Services.Trainer;
using HCM3.Services;
using XInputium.XInput;

namespace HCM3.ViewModels
{
    public class SpeedhackViewModel : ActionToggleBase
    {

       



 

        public ICommand? ApplyCommand { get; init; }

        public PC_Speedhack PC_Speedhack { get; init; }

        public SpeedhackViewModel()
        {
            if (!IsInDesignModeStatic) throw new Exception("This should only be run in design mode");

            //parameterless constructor for design view
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }


        //instead of sending the command, we send the object property within PersistentCheatService, so we can bind IsChecked to it's isChecked
        public SpeedhackViewModel(string effectText, PC_Speedhack? pC_Speedhack, HotkeyManager? hotkeyManager)
        {
            this.PC_Speedhack = pC_Speedhack; //for binding to IsChecked
            this.EffectText = effectText;
            this.ExecuteCommand = new RelayCommand(o => PC_Speedhack.ToggleCheat(), o => true);
            this.ApplyCommand = new RelayCommand(o => PC_Speedhack.ApplySpeed(), o => true);
            this.NameOfBinding = effectText.Replace(" ", "");


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
