using System;

using System.IO;
using System.Diagnostics;
using HCMExternal.Models;
using Serilog;

namespace HCMExternal.Services.CheckpointServiceNS
{

    public partial class CheckpointService
    {
        /// <summary>
        /// Creates a new saveFolder within the selected parent saveFolder.
        /// </summary>
        /// <param name="SelectedSaveFolder">The parent saveFolder within which to create a new saveFolder.</param>
        /// <exception cref="Exception"></exception>
        /// <exception cref="InvalidOperationException"></exception>
        public void NewFolder(SaveFolder? SelectedSaveFolder)
        {
            if (SelectedSaveFolder == null) throw new Exception("Can't create new folder - no savefolder selected to place it under");


            // Ask user what they want to name the folder
            string? userInput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters." + "\nThis new folder will be placed within the selected folder:\n" + SelectedSaveFolder.SaveFolderPath,
                                                       $"New Folder within: {SelectedSaveFolder.SaveFolderName}",
                                                       $"New Folder",
                                                       -1, -1);

            if (userInput == null) return; //They clicked the cancel button

            string proposedFolder = SelectedSaveFolder.SaveFolderPath + "\\" + userInput;
            Log.Debug("proposed new folder: " + proposedFolder);
            // Some basic but not comprehensive checks that the user inputted a valid value (trycatch will find the rest of invalids)
            if (userInput == "" || Directory.Exists(proposedFolder)) throw new InvalidOperationException("Failed to create new savefolder; was your new name valid and unique?");


                Directory.CreateDirectory(proposedFolder);




        }
    }
   
}
