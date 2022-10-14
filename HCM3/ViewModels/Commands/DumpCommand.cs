using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using HCM3.Models;
using HCM3.ViewModels;
using System.Diagnostics;
using HCM3.Services;

namespace HCM3.ViewModels.Commands
{
    public class DumpCommand : ICommand
    {
        internal DumpCommand(CheckpointViewModel checkpointViewModel, CheckpointServices checkpointServices)
        {
            this.CheckpointServices = checkpointServices;
            this.CheckpointViewModel = checkpointViewModel;

            //TODO: add this to checkpoint view model ?
            //CheckpointViewModel.PropertyChanged += (obj, args) =>
            //{
            //    if (args.PropertyName == nameof(CheckpointViewModel.SelectedGameSameAsActualGame))
            //    {
            //        RaiseCanExecuteChanged();
            //    }
            //};
        }

        private CheckpointServices CheckpointServices { get; init; }
        private CheckpointViewModel CheckpointViewModel { get; init; }
        public bool CanExecute(object? parameter)
        {
            return true;
            Trace.WriteLine("dump CanExecute checked");
            return (CheckpointViewModel.SelectedGameSameAsActualGame);
        }

        public void Execute(object? parameter)
        {
            try
            {
                CheckpointServices.TryDump(CheckpointViewModel.SelectedSaveFolder, CheckpointViewModel.SelectedGame);
                CheckpointViewModel.RefreshCheckpointList();
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to dump! \n" + ex.Message, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            }

        }

        public void RaiseCanExecuteChanged()
        {
            App.Current.Dispatcher.Invoke((Action)delegate // Need to make sure it's run on the UI thread
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
