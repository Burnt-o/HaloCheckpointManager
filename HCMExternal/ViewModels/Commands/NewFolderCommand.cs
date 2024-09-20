using HCMExternal.Services.CheckpointServiceNS;
using System;
using System.Windows.Input;

namespace HCMExternal.ViewModels.Commands
{
    internal class NewFolderCommand : ICommand
    {
        internal NewFolderCommand(CheckpointViewModel checkpointViewModel, CheckpointService checkpointServices)
        {
            CheckpointViewModel = checkpointViewModel;
            CheckpointServices = checkpointServices;
        }

        private CheckpointViewModel CheckpointViewModel { get; init; }
        private CheckpointService CheckpointServices { get; init; }
        public bool CanExecute(object? parameter)
        {
            return (CheckpointViewModel.SelectedSaveFolder != null);
        }

        public void Execute(object? parameter)
        {
            try
            {

                CheckpointServices.NewFolder(CheckpointViewModel.SelectedSaveFolder);
                CheckpointViewModel.RefreshSaveFolderTree();
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to create new folder! \n" + ex.ToString(), "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
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
