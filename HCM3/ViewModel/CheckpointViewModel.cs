using HCM3.ViewModel.MVVM;
using HCM3.Model;
using System.Collections.ObjectModel;
using System.Windows.Input;
using HCM3.Model.CheckpointModels;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows.Controls;
using System.Windows;
using System;
using Microsoft.Xaml.Behaviors;
using System.Collections;
using System.Collections.Specialized;
using System.Windows.Data;
using GongSolutions.Wpf.DragDrop;
using System.Windows.Controls;
using System.Windows.Shapes;
using System.Collections.Generic;
using System.Linq;

namespace HCM3.ViewModel
{

    internal class CheckpointViewModel : Presenter, IDropTarget
    {
        private readonly CheckpointModel CheckpointModel;
        public ObservableCollection<Checkpoint> CheckpointCollection { get; private set; }
        public ObservableCollection<SaveFolder> SaveFolderHierarchy { get; private set; }

        public SaveFolder? RootSaveFolder { get; private set; }

        public MainViewModel MainViewModel { get; private set; }

        public MainModel MainModel { get; private set; }

        private Checkpoint? _selectedCheckpoint;
        public Checkpoint? SelectedCheckpoint
        { 
            get { return _selectedCheckpoint; }
            set 
            { 
            _selectedCheckpoint = value;
                OnPropertyChanged(nameof(SelectedCheckpoint));
            }
        }

        void IDropTarget.DragOver(IDropInfo dropInfo)
        {
            // Get the types of the source and target items
            Type? sourceType = dropInfo.Data?.GetType();
            Type? targetType = dropInfo.TargetItem?.GetType();

            if (targetType == typeof(Checkpoint) && sourceType == typeof(SaveFolder))
            {
                // Can't drag a Folder into a Checkpoint
                return;
            }



            if (targetType == typeof(Checkpoint) && sourceType == typeof(Checkpoint))
            {
                //Trying to move a checkpoint relative to a checkpoint
                dropInfo.Effects = DragDropEffects.Move;
                dropInfo.DropTargetAdorner = DropTargetAdorners.Insert;
                return;
            }


            if (targetType == typeof(SaveFolder) && sourceType == typeof(Checkpoint))
            {
                //Trying to move a checkpoint into a different folder
                dropInfo.Effects = DragDropEffects.Move;
                dropInfo.DropTargetAdorner = DropTargetAdorners.Highlight;
                return;
            }

            if (targetType == typeof(SaveFolder) && sourceType == typeof(SaveFolder))
            {
                // Trying to move a SaveFolder relative to another SaveFolder (within the same level).
                // Only valid if both saveFolders share a parent directory.
                string? sourceParent = ((SaveFolder?)dropInfo.Data)?.ParentPath;
                string? targetParent = ((SaveFolder?)dropInfo.TargetItem)?.ParentPath;

                if (sourceParent == targetParent && sourceParent != null)
                {
                    dropInfo.Effects = DragDropEffects.Move;
                    //TODO: figure out why this wigs out with treeview, unlike the highlight adorner
                    dropInfo.DropTargetAdorner = DropTargetAdorners.Insert;
                }
                else
                {
                    dropInfo.Effects = DragDropEffects.None;
                }

                return;
            }





        }

