using System;
using System.IO;
using HCMExternal.Models;

namespace HCMExternal.Services.CheckpointServiceNS
{


    public partial class CheckpointService
    {

        /// <summary>
        /// Prompts the user to rename a checkpoint (this will change the checkpoints actual filename).
        /// </summary>
        /// <param name="SelectedSaveFolder">SaveFolder containing this checkpoint.</param>
        /// <param name="SelectedCheckpoint">The checkpoint to be renamed.</param>
        /// <exception cref="Exception"></exception>
        /// <exception cref="InvalidOperationException"></exception>
        public void RenameCheckpoint(SaveFolder? SelectedSaveFolder, Checkpoint? SelectedCheckpoint)
        {
            if (SelectedSaveFolder == null || SelectedCheckpoint == null) throw new Exception("Can't rename - no checkpoint selected");


            // Ask user what they want to name the checkpoint file
            string? userInput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                                                       $"Rename checkpoint: {SelectedCheckpoint.CheckpointName}",
                                                       $"{SelectedCheckpoint.CheckpointName}",
                                                       -1, -1);

            if (userInput == null) return; //They clicked the cancel button

            string proposedSave = (SelectedSaveFolder?.SaveFolderPath + $"\\{userInput}.bin");
            // Some basic but not comprehensive checks that the user inputted a valid value (trycatch will find the rest of invalids)
            if (userInput == "" || File.Exists(proposedSave)) throw new InvalidOperationException("Failed to rename checkpoint; was your new name valid and unique?");



            string oldsavePath = SelectedSaveFolder.SaveFolderPath + "//" + SelectedCheckpoint.CheckpointName + ".bin";
            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath) || !File.Exists(oldsavePath)) throw new Exception("Somehow old file was invalid");


            File.Move(oldsavePath, proposedSave);

        }
    }
   
}
