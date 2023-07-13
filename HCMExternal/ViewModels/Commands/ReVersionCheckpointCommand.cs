using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using HCMExternal.Models;
using HCMExternal.ViewModels;
using System.Diagnostics;
using HCMExternal.Services.CheckpointServiceNS;

namespace HCMExternal.ViewModels.Commands
{
    internal class ReVersionCheckpointCommand : ICommand
    {
        internal ReVersionCheckpointCommand(CheckpointViewModel checkpointViewModel, CheckpointService checkpointServices)
        {
            this.CheckpointViewModel = checkpointViewModel;
            this.CheckpointServices = checkpointServices;
        }

        private CheckpointViewModel CheckpointViewModel { get; init; }
        private CheckpointService CheckpointServices { get; init; }
        public bool CanExecute(object? parameter)
        {
            return (CheckpointViewModel.SelectedCheckpoint != null);
        }

        public void Execute(object? parameter)
        {
            try
            {

                CheckpointServices.ReVersionCheckpoint(CheckpointViewModel.SelectedSaveFolder, CheckpointViewModel.SelectedCheckpoint);
                CheckpointViewModel.RefreshCheckpointList();
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to re-version checkpoint! \n" + ex.ToString(), "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
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
