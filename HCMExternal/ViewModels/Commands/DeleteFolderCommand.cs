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
    internal class DeleteFolderCommand : ICommand
    {
        internal DeleteFolderCommand(CheckpointViewModel checkpointViewModel, CheckpointService checkpointServices)
        {
            this.CheckpointViewModel = checkpointViewModel;
            this.CheckpointServices = checkpointServices;
        }

        private CheckpointViewModel CheckpointViewModel { get; init; }
        private CheckpointService CheckpointServices { get; init; }
        public bool CanExecute(object? parameter)
        {
            return (CheckpointViewModel.SelectedSaveFolder != null && CheckpointViewModel.SelectedSaveFolder != CheckpointViewModel.RootSaveFolder);
        }

        public void Execute(object? parameter)
        {
            try
            {
                CheckpointServices.DeleteFolder(CheckpointViewModel.SelectedSaveFolder);
                CheckpointViewModel.RefreshSaveFolderTree();
                CheckpointViewModel.RefreshCheckpointList();
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to delete savefolder! \n" + ex.ToString(), "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            }

        }

        public void RaiseCanExecuteChanged()
        {
            HCMExternal.App.Current.Dispatcher.Invoke((Action)delegate // Need to make sure it's run on the UI thread
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
