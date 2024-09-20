using HCMExternal.Models;
using Serilog;
using System;
using System.IO;
using System.Text;

namespace HCMExternal.Services.CheckpointServiceNS
{

    public partial class CheckpointService
    {
        /// <summary>
        /// Prompts the user to manually enter the MCC-game-version-string for a selected checkpoint.
        /// </summary>
        /// <param name="SelectedSaveFolder">SaveFolder containing the selected checkpoint.</param>
        /// <param name="SelectedCheckpoint">The checkpoint whose version string will be manually entered.</param>
        /// <exception cref="Exception"></exception>
        /// <exception cref="InvalidOperationException"></exception>
        public void ReVersionCheckpoint(SaveFolder SelectedSaveFolder, Checkpoint? SelectedCheckpoint)
        {
            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath))
            {
                Log.Error("ReVersionCheckpoint: selected save folder didn't actually exist at path " + SelectedSaveFolder.SaveFolderPath);
                return;
            }

            if (SelectedCheckpoint == null)
            {
                throw new Exception("Can't edit checkpoint game version - no checkpoint selected");
            }

            string oldversion = SelectedCheckpoint.GameVersion != null ? SelectedCheckpoint.GameVersion.ToString() : string.Empty;

            // Prompt for input
            string? userInput = Microsoft.VisualBasic.Interaction.InputBox("Must be 10 chars long, in the format \"1.2645.0.0\"",
                                                       $"Edit checkpoint version: {SelectedCheckpoint.CheckpointName}",
                                                       $"{oldversion}",
                                                       -1, -1);

            if (userInput == "")
            {
                return; //They clicked the cancel button
            }

            // Some basic but not comprehensive checks that the user inputted a valid value (trycatch will find the rest of invalids)
            if (userInput == null || userInput.Length != 10 || !userInput.StartsWith("1."))
            {
                throw new InvalidOperationException("Failed to edit checkpoint version; was your new version string 10 chars long and in the corect format?");
            }

            string savePath = SelectedSaveFolder.SaveFolderPath + "//" + SelectedCheckpoint.CheckpointName + ".bin";
            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath) || !File.Exists(savePath))
            {
                throw new Exception("Somehow the file was invalid, so version string couldn't be edited");
            }

            //read checkpoint data from file
            byte[] checkpointData = File.ReadAllBytes(savePath);

            //get last-write-time for re-applying later
            DateTime? lwt = SelectedCheckpoint.ModifiedOn;

            //convert version string to bytes and put it in at the end of the data
            byte[] versionStringChars = Encoding.ASCII.GetBytes(userInput);
            Array.Copy(versionStringChars, 0, checkpointData, checkpointData.Length - 10, versionStringChars.Length);

            //write checkpoint data back to file
            File.WriteAllBytes(savePath, checkpointData);

            //re-apply last-write-time
            if (lwt != null)
            {
                FileInfo fileInfo = new FileInfo(savePath)
                {
                    LastWriteTime = lwt.Value
                };
            }

        }
    }

}
