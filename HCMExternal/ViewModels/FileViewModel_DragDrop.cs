using GongSolutions.Wpf.DragDrop;
using HCMExternal.Models;
using HCMExternal.ViewModels.Interfaces;
using Serilog;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Windows;
using System.Windows.Data;


namespace HCMExternal.ViewModels
{
    public partial class FileViewModel : Presenter, IDropTarget
    {

        void IDropTarget.DragOver(IDropInfo dropInfo)
        {
            // Switch on the types of the source and target items
            switch ((dropInfo.Data?.GetType(), dropInfo.TargetItem?.GetType()))
            {

                // Trying to move a checkpoint relative to a checkpoint (reordering)
                case var value when value == (typeof(Checkpoint), typeof(Checkpoint)):
                    dropInfo.Effects = DragDropEffects.Move;
                    dropInfo.DropTargetAdorner = DropTargetAdorners.Insert;
                    break;

                //Trying to move a checkpoint into a different folder
                case var value when value == (typeof(SaveFolder), typeof(Checkpoint)):
                    dropInfo.Effects = DragDropEffects.Move;
                    dropInfo.DropTargetAdorner = DropTargetAdorners.Highlight;
                    break;

                // Trying to move a SaveFolder relative to another SaveFolder (within the same level) (reordering).
                case var value when value == (typeof(SaveFolder), typeof(SaveFolder)):
                    // Only valid if both saveFolders share a parent directory.
                    string? sourceParent = ((SaveFolder?)dropInfo.Data)?.ParentPath;
                    string? targetParent = ((SaveFolder?)dropInfo.TargetItem)?.ParentPath;

                    if (sourceParent == targetParent && sourceParent != null)
                    {
                        dropInfo.Effects = DragDropEffects.Move;  //TODO: figure out why this wigs out with treeview, unlike the highlight adorner
                        dropInfo.DropTargetAdorner = DropTargetAdorners.Insert;
                    }
                    else
                        dropInfo.Effects = DragDropEffects.None; // invalid move

                    break;

                default:
                    dropInfo.Effects = DragDropEffects.None; // invalid move
                    break;
            }

        }

        void IDropTarget.Drop(IDropInfo dropInfo)
        {

            // Switch on the types of the source and target items
            switch ((dropInfo.Data?.GetType(), dropInfo.TargetItem?.GetType()))
            {
                // Reorder two checkpoints in the checkpointlist (checkpoints in view are always ordered by their LastWriteTime (aka ModifiedOn))
                case var value when value == (typeof(Checkpoint), typeof(Checkpoint)):
                    {
                        Log.Verbose("FileViewModel: reordering two checkpoints in the checkpoint list");
                        //Grab the collection view (checkpoints in order of LWT)
                        ListCollectionView checkpointCollectionView = (ListCollectionView)CollectionViewSource
                            .GetDefaultView(CheckpointCollection);

                        // Need to cap the new index as dropInfo uses count + 1 if dragging to the bottom of list
                        int newIndex = Math.Min(dropInfo.InsertIndex, checkpointCollectionView.Count - 1);
                        int oldIndex = checkpointCollectionView.IndexOf(dropInfo.Data);

                        if (newIndex == oldIndex)
                            return; // No move (user dragged it back to itself)

                        if (oldIndex < 0)
                            return; // Shouldn't happen

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
                        _checkpointIOService.BatchModCheckpointLWTs(listCheckpoints, listLWT, SelectedSaveFolder);

                        // Refresh the list
                        UpdateCheckpointCollection();
                    }
                    break;



                // Trying to move a SaveFolder relative to another SaveFolder (within the same level) (reordering).
                case var value when value == (typeof(SaveFolder), typeof(SaveFolder)):
                    {
                        Log.Verbose("FileViewModel: moving a save folder relative to another save folder");
                        string? sourceParent = ((SaveFolder?)dropInfo.Data)?.ParentPath;
                        string? targetParent = ((SaveFolder?)dropInfo.TargetItem)?.ParentPath;

                        if (sourceParent == targetParent && sourceParent != null && dropInfo.TargetCollection.GetType() == typeof(ObservableCollection<SaveFolder>) && dropInfo.Data.GetType() == typeof(SaveFolder))
                        {

                            ObservableCollection<SaveFolder> thisLevel = dropInfo.TargetCollection as ObservableCollection<SaveFolder>;
                            int newIndex = Math.Min(dropInfo.InsertIndex, thisLevel.Count);
                            int oldIndex = thisLevel.IndexOf(dropInfo.Data as SaveFolder);

                            if (newIndex == oldIndex)
                                return; // No move (user dragged it back to itself)

                            if (oldIndex < 0)
                                return; // Shouldn't happen

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
                            _checkpointIOService.BatchModSaveFolderCreatedOns(listSaveFolders, listCreatedOn);

                            // Refresh the tree
                            UpdateSaveFolderCollection();

                        }
                    }
                    break;

                default:

                    break;
            }



        }


    }
}
