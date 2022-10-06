using HCM3.ViewModel.MVVM;
using HCM3.Model;
using System.Collections.ObjectModel;
using System.Windows.Input;
using HCM3.Model.CheckpointModels;
using System.ComponentModel;
using System.Diagnostics;

namespace HCM3.ViewModel
{

    internal class CheckpointViewModel : Presenter
    {
        private readonly CheckpointModel CheckpointModel;
        public ObservableCollection<Checkpoint> CheckpointCollection { get; private set; }
        public ObservableCollection<SaveFolder> SaveFolderHierarchy { get; private set; }

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
        }

        public ICommand PrintTextCommand => new Command(_ => CheckpointModel.PrintText());

    }

}
