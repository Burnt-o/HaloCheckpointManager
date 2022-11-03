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

namespace HCM3.ViewModels
{
    public class LaunchViewModel : Presenter, IControlWithHotkey
    {

        private string _hotkeyText;
        public string HotkeyText
        {
            get { return _hotkeyText; }
            set
            {
                _hotkeyText = value;
                OnPropertyChanged(nameof(HotkeyText));
            }
        }

        private string _effectText;
        public string EffectText
        { 
        get { return _effectText; }
            set { _effectText = value; OnPropertyChanged(nameof(EffectText)); }
        }

        public string OptionText { get; set; }

        public ChangeHotkeyCommand? ChangeHotkeyCommand { get; init; }

        public ICommand? ExecuteCommand { get; init; }


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

        public void OpenOptionsWindow()
        {
            LaunchOptionsView win = new LaunchOptionsView();
            win.DataContext = this;
            win.ShowDialog();
        }

        public LaunchViewModel()
        {
            //parameterless constructor for design view
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }

        public LaunchViewModel(string hotkeyText, string effectText, TrainerServices? trainerServices) 
        {
            //load default values from settings
            this.LaunchSpeed = Properties.Settings.Default.LaunchSpeed;


            this.TrainerServices = trainerServices;
            
            this.EffectText = "Launch";
            this.HotkeyText = hotkeyText;
            this.ChangeHotkeyCommand = new(this);
            this.OpenOptionsWindowCommand = new RelayCommand(o => { OpenOptionsWindow(); }, o => true);
            this.ExecuteCommand = new RelayCommand(o => { ExecuteLaunch(); }, o => true);

        }

      




        public void ExecuteLaunch()
        {
            try
            {
                this.TrainerServices.BoostForward(LaunchSpeed);
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to execute Launch! \n" + ex.Message + ex.StackTrace, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            }
        }

        public void OnHotkeyPress()
        {
            ExecuteLaunch();
        }
    }
}
