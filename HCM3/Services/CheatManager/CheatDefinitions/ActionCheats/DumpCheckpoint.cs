using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using BurntMemory;
using HCM3.Models;
using HCM3.Helpers;
using HCM3.ViewModels;

namespace HCM3.Services.Cheats
{
    internal class DumpCheckpoint : AbstractActionCheat
    {

        // Reads the current checkpoint data from the game and dumps it to a file on disk
        public DumpCheckpoint(ServicesGroup servicesGroup) : base(servicesGroup)
        {
 
        }

        public CheckpointViewModel CheckpointViewModel { get; set; }



        protected override void ApplyCheat(HaloState haloState)
        {
            SaveFolder? selectedSaveFolder = this.CheckpointViewModel.SelectedSaveFolder;

            // Check that the folder we're going to dump the file into actually exists
            if (!Directory.Exists(selectedSaveFolder?.SaveFolderPath))
            {
                throw new InvalidOperationException("TryDump didn't have a valid folder to save the checkpoint to " + selectedSaveFolder?.SaveFolderPath);
            }


            byte[]? CheckpointData;
            // Special case for H1 Core Saves
            if (haloState.GameState == GameStateEnum.Halo1 && Properties.Settings.Default.H1Cores)
            {
                string coreBinPath = this.HaloMemoryService.GetCoreBinPath();

                FileInfo checkpointInfo = new FileInfo(coreBinPath);
                // Next let's read the checkpoint data from the file
                using (FileStream readStream = new FileStream(coreBinPath, FileMode.Open))
                {
                    using (BinaryReader readBinary = new BinaryReader(readStream))
                    {
                        CheckpointData = readBinary.ReadBytes((int)checkpointInfo.Length);
                    }
                }
            }
            else
            {
                // Load the required pointers to do a checkpoint dump
                List<string> requiredPointerNames = new();
                switch (haloState.GameState)
                {
                    case GameStateEnum.Halo1:
                        requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_CheckpointLocation1");
                        requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_CheckpointLength");
                        break;

                    case GameStateEnum.Halo2:
                    case GameStateEnum.Halo3:
                    case GameStateEnum.Halo3ODST:
                    case GameStateEnum.HaloReach:
                    case GameStateEnum.Halo4:
                        requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_CheckpointLocation1");
                        requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_CheckpointLocation2");
                        requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_CheckpointLength");
                        requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_DoubleRevertFlag");
                        break;

                    default:
                        throw new InvalidOperationException("TryDump was fed an invalid game, somehow. " + haloState.GameState.ToString());
                }

                // Load the required pointers into a dictionary
                Dictionary<string, object> requiredPointers = this.DataPointersService.GetRequiredPointers(requiredPointerNames, haloState);

                // Alright, time to read the data
                // Setup checkpoint data buffer to length of the checkpoint
                CheckpointData = new byte[(int)requiredPointers["CheckpointLength"]];
                switch (haloState.GameState)
                {
                    case GameStateEnum.Halo1:
                        CheckpointData = this.HaloMemoryService.ReadWrite.ReadBytes((ReadWrite.Pointer?)requiredPointers["CheckpointLocation1"], CheckpointData.Length);
                        break;

                    case GameStateEnum.Halo2:
                    case GameStateEnum.Halo3:
                    case GameStateEnum.Halo3ODST:
                    case GameStateEnum.HaloReach:
                    case GameStateEnum.Halo4:
                        byte? doubleRevertFlag = (byte?)this.HaloMemoryService.ReadWrite.ReadBytes((ReadWrite.Pointer?)requiredPointers["DoubleRevertFlag"])?.GetValue(0);
                        if (doubleRevertFlag == null)
                        {
                            throw new InvalidOperationException("Failed to read double revert flag");
                        }
                        if (doubleRevertFlag == 0)
                        {
                            CheckpointData = this.HaloMemoryService.ReadWrite.ReadBytes((ReadWrite.Pointer?)requiredPointers["CheckpointLocation1"], CheckpointData.Length);
                        }
                        else if (doubleRevertFlag == 1)
                        {
                            CheckpointData = this.HaloMemoryService.ReadWrite.ReadBytes((ReadWrite.Pointer?)requiredPointers["CheckpointLocation2"], CheckpointData.Length);
                        }
                        else
                        {
                            throw new InvalidOperationException("doubleRevertFlag was an invalid value (not 0 or 1)");
                        }
                        break;

                    default:
                        throw new InvalidOperationException("TryDump was fed an invalid game, somehow. " + haloState.GameState.ToString());
                }
            }





            if (CheckpointData == null)
            {
                throw new InvalidOperationException("Couldn't read checkpoint data from game");
            }


            // Add version string to checkpoint
            if (haloState.MCCVersion != null && haloState.MCCVersion.Length == 10)
            {
                byte[] versionStringChars = Encoding.ASCII.GetBytes(haloState.MCCVersion);
                Array.Copy(versionStringChars, 0, CheckpointData, CheckpointData.Length - 10, versionStringChars.Length);
            }



            string? userInput;
            if (Properties.Settings.Default.AutoName)
            {
                userInput = haloState.GameState.To2Letters() + "_" + DateTime.Now.ToString("yy-MM-dd HHmmss");

                int count = 2;
                while (File.Exists(userInput))
                {
                    userInput = userInput + "(" + count.ToString() + ")";
                    count++;
                }

            }
            else
            {
                // Ask user what they want to name the checkpoint file
                userInput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                                                           "Name your dumped checkpoint",
                                                           "",
                                                           -1, -1);
            }
            string proposedSave = (selectedSaveFolder?.SaveFolderPath + $"\\{userInput}.bin");
            // Some basic but not comprehensive checks that the user inputted a valid value (trycatch will find the rest of invalids)
            if (userInput != null && userInput != "" && !File.Exists(proposedSave))
            {
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
                    throw new InvalidOperationException("Failed to save checkpoint data to file; was your name valid and unique? \nError: " + ex.ToString());
                }
            }

        }

    }
}
