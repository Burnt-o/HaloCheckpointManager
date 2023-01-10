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
    public class DumpCommand : ICommand
    {
        internal DumpCommand(CheckpointViewModel checkpointViewModel, CheckpointServices checkpointServices, TrainerServices trainerServices)
        {
            this.CheckpointServices = checkpointServices;
            this.CheckpointViewModel = checkpointViewModel;
            this.TrainerServices = trainerServices;

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
        private TrainerServices TrainerServices { get; init; }
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
                Trace.WriteLine("Aligning tab to game");

                this.CheckpointServices.CommonServices.HaloMemoryService.HaloState.UpdateHaloState();
                HaloState actualGame = this.CheckpointServices.CommonServices.HaloMemoryService.HaloState;

                if (actualGame.CurrentHaloState != -1)
                {
                    CheckpointViewModel.InvokeRequestTabChange(actualGame.CurrentHaloState);

                    CheckpointViewModel.RefreshCheckpointList();
                }

                Trace.WriteLine("CheckpointViewModel.SelectedSaveFolder after realigning tab to game: " + CheckpointViewModel.SelectedSaveFolder);

                //Check if user wants automatic checkpoint when dumping
                if (Properties.Settings.Default.AutoCheckpoint)
                {
                    if (Properties.Settings.Default.H1Cores && CheckpointViewModel.SelectedGame == 0)
                    {
                        TrainerServices.ForceCoreSave();
                    }
                    else
                    {
                        TrainerServices.ForceCheckpoint();
                    }
                    System.Threading.Thread.Sleep(50); //Give MCC enough time to actually make the checkpoint/coresave
                }

                CheckpointServices.TryDump(CheckpointViewModel.SelectedSaveFolder, CheckpointViewModel.SelectedGame);
                CheckpointViewModel.RefreshCheckpointList();
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to dump! \n" + ex.ToString(), "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
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
