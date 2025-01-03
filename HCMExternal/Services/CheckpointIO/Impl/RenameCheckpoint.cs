using HCMExternal.Models;
using System;
using System.IO;

namespace HCMExternal.Services.CheckpointIO.Impl
{
    public partial class CheckpointIOService
    {

        /// <summary>
        /// Prompts the user to rename a checkpoint (this will change the checkpoints actual filename).
        /// </summary>
        /// <param name="SelectedSaveFolder">SaveFolder containing this checkpoint.</param>
        /// <param name="SelectedCheckpoint">The checkpoint to be renamed.</param>
        /// <exception cref="Exception"></exception>
        /// <exception cref="InvalidOperationException"></exception>
        public void RenameCheckpoint(SaveFolder SelectedSaveFolder, Checkpoint? SelectedCheckpoint)
        {
            if (SelectedCheckpoint == null)
            {
                throw new Exception("Can't rename - no checkpoint selected");
            }

            string oldsavePath = SelectedSaveFolder.SaveFolderPath + "//" + SelectedCheckpoint.CheckpointName + ".bin";

            if (!File.Exists(oldsavePath))
            {
                throw new Exception("Can't rename - original file didn't actually exist at path " + oldsavePath);
            }

            // Ask user what they want to name the checkpoint file
            string? userInput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no illegal characters",
                                                       $"Rename checkpoint: {SelectedCheckpoint.CheckpointName}",
                                                       $"{SelectedCheckpoint.CheckpointName}",
                                                       -1, -1);

            if (userInput == string.Empty)
            {
                return; //They clicked the cancel button
            }

            string proposedSave = (SelectedSaveFolder?.SaveFolderPath + $"\\{userInput}.bin");
            // Some basic but not comprehensive checks that the user inputted a valid value (trycatch will find the rest of invalids)
            if (File.Exists(proposedSave))
            {
                throw new InvalidOperationException("Failed to rename checkpoint; checkpoint by that name already exists");
            }

            File.Move(oldsavePath, proposedSave); // will throw if the filename was invalid

        }
    }

}
