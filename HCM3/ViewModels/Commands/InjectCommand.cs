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
using HCM3.Services.Trainer;

namespace HCM3.ViewModels.Commands
{
    internal class InjectCommand : ICommand
    {
        internal InjectCommand(CheckpointViewModel checkpointViewModel, CheckpointServices checkpointServices, TrainerServices trainerServices)
        {
            this.CheckpointViewModel = checkpointViewModel;
            this.CheckpointServices = checkpointServices;
            this.TrainerServices = trainerServices;

            CheckpointViewModel.PropertyChanged += (obj, args) =>
            {
                if (args.PropertyName == nameof(CheckpointViewModel.SelectedGameSameAsActualGame))
                {
                    RaiseCanExecuteChanged();
                }
                else if (args.PropertyName == nameof(CheckpointViewModel.SelectedCheckpoint))
                {
                    RaiseCanExecuteChanged();
                }
            };
        }

        private CheckpointViewModel CheckpointViewModel { get; init; }
        private CheckpointServices CheckpointServices { get; init; }
        private TrainerServices TrainerServices { get; init; }
        public bool CanExecute(object? parameter)
        {
            //return true;
            Trace.WriteLine("inject Can execute checked");
            return (CheckpointViewModel.SelectedGameSameAsActualGame && CheckpointViewModel.SelectedCheckpoint != null);
        }

        public void Execute(object? parameter)
        {
            try
            {
                CheckpointServices.TryInject(CheckpointViewModel.SelectedSaveFolder, CheckpointViewModel.SelectedCheckpoint, CheckpointViewModel.SelectedGame);

                if (Properties.Settings.Default.AutoRevert)
                {
                    if (Properties.Settings.Default.H1Cores && CheckpointViewModel.SelectedGame == 0)
                    {
                        TrainerServices.ForceCoreLoad();
                    }
                    else
                    {
                        TrainerServices.ForceRevert();
                    }
                    
                }

            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to Inject! \n" + ex.ToString(), "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
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
