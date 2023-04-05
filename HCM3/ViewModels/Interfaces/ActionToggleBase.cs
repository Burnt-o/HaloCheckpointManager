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
using System.Diagnostics;

namespace HCM3.ViewModels.MVVM
{
    public abstract class ActionToggleBase : Presenter
    {
        private bool _isChecked = false;
        public bool IsChecked
        {
            get
            {
                return _isChecked;
            }
            set
            {
                _isChecked = value;
                System.Windows.Application.Current.Dispatcher.Invoke((Action)delegate {
                    OnPropertyChanged(nameof(IsChecked));
                });
            }
        }





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

        private string _effectText = "";
        public string EffectText
        { 
        get => _effectText;
            set { _effectText = value; OnPropertyChanged(nameof(EffectText)); }
        }


        private string? _toolTip = null;
        public string? ToolTip
        {
            get => _toolTip;
            set { _toolTip = value; OnPropertyChanged(nameof(ToolTip)); }
        }



        public ICommand OpenHotkeyWindowCommand { get; init; }
        public ICommand? ExecuteCommand { get; init; }
        public ICommand OpenOptionsWindowCommand { get; init; }

        public HotkeyManager HotkeyManager { get; init; }

        public int? KBhotkey { get; set; }

        public XInputButton? GPhotkey { get; set; }

        //public bool IsEnabled { get; set; } //only used by toggle controls

        public string NameOfBinding { get; set; }



        private bool _isEnabled = true;
        public bool IsEnabled
        {
            get { return _isEnabled; }
            set { _isEnabled = value; OnPropertyChanged(nameof(IsEnabled)); }
        }

        public Visibility Visibility
        {
            get
            {
                return IsEnabled ? Visibility.Visible : Visibility.Collapsed;
            }
        }

        public void OpenOptionsWindow(Type OptionsView)
        {
            //must occur on UI thread
            Application.Current.Dispatcher.Invoke((Action)delegate {
                Window win = (Window)Activator.CreateInstance(OptionsView);
                win.Owner = App.Current.MainWindow; // makes the dialog be centers on the main window
                win.DataContext = this;
                win.ShowDialog();
            });
        }


        public void OpenHotkeyWindow()
        {
            HotkeyManager.IgnoreGamepad = true;
            
            //must occur on UI thread
            Application.Current.Dispatcher.Invoke((Action)delegate {
                ChangeHotkeyView chv = new(KBhotkey, GPhotkey, this.HotkeyManager, NameOfBinding);
                if (chv.ShowDialog().Value)
                {
                    HotkeyManager.KB_ChangeHotkey(this.NameOfBinding, chv.SelectedKey, OnHotkeyPress);
                    HotkeyManager.GP_ChangeHotkey(this.NameOfBinding, chv.SelectedPad, OnHotkeyPress);

                    this.KBhotkey = chv.SelectedKey;
                    this.GPhotkey = chv.SelectedPad;

                    SetHotkeyText();
                }
            });

  
            HotkeyManager.IgnoreGamepad = false;
        }

        public void OnHotkeyPress()
        {
            if (ExecuteCommand != null && ExecuteCommand.CanExecute(null) && this.IsEnabled)
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
