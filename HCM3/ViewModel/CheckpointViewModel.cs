using HCM3.ViewModel.MVVM;
using HCM3.Model;
using System.Collections.ObjectModel;
using System.Windows.Input;

namespace HCM3.ViewModel
{

    internal class CheckpointViewModel : Presenter
    {
        private readonly CheckpointModel CheckpointModel;
        public ObservableCollection<Checkpoint> CheckpointCollection { get; set; }

        public CheckpointViewModel(CheckpointModel checkpointModel)
        {
            CheckpointModel = checkpointModel;
            CheckpointCollection = CheckpointModel.CheckpointCollection;
        }

        public ICommand PrintTextCommand => new Command(_ => CheckpointModel.PrintText());

    }

}
