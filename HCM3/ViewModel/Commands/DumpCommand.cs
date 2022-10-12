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
    internal class DumpCommand : ICommand
    {
        internal DumpCommand(CheckpointViewModel checkpointViewModel)
        {
            CheckpointViewModel = checkpointViewModel;

            // Note to self. Need to restructure how viewmodel vs model has selectedtabindex, selected checkpoint etc etc. 
            // Also this command should probably be getting handed the viewmodel instead of the model

            //HaloStateEvents.HALOSTATECHANGED_EVENT += (obj, args) => { RaiseCanExecuteChanged(); };
            CheckpointViewModel.PropertyChanged += (obj, args) =>
            {
                if (args.PropertyName == nameof(CheckpointViewModel.SelectedGameSameAsActualGame))
                {
                    RaiseCanExecuteChanged();
                }
            };
        }

        private CheckpointViewModel CheckpointViewModel { get; set; }
        public bool CanExecute(object? parameter)
        {
            //return true;
            Trace.WriteLine("dump CanExecute checked");
            return (CheckpointViewModel.SelectedGameSameAsActualGame);
        }

        public void Execute(object? parameter)
        {
            try
            {
                CheckpointViewModel.CheckpointModel.TryDump(CheckpointViewModel.SelectedSaveFolder);
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
