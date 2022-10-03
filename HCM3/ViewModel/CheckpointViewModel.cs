using HCM3.ViewModel.MVVM;
using HCM3.Model;
using System.Collections.ObjectModel;
using System.Windows.Input;
using HCM3.Model.CheckpointModels;

namespace HCM3.ViewModel
{

    internal class CheckpointViewModel : Presenter
    {
        private readonly CheckpointModel CheckpointModel;
        public ObservableCollection<Checkpoint> CheckpointCollection { get; private set; }
        public ObservableCollection<SaveFolder> SaveFolderHierarchy { get; private set; }

        public CheckpointViewModel(CheckpointModel checkpointModel)
        {
            this.CheckpointModel = checkpointModel;
            this.CheckpointCollection = new ObservableCollection<Checkpoint>(CheckpointModel.CheckpointCollection);
            this.SaveFolderHierarchy = new ObservableCollection<SaveFolder>(CheckpointModel.SaveFolderHierarchy);
        }

        public ICommand PrintTextCommand => new Command(_ => CheckpointModel.PrintText());

    }

}
