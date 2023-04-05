using HCM3.ViewModels.MVVM;
using HCM3.Models;
using System.Collections.ObjectModel;
using System.Windows.Input;
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

namespace HCM3.ViewModels
{
    public partial class CheckpointViewModel : Presenter, IDropTarget
    {

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
                this.CheckpointServices.BatchModCheckpointLWTs(listCheckpoints, listLWT, SelectedSaveFolder);

                // Refresh the list
                this.RefreshCheckpointList();
            }


            if (targetType == typeof(SaveFolder) && sourceType == typeof(SaveFolder))
            {
                string? sourceParent = ((SaveFolder?)dropInfo.Data)?.ParentPath;
                string? targetParent = ((SaveFolder?)dropInfo.TargetItem)?.ParentPath;

                Trace.WriteLine("targ col type: " + dropInfo.TargetCollection.GetType().ToString());

                if (sourceParent == targetParent && sourceParent != null && dropInfo.TargetCollection.GetType() == typeof(ObservableCollection<SaveFolder>) && dropInfo.Data.GetType() == typeof(SaveFolder))
                {

                    ObservableCollection<SaveFolder> thisLevel = dropInfo.TargetCollection as ObservableCollection<SaveFolder>;
                    int newIndex = Math.Min(dropInfo.InsertIndex, thisLevel.Count);
                    int oldIndex = thisLevel.IndexOf(dropInfo.Data as SaveFolder);

                    if (newIndex == oldIndex) return; // No move (user dragged it back to itself)

                    if (oldIndex < 0) return; // Shouldn't happen

                    //Create a list of the SaveFolders and their associated createdOns (in order) from the collectionView
                    List<SaveFolder> listSaveFolders = new();
                    List<DateTime?> listCreatedOn = new();

                    foreach (SaveFolder s in thisLevel)
                    {
                        //Note that the two lists are in sync/paired.
                        listSaveFolders.Add(s);
                        listCreatedOn.Add(s.CreatedOn);
                    }

                    // Now we *move* a SaveFolder in the list by, well changing it, but WITHOUT changing the list of createdOns's (break the pairing)
                    SaveFolder oldSaveFolder = listSaveFolders[oldIndex];
                    listSaveFolders.RemoveAt(oldIndex);
                    listSaveFolders.Insert(newIndex, oldSaveFolder);

                    // Pass the two lists to checkpointmodel to actually apply the createdOns to the folders.
                    // The new list of saveFolders gets the old list of createdOnss, thus reordering them
                    this.CheckpointServices.BatchModSaveFolderCreatedOns(listSaveFolders, listCreatedOn);

                    // Refresh the tree
                    this.RefreshSaveFolderTree();

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

    }
}
