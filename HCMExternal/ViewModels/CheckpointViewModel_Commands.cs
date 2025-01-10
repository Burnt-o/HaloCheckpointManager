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

    public partial class CheckpointViewModel : Presenter, IDropTarget
    {
       
        public RelayCommand DeleteCheckpoint { get; init; }
        private void onDeleteCheckpoint()
        {
            cpservice.DeleteCheckpoint(SelectedSaveFolder, SelectedCheckpoint);
            RefreshCheckpointList();
        }

        public RelayCommand RenameCheckpoint { get; init; }
        private void onRenameCheckpoint()
        {
            cpservice.RenameCheckpoint(SelectedSaveFolder, SelectedCheckpoint);
        }

        public RelayCommand ReVersionCheckpoint { get; init; }
        private void onReVersionCheckpoint()
        {
            cpservice.ReVersionCheckpoint(SelectedSaveFolder, SelectedCheckpoint);
        }

        public RelayCommand SortCheckpoint { get; init; }
        private void onSortCheckpoint()
        {
            Application.Current.Dispatcher.Invoke(delegate
            {
                SortCheckpointsView win = new(cpservice, this)
                {
                    Owner = App.Current.MainWindow // makes the dialog be centered on the main window
                };
                win.ShowDialog();
            });
        }

        public RelayCommand OpenInExplorer { get; init; }
        private void onOpenInExplorer()
        {
            cpservice.OpenInExplorer(SelectedSaveFolder);
        }

        public RelayCommand RenameFolder { get; init; }
        private void onRenameFolder()
        {
            cpservice.RenameFolder(SelectedSaveFolder);
            RefreshSaveFolderTree();
        }

        public RelayCommand DeleteFolder { get; init; }
        private void onDeleteFolder()
        {
            cpservice.DeleteFolder(SelectedSaveFolder);
            RefreshSaveFolderTree();
            RefreshCheckpointList();
        }

        public RelayCommand NewFolder { get; init; }
        private void onNewFolder()
        {
            cpservice.NewFolder(SelectedSaveFolder);
            RefreshSaveFolderTree();
        }

        public RelayCommand ForceCheckpoint { get; init; }
        private void onForceCheckpoint()
        {
            exservice.ForceCheckpoint();
        }

        public RelayCommand ForceRevert { get; init; }
        private void onForceRevert()
        {
            exservice.ForceRevert();
        }

        public RelayCommand ForceDoubleRevert { get; init; }
        private void onForceDoubleRevert()
        {
            exservice.ForceDoubleRevert();
        }

        public RelayCommand DumpCheckpoint { get; init; }
        private void onDumpCheckpoint()
        {
            exservice.DumpCheckpoint(SelectedSaveFolder);

            App.Current.Dispatcher.Invoke((Action)delegate
            {
                RefreshCheckpointList();
            });
            
        }

        public RelayCommand InjectCheckpoint { get; init; }
        private void onInjectCheckpoint()
        {

            if (SelectedCheckpoint == null)
                System.Windows.MessageBox.Show("Failed to Inject! \n" + "No checkpoint selected!", "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            else
            {
                if (SelectedGame.Equals(HaloGame.ProjectCartographer))
                    exservice.InjectCheckpoint(SelectedCheckpoint);
                else
                    ipservice.UpdateSharedMemQueueInjectCommand();
            }
                
        }



    }




}
