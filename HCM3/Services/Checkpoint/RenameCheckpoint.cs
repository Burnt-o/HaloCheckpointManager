using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Diagnostics;
using HCM3.Models;

namespace HCM3.Services
{

    //public interface IService
    //{

    //}

    public partial class CheckpointServices
    {
        public void RenameCheckpoint(SaveFolder? SelectedSaveFolder, Checkpoint? SelectedCheckpoint)
        {
            if (SelectedSaveFolder == null || SelectedCheckpoint == null) throw new Exception("Can't rename - no checkpoint selected");


            // Ask user what they want to name the checkpoint file
            string? userInput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                                                       $"Rename checkpoint: {SelectedCheckpoint.CheckpointName}",
                                                       "",
                                                       -1, -1);
            string proposedSave = (SelectedSaveFolder?.SaveFolderPath + $"\\{userInput}.bin");
            // Some basic but not comprehensive checks that the user inputted a valid value (trycatch will find the rest of invalids)
            if (userInput == null || userInput == "" || File.Exists(proposedSave)) throw new InvalidOperationException("Failed to rename checkpoint; was your new name valid and unique?");



            string oldsavePath = SelectedSaveFolder.SaveFolderPath + "//" + SelectedCheckpoint.CheckpointName + ".bin";
            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath) || !File.Exists(oldsavePath)) throw new Exception("Somehow old file was invalid");


            File.Move(oldsavePath, proposedSave);

        }
    }
   
}
