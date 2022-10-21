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





        public MainViewModel(HaloMemoryService haloMemoryService, CheckpointViewModel checkpointViewModel, TrainerViewModel trainerViewModel) //(MainModel mainModel, HaloMemoryService haloMemoryService, CheckpointViewModel checkpointViewModel)
        {
 

            this.TrainerViewModel = trainerViewModel;

            this.HaloMemoryService = haloMemoryService;

            // Note to self; is this the best way to do this?
            this.CheckpointViewModel = checkpointViewModel;

            this.PropertyChanged += Handle_PropertyChanged;

            // Need to subscribe to HaloStateChanged of mainmodel, and tab changed of this mainViewModel,
            HaloStateEvents.HALOSTATECHANGED_EVENT += (obj, args) => { IsSelectedGameSameAsActualGame(); };

            // Load in selected tab to what it was when HCM closed last
            SelectedTabIndex = Properties.Settings.Default.LastSelectedTab;

        }


        private void Handle_PropertyChanged(object? sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(SelectedTabIndex))
            {
                if (SelectedTabIndex != 6) //Settings tab == 6
                {
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

        private void IsSelectedGameSameAsActualGame()
        {
            CheckpointViewModel.SelectedGameSameAsActualGame = (this.SelectedGame == this.HaloMemoryService.HaloState.CurrentHaloState);
            TrainerViewModel.SelectedGameSameAsActualGame = (this.SelectedGame == this.HaloMemoryService.HaloState.CurrentHaloState);
        }


    }
}
