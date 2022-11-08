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
    public class TeleportViewModel : ActionToggleBase
    {

        public string OptionText { get; set; }

       
        public ICommand? FillPositionCommand { get; init; }

        public ICommand OpenOptionsWindowCommand { get; init; }

        #region User inputted properties
        //For radio button to choose between "Forward x units" and "To specified position"
        private bool _teleportModeForward;
        public bool TeleportModeForward
        {
            get { return _teleportModeForward; }
            set
            {
                _teleportModeForward = value;
                Properties.Settings.Default.TeleportModeForward = value;
                OnPropertyChanged(nameof(TeleportModeForward));
            }
        }

        private bool _teleportIgnoreZ;
        public bool TeleportIgnoreZ
        {
            get { return _teleportIgnoreZ; }
            set
            {
                _teleportIgnoreZ = value;
                Properties.Settings.Default.TeleportIgnoreZ = value;
            }
        }

        private float _teleportLength;
        public float TeleportLength
        {
            get { return _teleportLength; }
            set
            {
                _teleportLength = value;

                Properties.Settings.Default.TeleportLength = value;
            }
        }

        private float _teleportX;
        public float TeleportX
        {
            get { return _teleportX; }
            set
            {
                _teleportX = value;
                OnPropertyChanged(nameof(TeleportX));
                Properties.Settings.Default.TeleportX = value;
            }
        }
        private float _teleportY;
        public float TeleportY
        {
            get { return _teleportY; }
            set
            {
                _teleportY = value;
                OnPropertyChanged(nameof(TeleportY));
                Properties.Settings.Default.TeleportY = value;
            }
        }
        private float _teleportZ;
        public float TeleportZ
        {
            get { return _teleportZ; }
            set
            {
                _teleportZ = value;
                OnPropertyChanged(nameof(TeleportZ));
                Properties.Settings.Default.TeleportZ = value;
            }
        } 
        #endregion

        TrainerServices TrainerServices { get; init; }



        public TeleportViewModel()
        {
            if (!IsInDesignModeStatic) throw new Exception("This should only be run in design mode");

            //parameterless constructor for design view
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }

        public TeleportViewModel(string effectText, TrainerServices? trainerServices, HotkeyManager? hotkeyManager)
        {
            //load default values from settings
            this.TeleportModeForward = Properties.Settings.Default.TeleportModeForward;
            this.TeleportX = Properties.Settings.Default.TeleportX;
            this.TeleportY = Properties.Settings.Default.TeleportY;
            this.TeleportZ = Properties.Settings.Default.TeleportZ;
            this.TeleportLength = Properties.Settings.Default.TeleportLength;
            this.TeleportIgnoreZ = Properties.Settings.Default.TeleportIgnoreZ;

            this.TrainerServices = trainerServices;
            
            this.EffectText = TeleportModeForward ? "Tele Forward" : "Tele Position";


            this.OpenOptionsWindowCommand = new RelayCommand(o => { OpenOptionsWindow(typeof(TeleportOptionsView)); }, o => true);
            this.ExecuteCommand = new RelayCommand(o => { ExecuteTeleport(); }, o => true);

            this.FillPositionCommand = new RelayCommand(o => {
                FillPosition();
            }, o => true);

            this.PropertyChanged += TeleportViewModel_PropertyChanged;

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

        private void FillPosition()
        {
            try
            {
                (float, float, float) tuple = this.TrainerServices.TeleportGetPosition();
                TeleportX = tuple.Item1;
                TeleportY = tuple.Item2;
                TeleportZ = tuple.Item3;
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to fill positions with current location! \n" + ex.Message + ex.StackTrace, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            }
        }


        private void TeleportViewModel_PropertyChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
           
            if (e.PropertyName == nameof(TeleportModeForward))
            {
                this.EffectText = TeleportModeForward ? "Tele Forward" : "Tele Position";
            }
        }

        public void ExecuteTeleport()
        {
            try
            {
                if (TeleportModeForward) // Forward x units
                {
                    this.TrainerServices.TeleportForward(TeleportLength, TeleportIgnoreZ);
                }
                else
                { 
                    this.TrainerServices.TeleportToLocation(TeleportX, TeleportY, TeleportZ);
                }
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to execute Teleport! \n" + ex.Message + ex.StackTrace, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            }
        }


    }
}
