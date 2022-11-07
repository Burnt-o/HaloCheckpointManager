using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows;
using HCM3.Services;
using XInputium.XInput;
using HCM3.Helpers;
using HCM3.ViewModels;
using System.Windows.Input;
using HCM3.ViewModels.Commands;
using HCM3.Views.Controls.Trainer.ButtonViews.ButtonOptions;

namespace HCM3.ViewModels.MVVM
{
    public abstract class ActionToggleBase : Presenter
    {
        // defines hotkey stuff
        private string? _hotkeyText;
        public string? HotkeyText
        {
            get { return _hotkeyText; }
            set
            {
                _hotkeyText = value;
                OnPropertyChanged(nameof(HotkeyText));
            }
        }
        public string EffectText { get; set; }


        public ICommand OpenHotkeyWindowCommand { get; init; }
        public ICommand? ExecuteCommand { get; init; }


        public HotkeyManager HotkeyManager { get; init; }

        public int? KBhotkey { get; set; }

        public XInputButton? GPhotkey { get; set; }

        //public bool IsEnabled { get; set; } //only used by toggle controls

        public string NameOfBinding { get; set; }








        public void OpenHotkeyWindow()
        {
            HotkeyManager.IgnoreGamepad = true;
            ChangeHotkeyView chv = new(KBhotkey, GPhotkey, this.HotkeyManager, NameOfBinding);
            if (chv.ShowDialog().Value)
            {
                HotkeyManager.KB_ChangeHotkey(this.NameOfBinding, chv.SelectedKey, OnHotkeyPress);
                HotkeyManager.GP_ChangeHotkey(this.NameOfBinding, chv.SelectedPad, OnHotkeyPress);

                this.KBhotkey = chv.SelectedKey;
                this.GPhotkey = chv.SelectedPad;

                SetHotkeyText();
            }
            else
            {
            }
            HotkeyManager.IgnoreGamepad = false;
        }

        public void OnHotkeyPress()
        {
            if (ExecuteCommand != null && ExecuteCommand.CanExecute(null))
            {
                ExecuteCommand.Execute(null);
            }
        }


        public void SetHotkeyText()
        {
            string? str = null;

            if (KBhotkey != null && KBhotkey.Value <= 0xFF && KBhotkey.Value >= 0)
            {
                str = KeyInterop.KeyFromVirtualKey(KBhotkey.Value).ToString();
                /*
                                ushort vk = (ushort)KBhotkey.Value;
                                var enumString = (Dictionaries.VirtualKeys)vk;
                                str = enumString.ToString();*/
            }

            if (GPhotkey != null)
            {
                if (str != null) str = str + ", ";
                str = str + "[" + GPhotkey.ToString() + "]";

            }

            if (str == null) str = "Unbound";

            HotkeyText = str;

        }

    }
}
