using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using HCM3.Models;
using System.Collections.ObjectModel;
using System.Windows.Input;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Diagnostics;
using HCM3.Services;
using NonInvasiveKeyboardHookLibrary;
using HCM3.Services.HaloMemory;

namespace HCM3.ViewModels
{
    internal sealed class MainViewModel : Presenter
    {
        private int _selectedTabIndex;
        public int SelectedTabIndex
        {
            get { return _selectedTabIndex; }
            set
            {
                _selectedTabIndex = value;
                OnPropertyChanged(nameof(SelectedTabIndex));
            }
        }

        //public MainModel MainModel { get; set; }

        public int SelectedGame { get; set; }
        public CheckpointViewModel CheckpointViewModel { get; init; }

        public TrainerViewModel TrainerViewModel { get; init; }

        public HaloMemoryService HaloMemoryService { get; init; }

        public KeyboardHookManager KeyboardHookManager { get; init; }

        public SettingsViewModel SettingsViewModel { get; init; }


        public MainViewModel(HaloMemoryService haloMemoryService, CheckpointViewModel checkpointViewModel, TrainerViewModel trainerViewModel, SettingsViewModel settingsViewModel) //(MainModel mainModel, HaloMemoryService haloMemoryService, CheckpointViewModel checkpointViewModel)
        {
 
            this.SettingsViewModel = settingsViewModel; 

            this.TrainerViewModel = trainerViewModel;

            this.HaloMemoryService = haloMemoryService;

            // Note to self; is this the best way to do this?
            this.CheckpointViewModel = checkpointViewModel;

            this.PropertyChanged += Handle_PropertyChanged;

            // Subscribe to checkpoint view model requesting a tab change
            this.CheckpointViewModel.RequestTabChange += Realign_Tab;


            // Need to subscribe to HaloStateChanged of mainmodel, and tab changed of this mainViewModel,
            HaloStateEvents.HALOSTATECHANGED_EVENT += (obj, args) => { IsSelectedGameSameAsActualGame(args.NewHaloState); };

            // Load in selected tab to what it was when HCM closed last
            SelectedTabIndex = Properties.Settings.Default.LastSelectedTab;

            KeyboardHookManager = new ();
            KeyboardHookManager.Start();

            // Register virtual key code 0x60 = NumPad0
            KeyboardHookManager.RegisterHotkey(0x60, () =>
            {
                Trace.WriteLine("NumPad0 detected");
            });


        }


        private void Handle_PropertyChanged(object? sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(SelectedTabIndex))
            {
                if (SelectedTabIndex != 6) //Settings tab == 6
                {
                    Trace.WriteLine("TAB WAS CHANGED");
                    CheckpointViewModel.SelectedGame = SelectedTabIndex;
                    TrainerViewModel.SelectedGame = SelectedTabIndex;
                    this.SelectedGame = SelectedTabIndex;
                    this.CheckpointViewModel.RefreshSaveFolderTree();
                    this.CheckpointViewModel.RefreshCheckpointList();
                }
                IsSelectedGameSameAsActualGame();

                // Save the selected tab to settings so we can load it in next time HCM starts
                Properties.Settings.Default.LastSelectedTab = SelectedTabIndex;
            }
        }


        private void Realign_Tab(object? sender, int requestedTab)
        { 
            if (SelectedTabIndex != requestedTab && requestedTab >= 0 && requestedTab <= 5)
        SelectedTabIndex = requestedTab;
        }

        private void IsSelectedGameSameAsActualGame(HaloState? haloState = null)
        {
            if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
            CheckpointViewModel.SelectedGameSameAsActualGame = (this.SelectedGame == (int)haloState.GameState);
            TrainerViewModel.SelectedGameSameAsActualGame = (this.SelectedGame == (int)haloState.GameState);
        }


    }
}
