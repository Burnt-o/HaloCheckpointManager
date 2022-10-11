using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using HCM3.Model;
using HCM3.ViewModel;
using System.Diagnostics;

namespace HCM3.ViewModel.Commands
{
    internal class DeleteCheckpointCommand : ICommand
    {
        internal DeleteCheckpointCommand(CheckpointViewModel checkpointViewModel)
        {
            CheckpointViewModel = checkpointViewModel;
        }

        private CheckpointViewModel CheckpointViewModel { get; set; }
        public bool CanExecute(object? parameter)
        {
            return (CheckpointViewModel.SelectedCheckpoint != null);
        }

        public void Execute(object? parameter)
        {
            try
            {
                CheckpointViewModel.CheckpointModel.DeleteCheckpoint(CheckpointViewModel.SelectedSaveFolder, CheckpointViewModel.SelectedCheckpoint);
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
