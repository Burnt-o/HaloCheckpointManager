using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;
using HCM3.Model;
using BurntMemory;
using System.IO;
using HCM3;
using System.Security.Cryptography;
using HCM3.Model.CheckpointModels;

namespace HCM3.Services
{
    public partial class CheckpointServices
    {
        public void TryInject(SaveFolder? selectedSaveFolder, Checkpoint? selectedCheckpoint, int selectedGame)
        {
            if (selectedCheckpoint == null) throw new Exception("No checkpoint was selected!");

            // Update HaloState
            this.HaloMemoryService.HaloState.UpdateHaloState();

            // Check that we're loaded into the game that matches the tab whose checkpoint we're trying to dump
            Dictionaries.HaloStateEnum game = (Dictionaries.HaloStateEnum)this.HaloMemoryService.HaloState.CurrentHaloState;

            if ((int)game != selectedGame)
            {
                throw new Exception("HCM didn't detect that you were in the right game: \n" +
                    "Expected: " + Dictionaries.TabIndexTo2LetterGameCode[selectedGame] + "\n" +
                    "Actual: " + game.ToString()
                    );
            }

            // Check that the file we're going to inject actually exists
            string checkpointPath = selectedSaveFolder?.SaveFolderPath + "\\" + selectedCheckpoint?.CheckpointName + ".bin";
            if (!File.Exists(checkpointPath))
            {
                throw new Exception("TryDump didn't have a valid folder to save the checkpoint to " + selectedSaveFolder?.SaveFolderPath);
            }


            string gameAs2Letters = Dictionaries.TabIndexTo2LetterGameCode[(int)game];
            string? MCCversion = this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion;

            if (MCCversion == null)
            {
                throw new Exception("HCM couldn't detect which version of MCC was running");
            }

            // Check that we have the required pointers to do a checkpoint inject
            List<string> requiredPointerNames = new();

                    requiredPointerNames.Add($"{gameAs2Letters}_CheckpointLocation1");
                    requiredPointerNames.Add($"{gameAs2Letters}_CheckpointLocation2");
                    requiredPointerNames.Add($"{gameAs2Letters}_CheckpointLength");
                    requiredPointerNames.Add($"{gameAs2Letters}_DoubleRevertFlag");
                    requiredPointerNames.Add($"{gameAs2Letters}_CheckpointData_PreserveLocations");
            requiredPointerNames.Add($"{gameAs2Letters}_CheckpointData_SHAoffset");
            requiredPointerNames.Add($"{gameAs2Letters}_CheckpointData_SHAlength");
            requiredPointerNames.Add($"{gameAs2Letters}_CheckpointData_LoadedBSPoffset");
            requiredPointerNames.Add($"{gameAs2Letters}_CheckpointData_LoadedBSPlength");
            requiredPointerNames.Add($"{gameAs2Letters}_LoadedBSP1");
            requiredPointerNames.Add($"{gameAs2Letters}_LoadedBSP2");


            // Load the required pointers into a dictionary
            Dictionary<string, object> requiredPointers = new();
            foreach (string requiredPointerName in requiredPointerNames)
            {
                object? pointer = this.DataPointersService.GetPointer(requiredPointerName, MCCversion);
                if (pointer == null)
                {
                    //throw new Exception("HCM doesn't have offsets loaded to perform this operation with this version of MCC."
                    //    + $"\nSpecifically: {requiredPointerName}"
                    //    ); ;
                }
                requiredPointers.Add(requiredPointerName[3..], pointer); // Cut off the gamecode part so we can just refer to the rest of the name later
            }

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
            byte[] endZeroArray = new byte[10] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            endZeroArray.CopyTo(checkpointData, checkpointData[^10]);

            // Let's get the pointer to the inGameCheckpoint that we're going to overwrite
            ReadWrite.Pointer inGameCheckpointLocation;
            byte? doubleRevertFlag = null;
            switch ((int)game)
            {
                case 0:
                    inGameCheckpointLocation = (ReadWrite.Pointer)requiredPointers["CheckpointLocation1"];
                    break;

                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
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
                    break;

                default:
                    throw new Exception("2 TryInject was fed an invalid game, somehow. " + game.ToString());
            }



            // Modify the checkpointData to implement "Preserve Locations". These are sections of the checkpoint data where we want 
            // to preserve the in-game values instead of overwriting it with those of the checkpoint. This is necessary to fix issues with
            // sharing checkpoints between players, for some games anyway. 
            // Easiest way to implement this is to just read the data from the game and overwrite the checkpointData at those locations.

            // First, get the PreserveLocations for this game. If null, then don't bother fixing preserve locations for this game.
            PreserveLocation[]? preserveLocations = (PreserveLocation[]?)requiredPointers["CheckpointData_PreserveLocations"];
            if (preserveLocations != null)
            {
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

            // Get offset of SHA checksum relative to checkpoint file start
            int? shaOffset = (int?)requiredPointers["CheckpointData_SHAoffset"];
            int? shaLength = (int?)requiredPointers["CheckpointData_SHAlength"];
            // If offsets are null then don't bother for this game
            if (shaOffset != null && shaLength != null)
            {
                // Zero out the hash at the offset
                byte[] zeroes = new byte[(int)shaLength];
                zeroes.CopyTo(checkpointData, (int)shaOffset);

                // Calculate the checksum
                byte[]? newHash;
                using (SHA1 cryptoProvider = SHA1.Create())
                {
                    newHash = cryptoProvider.ComputeHash(checkpointData);
                }

                // Write the new hash 
                newHash.CopyTo(checkpointData, (int)shaOffset);
            }


            // Now, time to finally inject the checkpoint
            bool success = this.HaloMemoryService.ReadWrite.WriteData(inGameCheckpointLocation, checkpointData, false);

            if (!success) throw new Exception("Failed to inject the checkpoint into game memory");

            // Okay, checkpoint should be injected. But we still need to fix the in-game memories cached BSPs to match those of our checkpoint
            // This is only appliciable to some games. Currently just h2, h3, and ODST.
            ReadWrite.Pointer? pointerLoadedBSP1 = (ReadWrite.Pointer?)requiredPointers["LoadedBSP1"];
            ReadWrite.Pointer? pointerLoadedBSP2 = (ReadWrite.Pointer?)requiredPointers["LoadedBSP2"];
            int? loadedBSPoffset = (int?)requiredPointers["CheckpointData_LoadedBSPoffset"];
            int? loadedBSPlength = (int?)requiredPointers["CheckpointData_LoadedBSPlength"];
            // Skip this step if pointers are null
            if (pointerLoadedBSP1 != null && pointerLoadedBSP2 != null && loadedBSPoffset != null && loadedBSPlength != null)
            {
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
                bool success2 = this.HaloMemoryService.ReadWrite.WriteData(cachedBSPPointer, loadedBSPData, false);

                if (!success2) throw new Exception("Failed to write cachedBSP data to game memory");

            }

            // Wew, we're done!

        }
    }
}
