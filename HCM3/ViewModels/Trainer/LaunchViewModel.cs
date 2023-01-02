using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using System.Windows.Input;
using HCM3.ViewModels.Commands;
using HCM3.Services.Trainer;
using System.Windows;
using HCM3.Views.Controls.Trainer.ButtonViews.ButtonOptions;
using System.Diagnostics;
using HCM3.Services;
using XInputium.XInput;

namespace HCM3.ViewModels
{
    public class LaunchViewModel : ActionToggleBase
    {

      

        public string OptionText { get; set; }

       
        public ICommand OpenOptionsWindowCommand { get; init; }

        #region User inputted properties

        private float _launchSpeed;
        public float LaunchSpeed
        {
            get { return _launchSpeed; }
            set
            {
                _launchSpeed = value;
                Properties.Settings.Default.LaunchSpeed = value;
            }
        }

        #endregion

        TrainerServices TrainerServices { get; init; }


        public LaunchViewModel()
        {
            if (!IsInDesignModeStatic) throw new Exception("This should only be run in design mode");

            //parameterless constructor for design view
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }

        public LaunchViewModel(string effectText, TrainerServices? trainerServices, HotkeyManager? hotkeyManager) 
        {
            //load default values from settings
            this.LaunchSpeed = Properties.Settings.Default.LaunchSpeed;


            this.TrainerServices = trainerServices;
            
            this.EffectText = "Launch";

            this.OpenOptionsWindowCommand = new RelayCommand(o => { OpenOptionsWindow(typeof(LaunchOptionsView)); }, o => true);
            this.ExecuteCommand = new RelayCommand(o => { ExecuteLaunch(); }, o => true);

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

      




        public void ExecuteLaunch()
        {
            try
            {
                this.TrainerServices.BoostForward(LaunchSpeed);
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to execute Launch! \n" + ex.ToString() + ex.StackTrace, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            }
        }


    }
}
