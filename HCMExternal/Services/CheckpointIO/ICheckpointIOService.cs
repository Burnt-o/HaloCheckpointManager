using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using HCMExternal.Models;
using HCMExternal.CheckpointSorting;

namespace HCMExternal.Services.CheckpointIO
{
    /// <summary>
    /// Manages checkpoints in the checkpoint list, ie operations like deleting/renaming checkpoints and folders, sorting, 
    /// and decoding the checkpoint files.
    /// </summary>
    public interface ICheckpointIOService
    {
        /// <summary>
        /// Rewrites the lastWriteTime of a list of checkpoints according to a specified order. LastWriteTimes are used to order the checkpoint list.
        /// </summary>
        /// <param name="listCheckpoints">List (in order) of checkpoint files whose lastWriteTimes will be modified.</param>
        /// <param name="listLWT">List (in order) of the new lastWriteTimes.</param>
        /// <param name="selectedSaveFolder">The location of the checkpoint files.</param>
        public void BatchModCheckpointLWTs(List<Checkpoint> listCheckpoints, List<DateTime?> listLWT, SaveFolder selectedSaveFolder);

        /// <summary>
        /// Rewrites the createdOnTimes dates of a list of save folders according to a specified order. CreatedOnTimes are used to order saveFolders with a shared parent.
        /// </summary>
        /// <param name="listSaveFolders">List (in order) of saveFolders (with a shared parent) whose createdOnTimes will be modified.</param>
        /// <param name="listCreatedOns">List (in order) of the new createdOnTimes.</param>
        public void BatchModSaveFolderCreatedOns(List<SaveFolder> listSaveFolders, List<DateTime?> listCreatedOns);

        /// <summary>
        /// Decodes a checkpoint file into a Checkpoint (model), reading off useful data like the level, difficulty, gametime, etc.
        /// </summary>
        /// <param name="checkpointFile">The checkpoint file.</param>
        /// <param name="selectedGame">Which game is this checkpoint from?</param>
        /// <returns></returns>
        public Checkpoint? DecodeCheckpointFile(FileInfo checkpointFile, HaloGame selectedGame);

        /// <summary>
        /// Deletes a checkpoint.
        /// </summary>
        /// <param name="SelectedSaveFolder">The folder containing the checkpoint to delete.</param>
        /// <param name="selectedCheckpoint">The checkpoint to delete.</param>
        public void DeleteCheckpoint(SaveFolder SelectedSaveFolder, Checkpoint? selectedCheckpoint);

        /// <summary>
        /// Creates a new saveFolder within the selected parent saveFolder.
        /// </summary>
        /// <param name="selectedSaveFolder">The parent saveFolder within which to create a new saveFolder.</param>
        /// <exception cref="Exception"></exception>
        /// <exception cref="InvalidOperationException"></exception>
        public void NewFolder(SaveFolder selectedSaveFolder);

        /// <summary>
        /// Opens a saveFolder in windows explorer.
        /// </summary>
        /// <param name="selectedSaveFolder">The saveFolder to view.</param>
        public void OpenInExplorer(SaveFolder selectedSaveFolder);

        /// <summary>
        /// Iterates through all the checkpoint files in a saveFolder and decodes them, creating the checkpointCollection observed by CheckpointViewModel.
        /// </summary>
        /// <param name="selectedSaveFolder">The saveFolder containing the checkpoints to parse.</param>
        /// <param name="selectedGame">Which game are these checkpoints from?</param>
        /// <returns></returns>
        public ObservableCollection<Checkpoint> PopulateCheckpointList(SaveFolder selectedSaveFolder, HaloGame selectedGame);

        /// <summary>
        /// Populates the saveFolder tree for a specified game -that's the root Folder and all folders underneath- into the saveFolderCollection observed by CheckpointViewModel.
        /// </summary>
        /// <param name="rootFolder">Out parameter: the root folder for this game.</param>
        /// <param name="selectedGame">Which game is this for?</param>
        /// <returns></returns>
        public ObservableCollection<SaveFolder> PopulateSaveFolderTree(out SaveFolder rootFolder, HaloGame selectedGame);

        /// <summary>
        /// Prompts the user to rename a checkpoint (this will change the checkpoints actual filename).
        /// </summary>
        /// <param name="selectedSaveFolder">SaveFolder containing this checkpoint.</param>
        /// <param name="selectedCheckpoint">The checkpoint to be renamed.</param>
        /// <exception cref="Exception"></exception>
        /// <exception cref="InvalidOperationException"></exception>
        public void RenameCheckpoint(SaveFolder selectedSaveFolder, Checkpoint? selectedCheckpoint);

        /// <summary>
        /// Prompts the user to rename a saveFolder (this will change the actual folder name).
        /// </summary>
        /// <param name="SelectedSaveFolder">The saveFolder to rename.</param>
        /// <exception cref="Exception"></exception>
        /// <exception cref="InvalidOperationException"></exception>
        public void RenameFolder(SaveFolder SelectedSaveFolder);

        /// <summary>
        /// Deletes a saveFolder containing checkpoints.
        /// </summary>
        /// <param name="SelectedSaveFolder">The saveFolder to delete.</param>
        /// <exception cref="Exception"></exception>
        public void DeleteFolder(SaveFolder SelectedSaveFolder);

        /// <summary>
        /// Prompts the user to manually enter the MCC-game-version-string for a selected checkpoint.
        /// </summary>
        /// <param name="selectedSaveFolder">SaveFolder containing the selected checkpoint.</param>
        /// <param name="selectedCheckpoint">The checkpoint whose version string will be manually entered.</param>
        /// <exception cref="Exception"></exception>
        /// <exception cref="InvalidOperationException"></exception>
        public void ReVersionCheckpoint(SaveFolder selectedSaveFolder, Checkpoint? selectedCheckpoint);

        /// <summary>
        /// Sorts all the checkpoints in a folder, according to a list (in order) of ICompareCheckpoints (the user sets these).
        /// </summary>
        /// <param name="selectedSaveFolder">SaveFolder containing the checkpoints to be sorted.</param>
        /// <param name="checkpointCollection">CheckpointCollection containing the checkopints.</param>
        /// <param name="comparers">List of comparerers passed from user selection.</param>
        /// <param name="selectedGame">Which game are we sorting? Needed to implement level sorting.</param>
        /// <exception cref="Exception"></exception>
        public void SortCheckpoints(SaveFolder selectedSaveFolder, ObservableCollection<Checkpoint> checkpointCollection, List<Tuple<ICompareCheckpoints, bool>> comparers, HaloGame selectedGame);


    }
}
