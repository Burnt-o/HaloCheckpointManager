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
        private int _selectedTabIndex; // TODO: recheck this logic maybe related to nullness
        public int SelectedTabIndex 
        {
            get { return _selectedTabIndex; }
            set 
            { 
            _selectedTabIndex = value;
            OnPropertyChanged(nameof(SelectedTabIndex));
                //Trace.WriteLine("tabindex was set");
            }
        }

        private readonly MainModel _model;
        
        public ObservableCollection<Checkpoint> CheckpointCollection { get; set; }
        public CheckpointModel CheckpointModel { get; set; }
        public CheckpointViewModel CheckpointViewModel { get; set; }

        public MainViewModel(MainModel model, TabItem Halo1, TabItem Halo2)
        {
            _model = model;
            HaloMemory HaloMemory = new();

            CheckpointCollection = new();
            CheckpointModel = new(HaloMemory, CheckpointCollection);
            CheckpointViewModel = new(CheckpointModel, CheckpointCollection);
            Halo1.DataContext = CheckpointViewModel;
            Halo2.DataContext = CheckpointViewModel;


            Checkpoint checkpoint = new Checkpoint("hiii");
            CheckpointCollection.Add(checkpoint);

            checkpoint.CheckpointName = "cum";
            this.PropertyChanged += SelectedTabIndex_PropertyChanged;
        }




        //handler
        private void SelectedTabIndex_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            // TODO: set the settings option in here (note to self; let's just make the settings only for actually loading into HCM; we can store everything else here in mainviewmodel. Maybe just make the checkpointview subscribe to this sti event?)
            Trace.WriteLine("STI event raised woo! sender: " + sender.ToString() + ", e: " + e.PropertyName);

            // TODO: tell the checkpointviewmodel to refresh it's list. or just make it subscribe to this idk
            // TODO: tell the trainerviewmodel to er .. actually not sure how that's gonna work yet
        }
    }
}
