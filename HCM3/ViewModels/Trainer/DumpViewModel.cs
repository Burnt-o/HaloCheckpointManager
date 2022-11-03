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
    internal class DumpViewModel : Presenter, IControlWithHotkey
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


        #endregion

        TrainerServices TrainerServices { get; init; }

        public void OpenOptionsWindow()
        {
            LaunchOptionsView win = new LaunchOptionsView();
            win.DataContext = this;
            win.ShowDialog();
        }

        public DumpViewModel()
        {
            //parameterless constructor for design view
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }

        public DumpViewModel(string hotkeyText, string effectText, CheckpointViewModel? checkpointViewModel)
        {



            this.EffectText = effectText;
            this.HotkeyText = hotkeyText;

            this.ChangeHotkeyCommand = new(this);
            this.OpenOptionsWindowCommand = new RelayCommand(o => { OpenOptionsWindow(); }, o => true);

            if (checkpointViewModel != null)
            {
                this.ExecuteCommand = checkpointViewModel.Dump;
            }

            
            //this.ExecuteCommand = new RelayCommand(o => { ExecuteLaunch(); }, o => true);

        }







        public void OnHotkeyPress()
        {
            ExecuteCommand.Execute(null);
        }
    }
}
