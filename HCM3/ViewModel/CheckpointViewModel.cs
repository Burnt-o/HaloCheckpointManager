using HCM3.ViewModel.MVVM;
using HCM3.Model;
using System.Collections.ObjectModel;
using System.Windows.Input;
using HCM3.Model.CheckpointModels;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows.Controls;
using System.Windows;
using System;
using Microsoft.Xaml.Behaviors;
using System.Collections;
using System.Collections.Specialized;

namespace HCM3.ViewModel
{

    internal class CheckpointViewModel : Presenter
    {
        private readonly CheckpointModel CheckpointModel;
        public ObservableCollection<Checkpoint> CheckpointCollection { get; private set; }
        public ObservableCollection<SaveFolder> SaveFolderHierarchy { get; private set; }

        public SaveFolder? RootSaveFolder { get; private set; }

        public MainViewModel MainViewModel { get; private set; }

        public MainModel MainModel { get; private set; }

        private Checkpoint? _selectedCheckpoint;
        public Checkpoint? SelectedCheckpoint
        { 
            get { return _selectedCheckpoint; }
            set 
            { 
            _selectedCheckpoint = value;
                OnPropertyChanged(nameof(SelectedCheckpoint));
            }
        }

        public CheckpointViewModel(CheckpointModel checkpointModel, MainViewModel mainViewModel, MainModel mainModel)
        {
            this.CheckpointModel = checkpointModel;
            this.CheckpointCollection = CheckpointModel.CheckpointCollection;
            this.SaveFolderHierarchy = CheckpointModel.SaveFolderHierarchy;
            this.RootSaveFolder = CheckpointModel.RootSaveFolder;
            this.PropertyChanged += Handle_PropertyChanged;
            this.MainViewModel = mainViewModel;
            this.MainModel = mainModel;


        }

        private void Handle_PropertyChanged(object? sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(SelectedCheckpoint))
            {
                Trace.WriteLine("selected checkpoint changed");
                CheckpointModel.SelectedCheckpoint = SelectedCheckpoint;
            }

        }



        private ICommand _dump;
        public ICommand Dump
        {
            get { return _dump ?? (_dump = new DumpCommand(CheckpointModel)); }
            set { _dump = value; }
        }
        public ICommand Inject { get; private set; }

        public void FolderChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            SaveFolder? saveFolder = (SaveFolder?)e.NewValue;
            Trace.WriteLine("Selected Folder Path: " + saveFolder?.SaveFolderPath);

            if (saveFolder != null)
            {
                Properties.Settings.Default.LastSelectedFolder[MainViewModel.SelectedTabIndex] = saveFolder.SaveFolderPath;
            }
            
            CheckpointModel.SelectedSaveFolder = saveFolder;
            CheckpointModel.RefreshCheckpointList();

            
        }

        public class DumpCommand : ICommand
        {
            public DumpCommand(CheckpointModel checkpointModel)
            {
                CheckpointModel = checkpointModel;
            }

            private CheckpointModel CheckpointModel { get; set; }
            public bool CanExecute(object? parameter)
            {
                return true;
            }

            public void Execute(object? parameter)
            {
                try
                {
                    CheckpointModel.TryDump();
                }
                catch (Exception ex)
                {
                    System.Windows.MessageBox.Show("Failed to dump! \n" + ex.Message, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
                }
                
            }

            public event EventHandler? CanExecuteChanged;
        }


    }




}
