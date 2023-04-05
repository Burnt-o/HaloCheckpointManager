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
using HCM3.Services.Cheats;

namespace HCM3.ViewModels.Commands
{
    public class DumpCommand : ICommand
    {
        internal DumpCommand(CheckpointViewModel checkpointViewModel, CheckpointServices checkpointServices, CheatManagerService cheatManagerService)
        {
            this.CheckpointServices = checkpointServices;
            this.CheckpointViewModel = checkpointViewModel;
            this.CheatManagerService = cheatManagerService;

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
        private CheatManagerService CheatManagerService { get; init; }
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

                HaloState haloState = this.CheckpointServices.HaloMemoryService.HaloMemoryManager.UpdateHaloState();

                if (haloState.GameState != Helpers.GameStateEnum.Unattached)
                {
                    CheckpointViewModel.InvokeRequestTabChange((int)haloState.GameState);

                    CheckpointViewModel.RefreshCheckpointList();
                }

                Trace.WriteLine("CheckpointViewModel.SelectedSaveFolder after realigning tab to game: " + CheckpointViewModel.SelectedSaveFolder);

                //Check if user wants automatic checkpoint when dumping
                if (Properties.Settings.Default.AutoCheckpoint)
                {
                    if (Properties.Settings.Default.H1Cores && CheckpointViewModel.SelectedGame == 0)
                    {
                        CheatManagerService.ListOfCheats["ForceCoreSave"].DoCheat();
                    }
                    else
                    {
                        CheatManagerService.ListOfCheats["ForceCheckpoint"].DoCheat();
                    }
                    System.Threading.Thread.Sleep(50); //Give MCC enough time to actually make the checkpoint/coresave
                }

                this.CheatManagerService.ListOfCheats["DumpCheckpoint"].DoCheat();
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
