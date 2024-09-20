using Serilog;
using System;
using System.Windows.Input;


namespace HCMExternal.ViewModels.Commands
{
    internal class InjectCommand : ICommand
    {
        internal InjectCommand()
        {

            //TODO
            //CheckpointViewModel.PropertyChanged += (obj, args) =>
            //{
            //    if (args.PropertyName == nameof(CheckpointViewModel.SelectedGameSameAsActualGame))
            //    {
            //        RaiseCanExecuteChanged();
            //    }
            //    else if (args.PropertyName == nameof(CheckpointViewModel.SelectedCheckpoint))
            //    {
            //        RaiseCanExecuteChanged();
            //    }
            //};
        }

        public static event Action InjectEvent = delegate { Log.Information("Firing dump event"); };

        public bool CanExecute(object? parameter)
        {
            return true;
            //TODO
        }

        public void Execute(object? parameter)
        {
            try
            {
                InjectEvent();
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to Inject! \n" + ex.ToString(), "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            }

        }

        public void RaiseCanExecuteChanged()
        {
            App.Current.Dispatcher.Invoke(delegate // Need to make sure it's run on the UI thread
            {
                _canExecuteChanged?.Invoke(this, EventArgs.Empty);
            });

        }

        private EventHandler? _canExecuteChanged;

        public event EventHandler? CanExecuteChanged
        {
            add
            {
                _canExecuteChanged += value;
                CommandManager.RequerySuggested += value;
            }
            remove
            {
                _canExecuteChanged -= value;
                CommandManager.RequerySuggested -= value;
            }
        }


    }
}
