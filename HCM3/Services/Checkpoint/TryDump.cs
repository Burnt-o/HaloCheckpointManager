using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;
using BurntMemory;
using System.IO;
using HCM3.Models;
using HCM3.Helpers;


namespace HCM3.Services
{
    public partial class CheckpointServices
    {
        public void TryDump(SaveFolder? selectedSaveFolder, int selectedGame)
        {

            this.CommonServices.IsGameCorrect(selectedGame);
            string gameAs2Letters = Dictionaries.TabIndexTo2LetterGameCode[(int)selectedGame];

            // Check that the folder we're going to dump the file into actually exists
            if (!Directory.Exists(selectedSaveFolder?.SaveFolderPath))
            {
                throw new InvalidOperationException("TryDump didn't have a valid folder to save the checkpoint to " + selectedSaveFolder?.SaveFolderPath);
            }


            // Load the required pointers to do a checkpoint dump
            List<string> requiredPointerNames = new();
            switch ((int)selectedGame)
            {
                case 0:
                    requiredPointerNames.Add($"{gameAs2Letters}_CheckpointLocation1");
                    requiredPointerNames.Add($"{gameAs2Letters}_CheckpointLength");
                    break;

                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    requiredPointerNames.Add($"{gameAs2Letters}_CheckpointLocation1");
                    requiredPointerNames.Add($"{gameAs2Letters}_CheckpointLocation2");
                    requiredPointerNames.Add($"{gameAs2Letters}_CheckpointLength");
                    requiredPointerNames.Add($"{gameAs2Letters}_DoubleRevertFlag");
                    break;

                default:
                    throw new InvalidOperationException("TryDump was fed an invalid game, somehow. " + selectedGame.ToString());
            }

            // Load the required pointers into a dictionary
            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

            // Alright, time to read the data
            // Setup checkpoint data buffer to length of the checkpoint
            byte[]? CheckpointData = new byte[(int)requiredPointers["CheckpointLength"]];
            switch ((int)selectedGame)
            {
                case 0:
                    CheckpointData = this.HaloMemoryService.ReadWrite.ReadData((ReadWrite.Pointer?)requiredPointers["CheckpointLocation1"], CheckpointData.Length);
                    break;

                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    byte? doubleRevertFlag = (byte?)this.HaloMemoryService.ReadWrite.ReadBytes((ReadWrite.Pointer?)requiredPointers["DoubleRevertFlag"])?.GetValue(0);
                    if (doubleRevertFlag == null)
                    {
                        throw new InvalidOperationException("Failed to read double revert flag");
                    }
                    if (doubleRevertFlag == 0)
                    {
                        CheckpointData = this.HaloMemoryService.ReadWrite.ReadData((ReadWrite.Pointer?)requiredPointers["CheckpointLocation1"], CheckpointData.Length);
                    }
                    else if (doubleRevertFlag == 1)
                    {
                        CheckpointData = this.HaloMemoryService.ReadWrite.ReadData((ReadWrite.Pointer?)requiredPointers["CheckpointLocation2"], CheckpointData.Length);
                    }
                    else 
                    {
                        throw new InvalidOperationException("doubleRevertFlag was an invalid value (not 0 or 1)");
                    }
                    break;

                default:
                    throw new InvalidOperationException("TryDump was fed an invalid game, somehow. " + selectedGame.ToString());
            }

            if (CheckpointData == null)
            {
                throw new InvalidOperationException("Couldn't read checkpoint data from game");
            }


            // Add version string to checkpoint
            if (this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion != null && this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion.Length == 10)
            {
                byte[] versionStringChars = Encoding.ASCII.GetBytes(this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion);
                Array.Copy(versionStringChars, 0, CheckpointData, CheckpointData.Length - 10, versionStringChars.Length);
            }
                

            


            // Ask user what they want to name the checkpoint file
            string? userInput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                                                       "Name your dumped checkpoint",
                                                       "",
                                                       -1, -1);
            string proposedSave = (selectedSaveFolder?.SaveFolderPath + $"\\{userInput}.bin");
            // Some basic but not comprehensive checks that the user inputted a valid value (trycatch will find the rest of invalids)
            if (userInput != null && userInput != "" && !File.Exists(proposedSave))
                try
                {
                    // Now dump the data to a file in the saveFolder location
                    File.WriteAllBytes(proposedSave, CheckpointData);
                    FileInfo test = new FileInfo(proposedSave);
                    if (!File.Exists(test.ToString()) || test.Length < 1000)
                    {
                        throw new InvalidOperationException("Failed to save checkpoint data to file, not sure why. Check local file read/write permissions?");
                    }


                }
                catch (Exception ex)
                {
                    throw new InvalidOperationException("Failed to save checkpoint data to file; was your name valid and unique? \nError: " + ex.Message);
                }

        }

    }


}
