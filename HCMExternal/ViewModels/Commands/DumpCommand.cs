using Serilog;
using System;
using System.Windows.Input;

namespace HCMExternal.ViewModels.Commands
{
    public class DumpCommand : ICommand
    {
        internal DumpCommand()
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

        public static event Action DumpEvent = delegate { Log.Information("Firing dump event"); };

        public bool CanExecute(object? parameter)
        {
            //TODO
            return true;
        }

        public void Execute(object? parameter)
        {
            try
            {
                DumpEvent();
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to dump! \n" + ex.ToString(), "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
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
