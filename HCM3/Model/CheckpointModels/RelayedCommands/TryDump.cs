using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;
using HCM3.Model;
using BurntMemory;
using System.IO;

namespace HCM3.Model.CheckpointModels
{
    internal sealed partial class CheckpointModel
    {
        public void TryDump(SaveFolder? SelectedSaveFolder)
        {

            // Update HaloState
            MainModel.HaloMemory.HaloState.UpdateHaloState();

            // Check that we're loaded into the game that matches the tab whose checkpoint we're trying to dump
            Dictionaries.HaloStateEnum game = (Dictionaries.HaloStateEnum)this.MainModel.HaloMemory.HaloState.CurrentHaloState;

            if ((int)game != this.MainModel.SelectedTabIndex)
            {
                throw new InvalidOperationException("HCM didn't detect that you were in the right game: \n" +
                    "Expected: " + Dictionaries.TabIndexTo2LetterGameCode[this.MainModel.SelectedTabIndex] + "\n" +
                    "Actual: " + game.ToString()
                    );
            }

            // Check that the folder we're going to dump the file into actually exists
            if (!Directory.Exists(SelectedSaveFolder?.SaveFolderPath))
            {
                throw new InvalidOperationException("TryDump didn't have a valid folder to save the checkpoint to " + SelectedSaveFolder?.SaveFolderPath);
            }


            string gameAs2Letters = Dictionaries.TabIndexTo2LetterGameCode[(int)game];
            string? MCCversion = this.MainModel.CurrentAttachedMCCVersion;

            if (MCCversion == null)
            {
                throw new InvalidOperationException("HCM couldn't detect which version of MCC was running");
            }

            // Check that we have the required pointers to do a checkpoint dump
            List<string> requiredPointerNames = new();
            switch ((int)game)
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
                    throw new InvalidOperationException("TryDump was fed an invalid game, somehow. " + game.ToString());
            }

            // Load the required pointers into a dictionary
            Dictionary<string, object> requiredPointers = new();
            foreach (string requiredPointerName in requiredPointerNames)
            {
                    object? pointer = MainModel.DataPointers.GetPointer(requiredPointerName, MCCversion);
                if (pointer == null)
                {
                    throw new InvalidOperationException("HCM doesn't have offsets loaded to perform this operation with this version of MCC."
                        + $"\nSpecifically: {requiredPointerName}"
                        ); ;
                }
                requiredPointers.Add(requiredPointerName[3..], pointer); // Cut off the gamecode part so we can just refer to the rest of the name later
            }

            // Alright, time to read the data
            // Setup checkpoint data buffer to length of the checkpoint
            byte[]? CheckpointData = new byte[(int)requiredPointers["CheckpointLength"]];
            switch ((int)game)
            {
                case 0:
                    CheckpointData = this.MainModel.HaloMemory.ReadWrite.ReadData((ReadWrite.Pointer?)requiredPointers["CheckpointLocation1"], CheckpointData.Length);
                    break;

                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    byte? doubleRevertFlag = (byte?)this.MainModel.HaloMemory.ReadWrite.ReadBytes((ReadWrite.Pointer?)requiredPointers["DoubleRevertFlag"])?.GetValue(0);
                    if (doubleRevertFlag == null)
                    {
                        throw new InvalidOperationException("Failed to read double revert flag");
                    }
                    if (doubleRevertFlag == 0)
                    {
                        CheckpointData = this.MainModel.HaloMemory.ReadWrite.ReadData((ReadWrite.Pointer?)requiredPointers["CheckpointLocation1"], CheckpointData.Length);
                    }
                    else if (doubleRevertFlag == 1)
                    {
                        CheckpointData = this.MainModel.HaloMemory.ReadWrite.ReadData((ReadWrite.Pointer?)requiredPointers["CheckpointLocation2"], CheckpointData.Length);
                    }
                    else 
                    {
                        throw new InvalidOperationException("doubleRevertFlag was an invalid value (not 0 or 1)");
                    }
                    break;

                default:
                    throw new InvalidOperationException("TryDump was fed an invalid game, somehow. " + game.ToString());
            }

            if (CheckpointData == null)
            {
                throw new InvalidOperationException("Couldn't read checkpoint data from game");
            }


            // Add version string to checkpoint
            if (MainModel.CurrentAttachedMCCVersion != null && MainModel.CurrentAttachedMCCVersion.Length == 10)
            {
                byte[] versionStringChars = Encoding.ASCII.GetBytes(MainModel.CurrentAttachedMCCVersion);
                Array.Copy(versionStringChars, 0, CheckpointData, CheckpointData.Length - 10, versionStringChars.Length);
            }
                

            


            // Ask user what they want to name the checkpoint file
            string? userInput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                                                       "Name your dumped checkpoint",
                                                       "",
                                                       -1, -1);
            string proposedSave = (SelectedSaveFolder?.SaveFolderPath + $"\\{userInput}.bin");
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
