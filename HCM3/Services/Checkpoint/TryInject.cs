using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;
using BurntMemory;
using System.IO;
using HCM3;
using System.Security.Cryptography;
using HCM3.Models;
using HCM3.Helpers;
using System.Diagnostics;
using System.Windows;

namespace HCM3.Services
{
    public partial class CheckpointServices
    {
        public void TryInject(SaveFolder? selectedSaveFolder, Checkpoint? selectedCheckpoint, int selectedGame)
        {
            if (selectedCheckpoint == null) throw new Exception("No checkpoint was selected!");

            this.CommonServices.CheckGameIsAligned(selectedGame);
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)selectedGame];

            // Check that the file we're going to inject actually exists
            string checkpointPath = selectedSaveFolder?.SaveFolderPath + "\\" + selectedCheckpoint?.CheckpointName + ".bin";
            if (!File.Exists(checkpointPath))
            {
                throw new Exception("The file HCM is trying to inject doesn't exist ?! " + selectedSaveFolder?.SaveFolderPath);
            }


            // Special case for H1 Core Saves
            if (gameAs2Letters == "H1" && Properties.Settings.Default.H1Cores)
            {
                InjectH1Core(checkpointPath);
                return;
            }





            // Load the required pointers to do a checkpoint inject
            Dictionary<string, bool> injectRequirements = (Dictionary<string, bool>)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_InjectRequirements");

            List<string> requiredPointerNames = new();
            foreach (KeyValuePair<string, bool> kvp in injectRequirements)
            {
                if (kvp.Value)
                { 
                requiredPointerNames.Add($"{gameAs2Letters}_" + kvp.Key);
                }
            }
          
            // Load the required pointers into a dictionary
            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

            FileInfo checkpointInfo = new FileInfo(checkpointPath);
            byte[]? checkpointData;
            // Next let's read the checkpoint data from the file
            using (FileStream readStream = new FileStream(checkpointPath, FileMode.Open))
            {
                using (BinaryReader readBinary = new BinaryReader(readStream))
                {
                    checkpointData = readBinary.ReadBytes((int)checkpointInfo.Length);
                }
            }

            // Check that it was read properly
            if (checkpointData == null || (checkpointData.Length != checkpointInfo.Length))
            {
                throw new Exception("HCM failed to read data of checkpoint to inject");
            }

