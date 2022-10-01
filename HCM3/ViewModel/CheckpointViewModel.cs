using HCM3.ViewModel.MVVM;
using HCM3.Model;
using System.Collections.ObjectModel;
using System.Windows.Input;

namespace HCM3.ViewModel
{

    internal class CheckpointViewModel : Presenter
    {
        private readonly CheckpointModel _model;
        public ObservableCollection<Checkpoint> CheckpointCollection { get; set; }

        public CheckpointViewModel(CheckpointModel model, ObservableCollection<Checkpoint> checkpointCollection)
        {
           (_model, CheckpointCollection) = (model, checkpointCollection);
        }

        public ICommand PrintTextCommand => new Command(_ => _model.PrintText());

    }

}
