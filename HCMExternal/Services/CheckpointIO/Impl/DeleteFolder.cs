using HCMExternal.Models;
using System;
using System.IO;
using System.Windows;

namespace HCMExternal.Services.CheckpointIO.Impl
{
    public partial class CheckpointIOService
    {

        /// <summary>
        /// Deletes a saveFolder containing checkpoints.
        /// </summary>
        /// <param name="SelectedSaveFolder">The saveFolder to delete.</param>
        /// <exception cref="Exception"></exception>
        public void DeleteFolder(SaveFolder SelectedSaveFolder)
        {
            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath))
            {
                throw new Exception("Can't delete - selected save folder didn't actually exist at path " + SelectedSaveFolder.SaveFolderPath);
            }

            //We want to count how many checkpoints are in this folder and sub-folders so we can warn the user that they may be about to delete many checkpoints.

            int checkpointCount = Directory.GetFiles(SelectedSaveFolder.SaveFolderPath, "*.bin", SearchOption.AllDirectories).Length;


            if (MessageBox.Show("Are you sure you want to delete the folder \"" + SelectedSaveFolder.SaveFolderName + "\", and all it's subfolders?"
                + "\n" + "This will delete " + checkpointCount + " checkpoint(s) contained within, as well as any other files."
                , "HCM - Delete folder?", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.Yes)
            {
                Directory.Delete(SelectedSaveFolder.SaveFolderPath, true);
            }






        }
    }

}
