using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModel.MVVM;
using HCM3.Model;
using System.Collections.ObjectModel;
using System.Windows.Input;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Diagnostics;

namespace HCM3.ViewModel
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

        public MainModel MainModel { get; set; }

        public CheckpointViewModel CheckpointViewModel { get; init; }

        public MainViewModel(MainModel mainModel)
        {
            MainModel = mainModel;

            CheckpointViewModel = new(MainModel.CheckpointModel);

            this.PropertyChanged += Handle_PropertyChanged;

            // Load in selected tab to what it was when HCM closed last
            SelectedTabIndex = Properties.Settings.Default.LastSelectedTab;
        }


        private void Handle_PropertyChanged(object? sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(SelectedTabIndex))
            {
                // Tell main model that the tab changed so it can refresh checkpoint details to be of the correct game/folder
                MainModel.HCMTabChanged(SelectedTabIndex);

                // Save the selected tab to settings so we can load it in next time HCM starts
                Properties.Settings.Default.LastSelectedTab = SelectedTabIndex;
            }
        }
    }
}