        void IDropTarget.Drop(IDropInfo dropInfo)
        {
            // Get the types of the source and target items
            Type? sourceType = dropInfo.Data?.GetType();
            Type? targetType = dropInfo.TargetItem?.GetType();




            // Reorder two checkpoints in the checkpointlist (checkpoints in view are always ordered by their LastWriteTime (aka ModifiedOn))
            if (sourceType == typeof(Checkpoint) && targetType == typeof(Checkpoint))
            {
                //Grab the collection view (checkpoints in order of LWT)
                ListCollectionView checkpointCollectionView = (ListCollectionView)CollectionViewSource
                    .GetDefaultView(this.CheckpointCollection);

                // Need to cap the new index as dropInfo uses count + 1 if dragging to the bottom of list
                int newIndex = Math.Min(dropInfo.InsertIndex, checkpointCollectionView.Count - 1);
                int oldIndex = checkpointCollectionView.IndexOf(dropInfo.Data);

                if (newIndex == oldIndex) return; // No move (user dragged it back to itself)

                if (oldIndex < 0) return; // Shouldn't happen

                //Create a list of the checkpoints and their associated LWTs (in order) from the collectionView
                List<Checkpoint> listCheckpoints = new();
                List<DateTime?> listLWT = new();

                foreach (Checkpoint c in checkpointCollectionView)
                {
                    //Note that the two lists are in sync/paired.
                    listCheckpoints.Add(c);
                    listLWT.Add(c.ModifiedOn);
                }

                // Now we *move* a checkpoint in the list by, well changing it, but WITHOUT changing the list of LWT's (break the pairing)
                Checkpoint oldCheckpoint = listCheckpoints[oldIndex];
                listCheckpoints.RemoveAt(oldIndex);
                listCheckpoints.Insert(newIndex, oldCheckpoint);

                // Pass the two lists to checkpointmodel to actually apply the LWTs to the files.
                    // The new list of checkpoints gets the old list of LWTs, thus reordering them
                CheckpointModel.BatchModCheckpointLWTs(listCheckpoints, listLWT);

                // Refresh the list
                CheckpointModel.RefreshCheckpointList();
            }


            if (targetType == typeof(SaveFolder) && sourceType == typeof(SaveFolder))
            {
                string? sourceParent = ((SaveFolder?)dropInfo.Data)?.ParentPath;
                string? targetParent = ((SaveFolder?)dropInfo.TargetItem)?.ParentPath;

                if (sourceParent == targetParent && sourceParent != null)
                {
                    dropInfo.Effects = DragDropEffects.Move;
                    //TODO: figure out why this wigs out with treeview, unlike the highlight adorner
                    dropInfo.DropTargetAdorner = DropTargetAdorners.Insert;
                }
            }

            
           

            if (sourceType == typeof(Checkpoint) && targetType == typeof(SaveFolder))
            {
                // TODO: move checkpoint to different folder
                return;
            }

            if (sourceType == typeof(SaveFolder) && targetType == typeof(SaveFolder))
            {
                // TODO: reorder folders (or move one inside the other? need to figure that out?)
                // Actually, let's JUST support reordering within a level; that's the one thing the user can't do using windows explorer.
                // So we'll have to add a check that the source and target savefolders have the exact same parent. could add this to the dragover as well to make more clear.
                return;
            }



        }

        public CheckpointViewModel(CheckpointModel checkpointModel, MainViewModel mainViewModel, MainModel mainModel)
        {
            this.CheckpointModel = checkpointModel;
            this.CheckpointCollection = CheckpointModel.CheckpointCollection;
            this.SaveFolderHierarchy = CheckpointModel.SaveFolderHierarchy;
            this.RootSaveFolder = CheckpointModel.RootSaveFolder;
            this.PropertyChanged += Handle_PropertyChanged;
            this.MainViewModel = mainViewModel;
            this.MainModel = mainModel;

            ListCollectionView view = (ListCollectionView)CollectionViewSource
                    .GetDefaultView(this.CheckpointCollection);

            view.CustomSort = new SortCheckpointsByLastWriteTime();

        }

        private void Handle_PropertyChanged(object? sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(SelectedCheckpoint))
            {
                Trace.WriteLine("selected checkpoint changed");
                CheckpointModel.SelectedCheckpoint = SelectedCheckpoint;
            }

        }

        public class SortCheckpointsByLastWriteTime : IComparer
        {
            public int Compare(object x, object y)
            {
                Checkpoint cx = (Checkpoint)x;
                Checkpoint cy = (Checkpoint)y;

                if (cx.ModifiedOn == null || cy.ModifiedOn == null)
                { return 0; }

                int? diff =  (int?)(cx.ModifiedOn - cy.ModifiedOn)?.TotalSeconds;
                return diff == null ? 0 : (int)diff;
            }
        }



        private ICommand _dump;
        public ICommand Dump
        {
            get { return _dump ?? (_dump = new DumpCommand(CheckpointModel)); }
            set { _dump = value; }
        }
        public ICommand Inject { get; private set; }

        public void FolderChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            SaveFolder? saveFolder = (SaveFolder?)e.NewValue;
            Trace.WriteLine("Selected Folder Path: " + saveFolder?.SaveFolderPath);

            if (saveFolder != null)
            {
                Properties.Settings.Default.LastSelectedFolder[MainViewModel.SelectedTabIndex] = saveFolder.SaveFolderPath;
            }
            
            CheckpointModel.SelectedSaveFolder = saveFolder;
            CheckpointModel.RefreshCheckpointList();

            
        }

        public class DumpCommand : ICommand
        {
            public DumpCommand(CheckpointModel checkpointModel)
            {
                CheckpointModel = checkpointModel;
            }

            private CheckpointModel CheckpointModel { get; set; }
            public bool CanExecute(object? parameter)
            {
                return true;
            }

            public void Execute(object? parameter)
            {
                try
                {
                    CheckpointModel.TryDump();
                }
                catch (Exception ex)
                {
                    System.Windows.MessageBox.Show("Failed to dump! \n" + ex.Message, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
                }
                
            }

            public event EventHandler? CanExecuteChanged;
        }


    }




}
