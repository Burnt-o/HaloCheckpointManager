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


namespace HCM3.ViewModel
{

    internal class CheckpointViewModel : Presenter
    {
        private readonly CheckpointModel CheckpointModel;
        public ObservableCollection<Checkpoint> CheckpointCollection { get; private set; }
        public ObservableCollection<SaveFolder> SaveFolderHierarchy { get; private set; }

        private SaveFolder? _selectedSaveFolder;
        public SaveFolder? SelectedSaveFolder
        {
            get { return _selectedSaveFolder; }
            set
            {
                _selectedSaveFolder = value;
                OnPropertyChanged(nameof(SelectedSaveFolder));
            }
        }

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

        public CheckpointViewModel(CheckpointModel checkpointModel)
        {
            this.CheckpointModel = checkpointModel;
            this.CheckpointCollection = CheckpointModel.CheckpointCollection;
            this.SaveFolderHierarchy = CheckpointModel.SaveFolderHierarchy;

            this.PropertyChanged += Handle_PropertyChanged;



        }

        private void Handle_PropertyChanged(object? sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(SelectedCheckpoint))
            {
                Trace.WriteLine("selected checkpoint changed");
            }
            else if (e.PropertyName == nameof(SelectedSaveFolder))
            {

                Trace.WriteLine("uhhhhhhhhhhhhhhhhhhhhhhhhhhh " + SelectedSaveFolder?.SaveFolderName);
            }
        }

        public ICommand PrintTextCommand => new Command(_ => CheckpointModel.PrintText());

        public void FolderChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            Trace.WriteLine("let's do it the easy way");
            SaveFolder? saveFolder = (SaveFolder?)e.NewValue;
            if (saveFolder != null)
            {
                Trace.WriteLine("Selected Folder Path: " + saveFolder.SaveFolderPath);
            }
        }

    }

}
