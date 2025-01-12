using GongSolutions.Wpf.DragDrop;
using HCMExternal.Models;
using HCMExternal.Services.CheckpointIO;
using HCMExternal.ViewModels.Commands;
using HCMExternal.ViewModels.Interfaces;
using HCMExternal.Views;
using Serilog;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading;
using System.Windows;
using System.Windows.Data;
using System.Windows.Input;


namespace HCMExternal.ViewModels
{

    public partial class MainViewModel : Presenter
    {
       

        private void onDeleteCheckpoint()
        {
            _checkpointIOService.DeleteCheckpoint(FileViewModel.SelectedSaveFolder, FileViewModel.SelectedCheckpoint);
            FileViewModel.UpdateCheckpointCollection();
        }


        private void onRenameCheckpoint()
        {
            _checkpointIOService.RenameCheckpoint(FileViewModel.SelectedSaveFolder, FileViewModel.SelectedCheckpoint);
        }


        private void onReVersionCheckpoint()
        {
            _checkpointIOService.ReVersionCheckpoint(FileViewModel.SelectedSaveFolder, FileViewModel.SelectedCheckpoint);
        }


        private void onSortCheckpoint()
        {
            Application.Current.Dispatcher.Invoke(delegate
            {
                SortCheckpointsView win = new(_checkpointIOService, FileViewModel)
                {
                    Owner = App.Current.MainWindow // makes the dialog be centered on the main window
                };
                win.ShowDialog();
            });
        }


        private void onOpenInExplorer()
        {
            _checkpointIOService.OpenInExplorer(FileViewModel.SelectedSaveFolder);
        }


        private void onRenameFolder()
        {
            _checkpointIOService.RenameFolder(FileViewModel.SelectedSaveFolder);
            FileViewModel.UpdateSaveFolderCollection();
        }


        private void onDeleteFolder()
        {
            _checkpointIOService.DeleteFolder(FileViewModel.SelectedSaveFolder);
            FileViewModel.UpdateSaveFolderCollection();
            FileViewModel.UpdateCheckpointCollection();
        }


        private void onNewFolder()
        {
            _checkpointIOService.NewFolder(FileViewModel.SelectedSaveFolder);
            FileViewModel.UpdateSaveFolderCollection();
        }


        private void onForceCheckpoint()
        {
            _externalService.ForceCheckpoint();
        }


        private void onForceRevert()
        {
            _externalService.ForceRevert();
        }


        private void onForceDoubleRevert()
        {
            _externalService.ForceDoubleRevert();
        }


        private void onDumpCheckpoint()
        {
            _externalService.DumpCheckpoint(FileViewModel.SelectedSaveFolder);

            App.Current.Dispatcher.Invoke((Action)delegate
            {
                FileViewModel.UpdateCheckpointCollection();
            });
            
        }


        private void onInjectCheckpoint()
        {

            if (FileViewModel.SelectedCheckpoint == null)
                System.Windows.MessageBox.Show("Failed to Inject! \n" + "No checkpoint selected!", "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            else
            {
                if (FileViewModel.SelectedGame.Equals(HaloGame.ProjectCartographer))
                    _externalService.InjectCheckpoint(FileViewModel.SelectedCheckpoint);
                else
                    _interprocService.UpdateSharedMemQueueInjectCommand();
            }
                
        }

        private void onDisableCheckpoints(bool shouldDisableCheckpoint)
        {
            _externalService.DisableCheckpoints(shouldDisableCheckpoint);
        }



    }




}
