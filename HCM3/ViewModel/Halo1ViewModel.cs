using HCM3.ViewModel.MVVM;
using HCM3.Model;
using System.Collections.ObjectModel;
using System.Windows.Input;

namespace HCM3.ViewModel
{

    internal sealed class Halo1ViewModel : Presenter
    {
        public string LevelName { get; set; } // for testing
        private readonly Halo1Model _model;
        public ObservableCollection<Checkpoint> CheckpointCollectionH1 { get; set; }

        public Halo1ViewModel(Halo1Model model, ObservableCollection<Checkpoint> checkpointCollectionH1)
        {
           // (_model, CheckpointCollectionH1) = (model, checkpointCollectionH1);
           _model = model;
            //TODO remove this, let it get passed in by constructor
            CheckpointCollectionH1 = checkpointCollectionH1;
            LevelName = "ARGH";
        }

        public ICommand PrintTextCommand => new Command(_ => _model.PrintText());

    }

}
