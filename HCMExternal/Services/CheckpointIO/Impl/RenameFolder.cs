using HCMExternal.Models;
using Serilog;
using System;
using System.IO;

namespace HCMExternal.Services.CheckpointIO.Impl
{
    public partial class CheckpointIOService
    {
        /// <summary>
        /// Prompts the user to rename a saveFolder (this will change the actual folder name).
        /// </summary>
        /// <param name="SelectedSaveFolder">The saveFolder to rename.</param>
        /// <exception cref="Exception"></exception>
        /// <exception cref="InvalidOperationException"></exception>
        public void RenameFolder(SaveFolder SelectedSaveFolder)
        {
            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath))
            {
                Log.Error("RenameFolder: selected save folder didn't actually exist at path " + SelectedSaveFolder.SaveFolderPath);
                return;
            }

            // Ask user what they want to name the folder
            string? userInput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                                                       $"Rename folder: {SelectedSaveFolder.SaveFolderName}",
                                                       $"{SelectedSaveFolder.SaveFolderName}",
                                                       -1, -1);

            if (userInput == string.Empty)
            {
                return; //They clicked the cancel button
            }

            string proposedFolder = SelectedSaveFolder.ParentPath + "\\" + userInput;
            Log.Verbose("proposed folder: " + proposedFolder);
            // Some basic but not comprehensive checks that the user inputted a valid value (trycatch will find the rest of invalids)
            if (Directory.Exists(proposedFolder))
            {
                throw new InvalidOperationException("Failed to rename savefolder; directory by that name already exists");
            }

            Directory.Move(SelectedSaveFolder.SaveFolderPath, proposedFolder); // will throw if user chose an illegal name




        }
    }

}