            //Warn user if injecting checkpoint into wrong in-game level
            if (Properties.Settings.Default.WarnInjectWrongLevel)
            {
                string? levelString = null;
                try
                {
                    byte[] levelBytes = new byte[64];
                    int levelNameOffset = (int)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_CheckpointData_LevelCode");

                Array.Copy(checkpointData, levelNameOffset, levelBytes, 0, 64);

                    levelString = ASCIIEncoding.ASCII.GetString(levelBytes);
                    
                    levelString = levelString.Substring(0, this.HaloMemoryService.HaloState.CurrentLevelCode.Length);
                    Trace.WriteLine("length of currentlevelcode: " + this.HaloMemoryService.HaloState.CurrentLevelCode.Length + ", length of new substring: " + levelString.Length);
                    //while (levelString.Contains(@"\"))
                    //{ levelString = levelString.Substring(levelString.LastIndexOf(@"\") + 1); }

                }
                catch (Exception ex)
                {
                    levelString = null;
                    Trace.WriteLine("WarnInjectWrongLevel failed: " + ex.ToString());
                }


                if (levelString != null && this.HaloMemoryService.HaloState.CurrentLevelCode != null && levelString.Substring(0,3) != this.HaloMemoryService.HaloState.CurrentLevelCode.Substring(0, 3))
                {
                    string message = "It appears the checkpoint you're injecting is not on the same level as the current in-game level. This is very likely to cause the game to crash. Proceed anyway?\nCheckpoint level: " + levelString + "\nCurrent level: " + this.HaloMemoryService.HaloState.CurrentLevelCode;
                    Trace.WriteLine(message);
                    Trace.WriteLine("current level ?: " + this.HaloMemoryService.HaloState.CurrentLevelCode);

                    if (MessageBox.Show(message, "HCM Warning", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.No)
                    {
                        return;
                    }
                }

            }

            //Warn user if injecting checkpoint into wrong version of MCC
            if (Properties.Settings.Default.WarnInjectWrongVersion)
            {
                byte[] versionBytes = new byte[10];
                Array.Copy(checkpointData, checkpointData.Length - 10, versionBytes, 0, 10);
                string? versionString = null;
                try
                {
                    versionString = ASCIIEncoding.ASCII.GetString(versionBytes);
                }
                catch { }
                

                if (versionString != null && versionString != this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion)
                {
                    if (MessageBox.Show("It appears the checkpoint you're injecting may not match the version of MCC that is currently running. Proceed anyway?", "HCM Warning", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.No)
                    {
                        return;
                    }
                }

            }

            // Modify the checkpointData to remove the version string at end of file
            Array.Fill(checkpointData, (byte)0, checkpointData.Length - 10, 10);

            // Let's get the pointer to the inGameCheckpoint that we're going to overwrite
            ReadWrite.Pointer inGameCheckpointLocation;
            byte? doubleRevertFlag = null;

            if (injectRequirements["DoubleRevertFlag"])
            {
                doubleRevertFlag = (byte?)this.HaloMemoryService.ReadWrite.ReadBytes((ReadWrite.Pointer)requiredPointers["DoubleRevertFlag"])?.GetValue(0);
                if (doubleRevertFlag == null)
                {
                    throw new Exception("Failed to read double revert flag");
                }
                if (doubleRevertFlag == 0)
                {
                    inGameCheckpointLocation = (ReadWrite.Pointer)requiredPointers["CheckpointLocation1"];
                }
                else if (doubleRevertFlag == 1)
                {
                    inGameCheckpointLocation = (ReadWrite.Pointer)requiredPointers["CheckpointLocation2"];
                }
                else
                {
                    throw new Exception("doubleRevertFlag was an invalid value (not 0 or 1)");
                }
            }
            else
            {
                inGameCheckpointLocation = (ReadWrite.Pointer)requiredPointers["CheckpointLocation1"];
            }





            // Modify the checkpointData to implement "Preserve Locations". These are sections of the checkpoint data where we want 
            // to preserve the in-game values instead of overwriting it with those of the checkpoint. This is necessary to fix issues with
            // sharing checkpoints between players, for some games anyway. 
            // Easiest way to implement this is to just read the data from the game and overwrite the checkpointData at those locations.

            // First, get the PreserveLocations for this game. If null, then don't bother fixing preserve locations for this game.
            if (injectRequirements["CheckpointData_PreserveLocations"])
            {
                PreserveLocation[] preserveLocations = (PreserveLocation[])requiredPointers["CheckpointData_PreserveLocations"];
                // Now loop over each one, read the data from the game, and overwrite that part of checkpointData
                foreach (PreserveLocation preserveLocation in preserveLocations)
                {
                    if (preserveLocation != null && preserveLocation.Length != 0 && !(preserveLocation.Offset + preserveLocation.Length > checkpointData.Length))
                    {
                        // Read from game
                        byte[]? preservedGameData = this.HaloMemoryService.ReadWrite.ReadBytes(inGameCheckpointLocation + preserveLocation.Offset, preserveLocation.Length);
                        if (preservedGameData == null) throw new Exception("couldn't read PreserveLocations");
                        // Overwrite that part of checkpointData
                        preservedGameData.CopyTo(checkpointData, preserveLocation.Offset);
                    }
                }
            }


            // Next, for some games we have to fix the SHA checksum of the checkpoint
            // Currently just h3, ODST, and halo reach
            if (injectRequirements["CheckpointData_SHAoffset"] && injectRequirements["CheckpointData_SHAlength"])
            {
                // Get offset of SHA checksum relative to checkpoint file start
                int shaOffset = (int)requiredPointers["CheckpointData_SHAoffset"];
                int shaLength = (int)requiredPointers["CheckpointData_SHAlength"];
                // If offsets are null then don't bother for this game

                    // Zero out the hash at the offset
                    byte[] zeroes = new byte[(int)shaLength];
                    zeroes.CopyTo(checkpointData, (int)shaOffset);

                    // Calculate the checksum
                    //byte[] newHash = new byte[(int)shaLength];
                    using (SHA1 cryptoProvider = SHA1.Create())
                    {
                    byte[] newHash = cryptoProvider.ComputeHash(checkpointData);

                    Trace.WriteLine("newHash: \n");
                    foreach (byte b in newHash)
                    {
                        Trace.Write(b.ToString("X2"));
                    }

                    // Write the new hash 
                    newHash.CopyTo(checkpointData, (int)shaOffset);
                }


            }

            // Now, time to finally inject the checkpoint
            Trace.WriteLine("Length of injected checkpoint: " + checkpointData.Length.ToString("X"));
            Trace.WriteLine("Injecting it to: " + this.HaloMemoryService.ReadWrite.ResolvePointer(inGameCheckpointLocation)?.ToString("X"));
            bool success = this.HaloMemoryService.ReadWrite.WriteBytes(inGameCheckpointLocation, checkpointData, false);

            if (!success) throw new Exception("Failed to inject the checkpoint into game memory");

            // Okay, checkpoint should be injected. But we still need to fix the in-game memories cached BSPs to match those of our checkpoint
            // This is only appliciable to some games. Currently just h2, h3, and ODST.
            if (injectRequirements["LoadedBSP1"] && injectRequirements["LoadedBSP2"] && injectRequirements["CheckpointData_LoadedBSPoffset"] && injectRequirements["CheckpointData_LoadedBSPlength"] && injectRequirements["DoubleRevertFlag"])
            {
                ReadWrite.Pointer pointerLoadedBSP1 = (ReadWrite.Pointer)requiredPointers["LoadedBSP1"];
                ReadWrite.Pointer pointerLoadedBSP2 = (ReadWrite.Pointer)requiredPointers["LoadedBSP2"];
                int loadedBSPoffset = (int)requiredPointers["CheckpointData_LoadedBSPoffset"];
                int loadedBSPlength = (int)requiredPointers["CheckpointData_LoadedBSPlength"];
                // Skip this step if pointers are null

                    if (doubleRevertFlag == null) throw new Exception("doubleRevertFlag was null when trying to fix LoadedBSPs");

                    // Load the pointer to the in-game memory that stores information about cached BSPs
                    ReadWrite.Pointer? cachedBSPPointer = null;
                    if (doubleRevertFlag == 0)
                    {
                        cachedBSPPointer = pointerLoadedBSP1;
                    }
                    else if (doubleRevertFlag == 1)
                    {
                        cachedBSPPointer = pointerLoadedBSP2;
                    }

                    if (cachedBSPPointer == null) throw new Exception("couldn't get pointer to LoadedBSP, DR was: " + doubleRevertFlag.ToString());

                    // Now we need to get the correct BSP cache data from checkpointData
                    byte[] loadedBSPData = new byte[(int)loadedBSPlength];
                    // Copy the data from checkpointData
                    Array.Copy(checkpointData, (int)loadedBSPoffset, loadedBSPData, 0, (int)loadedBSPlength);

                    // Now set the in-game-memory cached BSPs to those listed in checkpointData
                    bool success2 = this.HaloMemoryService.ReadWrite.WriteBytes(cachedBSPPointer, loadedBSPData, false);

                    if (!success2) throw new Exception("Failed to write cachedBSP data to game memory");
                
            }

            // Wew, we're done! Let the user know
            string messageToPrint = "Checkpoint injected. ";
            if (!Properties.Settings.Default.DisableOverlay)
            {
                if (!this.HaloMemoryService.HaloState.OverlayHooked) throw new Exception("Overlay wasn't hooked");
                if (!this.InternalServices.PrintTemporaryMessageInternal(messageToPrint)) throw new Exception("Error printing message");
            }
            else
            {
                //throw new Exception ("Overlay needs to be enabled for this cheat to work")
                if (!this.CommonServices.PrintMessage(messageToPrint))
                {
                    throw new Exception("Failed to print in-game message");
                }
            }


        }

        private void InjectH1Core(string checkpointPath)
        {
            FileInfo checkpointInfo = new FileInfo(checkpointPath);
            byte[]? checkpointData;
            // Next let's read the checkpoint data from the file
            using (FileStream readStream = new FileStream(checkpointPath, FileMode.Open))
            {
                using (BinaryReader readBinary = new BinaryReader(readStream))
                {
                    checkpointData = readBinary.ReadBytes((int)checkpointInfo.Length);
                }
            }

            // Check that it was read properly
            if (checkpointData == null || (checkpointData.Length != checkpointInfo.Length))
            {
                throw new Exception("HCM failed to read data of checkpoint to inject");
            }

            // Modify the checkpointData to remove the version string at end of file
            Array.Fill(checkpointData, (byte)0, checkpointData.Length - 10, 10);

            string coreBinPath = GetCoreBinPath();

            //write checkpoint data to corebinpath
            File.Delete(coreBinPath);
            File.WriteAllBytes(coreBinPath, checkpointData);
            FileInfo test = new FileInfo(coreBinPath);
            if (!File.Exists(test.ToString()) || test.Length < 1000)
            {
                throw new InvalidOperationException("Failed to save coresave data to file, not sure why. Check local file read/write permissions?");
            }

        }

    }
}
