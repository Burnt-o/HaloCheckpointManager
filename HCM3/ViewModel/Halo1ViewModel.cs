using HCM3.ViewModel.MVVM;
using HCM3.Model;
using System.Collections.ObjectModel;
using System.Windows.Input;

namespace HCM3.ViewModel
{

    internal sealed class Halo1ViewModel : Presenter
    {
        private readonly Halo1Model _model;
        private string _someText = string.Empty;

        public Halo1ViewModel(Halo1Model model, ObservableCollection<string> history)
        {
            (_model, History) = (model, history);
        }
        public string SomeText
        {
            get => _someText;
            set => Update(ref _someText, value);
        }

        public ObservableCollection<string> History { get; }

        public ICommand PrintTextCommand => new Command(_ => _model.PrintText());

        private void OnUpdate() => SomeText = string.Empty;
    }

}
