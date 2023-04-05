using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using System.Windows.Input;
using HCM3.ViewModels.Commands;
using HCM3.Services.Cheats;
using System.Windows;
using HCM3.Views.Controls.Trainer.ButtonViews.ButtonOptions;
using System.Diagnostics;
using HCM3.Services;
using XInputium.XInput;

namespace HCM3.ViewModels
{
    public class DisplayInfoViewModel : ActionToggleBase
    {

      

        public string OptionText { get; set; }

       
        public ICommand OpenOptionsWindowCommand { get; init; }

        #region User inputted properties

        private float _fontSize;
        public float FontSize
        {
            get { return _fontSize; }
            set
            {
                _fontSize = value;
                Properties.Settings.Default.DIFontSize = value;
            }
        }

        private float _screenX;
        public float ScreenX
        {
            get { return _screenX; }
            set
            {
                _screenX = value;
                Properties.Settings.Default.DIScreenX = value;
            }
        }

        private float _screenY;
        public float ScreenY
        {
            get { return _screenY; }
            set
            {
                _screenY = value;
                Properties.Settings.Default.DIScreenY = value;
            }
        }

        private int _sigDig;
        public int SigDig
        {
            get { return _sigDig; }
            set
            {
                _sigDig = value;
                Properties.Settings.Default.DISigDig = value;
            }
        }

        #endregion

        //nfi if this should be ICheat
        ICheat DisplayInfo { get; init; }


        public DisplayInfoViewModel()
        {
            if (!IsInDesignModeStatic) throw new Exception("This should only be run in design mode");

            //parameterless constructor for design view
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }

        public DisplayInfoViewModel(string effectText, ICheat displayInfo, HotkeyManager? hotkeyManager) 
        {
            //load default values from settings
            this.FontSize = Properties.Settings.Default.DIFontSize;
            this.ScreenX = Properties.Settings.Default.DIScreenX;
            this.ScreenY = Properties.Settings.Default.DIScreenY;
            this.SigDig = Properties.Settings.Default.DISigDig;


            this.DisplayInfo = displayInfo;
            
            this.EffectText = "Display Info";

            this.OpenOptionsWindowCommand = new RelayCommand(o => { OpenOptionsWindow(typeof(DisplayInfoOptionsView)); }, o => true);
            this.ExecuteCommand = new RelayCommand(o => { ExecuteDisplayInfo(); }, o => true);

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

      




        public void ExecuteDisplayInfo()
        {
            try
            {
                this.DisplayInfo.DoCheat();
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to execute DisplayInfo! \n" + ex.ToString() + ex.StackTrace, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            }
        }


    }
}
