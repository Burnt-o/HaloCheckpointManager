using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Packaging;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using HCMExternal.Models;
using HCMExternal.Services.External.FileMapping;
using HCMExternal.Services.PointerData;
using Serilog;

namespace HCMExternal.Services.External.Impl
{
    internal partial class ExternalService : IExternalService
    {

        // MCC stores checkpoints in its process memory; read/write is as simple as Read(Write)ProcessMemory https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-readprocessmemory
        private void InjectCheckpointMCC(Checkpoint checkpoint, HaloProcessInfo haloProcess)
        {
            // get inject requirements
            InjectRequirements injectRequirements = PointerData.GetGameProcessData<InjectRequirements>(haloProcess.processType, haloProcess.haloGame, "InjectRequirements", haloProcess.processVersion);

            throw new NotImplementedException("MCC external injecting not implemented. Use internal overlay instead.");
        }


        // Cartographer stores checkpoints on disk with non-shared file handles. We need to duplicate the handles then map them to our memory so we can read/write to them without messing with the games filestream pointer.
        private void InjectCheckpointCartographer(Checkpoint checkpoint, HaloProcessInfo haloProcess)
        {
            // Need to make sure both checkpoint slots are already occupied
            // (for multiplayer, they usually aren't).
            // Otherwise a crash is likely.

            {
                IMultilevelPointer CheckpointOccupiedSlotAPointer = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, "CheckpointOccupiedSlotA", haloProcess.processVersion);
                IMultilevelPointer CheckpointOccupiedSlotBPointer = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, "CheckpointOccupiedSlotB", haloProcess.processVersion);

                int tries = 0;
                int giveup = 10;
                while (CheckpointOccupiedSlotAPointer.readData(haloProcess.processHandle, 1)[0] != 1 || CheckpointOccupiedSlotBPointer.readData(haloProcess.processHandle, 1)[0] != 1)
                {
                    Log.Verbose("Forcing checkpoint to occupy slots before injection!");
                    ForceCheckpoint();
                    System.Threading.Thread.Sleep(100);
                    tries++;
                    if (tries > giveup)
                        throw new Exception("Failed to occupy checkpoint slots! Cannot inject a checkpoint into empty checkpoint slots.");
                }
            }



            // load checkpoint data from disk
            byte[] checkpointData = File.ReadAllBytes(checkpoint.CheckpointPath);

            // get expected length of checkpoint data
            UInt32 checkpointLength = (UInt32)PointerData.GetGameProcessData<int>(haloProcess.processType, haloProcess.haloGame, "CheckpointLength", haloProcess.processVersion);

            // confirm loaded length matches expected length
            if (checkpointData.Length != checkpointLength)
                throw new Exception(string.Format("Checkpoint data at path {0} had unexpected length: 0x{1:X} instead of 0x{2:X}", checkpoint.CheckpointPath, checkpointData.Length, checkpointLength));




            // get pointer to double revert flag
            IMultilevelPointer doubleRevertFlagPointer = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, "DoubleRevertFlag", haloProcess.processVersion);

            // use it to figure which checkpoint slot we're dealing with.
            // TODO: injecting failing when isCheckpointSlotA == true. works fine when false. aka CheckpoitnFileHandleA is broken, somehow. 
            bool isCheckpointSlotA = doubleRevertFlagPointer.readData(haloProcess.processHandle, 1)[0] == 0;
            Log.Debug("isCheckpointSlotA: " + isCheckpointSlotA);
            Log.Debug("doubleRevertFlagPointer value: " + doubleRevertFlagPointer.readData(haloProcess.processHandle, 1)[0]);

            string desiredCheckpointFileHandle = isCheckpointSlotA ?
                "CheckpointFileHandleA" :
                "CheckpointFileHandleB";

            // get pointer to checkpoint file handle
            IMultilevelPointer CheckpointFileHandlePointer = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, desiredCheckpointFileHandle, haloProcess.processVersion);

            // read it
            UInt32 CheckpointFileHandle = BitConverter.ToUInt32(CheckpointFileHandlePointer.readData(haloProcess.processHandle, sizeof(UInt32)));



            // map previous checkpoint file handle to memory
            var unmanagedCheckpointData = MappedCartographerSaveFactory.make(CheckpointFileHandle, haloProcess.processHandle, checkpointLength);

            // copy data of previous checkpoint
            byte[] previousCheckpointData = new byte[checkpointLength];
            Marshal.Copy(unmanagedCheckpointData.data(), previousCheckpointData, 0, (int)checkpointLength);


            // Preserve preserve locations.
            // A "preserve location" means to keep some of the bytes of the previous checkpoint in memory instead of overwriting the whole thing.
            // This is generally for player and session specific data that would cause game crashes if overwritten

            // Get preserve locations
            PreserveLocationArray preserveLocations = PointerData.GetGameProcessData<PreserveLocationArray>(haloProcess.processType, haloProcess.haloGame, "PreserveLocationArray", haloProcess.processVersion);

            foreach (PreserveLocation preserveLocation in preserveLocations.data)
            {
                // for debugging
                {
                    Log.Verbose(string.Format("Preserving checkpoint data at 0x{0:X}, length 0x{1:X}", preserveLocation.Offset, preserveLocation.Length));

                    byte[] overwrittenBytes = new byte[preserveLocation.Length];
                    byte[] preservedBytes = new byte[preserveLocation.Length];

                    Array.Copy(checkpointData, preserveLocation.Offset, overwrittenBytes, 0, preserveLocation.Length);
                    Array.Copy(previousCheckpointData, preserveLocation.Offset, preservedBytes, 0, preserveLocation.Length);

                    Log.Verbose(string.Format("Preserved value at this preserve location was: " + Convert.ToHexString(preservedBytes)));
                    Log.Verbose(string.Format("We prevented overwritting it with the value:   " + Convert.ToHexString(overwrittenBytes)));
                    if (overwrittenBytes.SequenceEqual(preservedBytes))
                        Log.Verbose("(These were the same value so no actual effect)");
                }

                // copy previousCheckpointData into our checkpointData buffer at the specified length and offset
                Array.Copy(previousCheckpointData, preserveLocation.Offset, checkpointData, preserveLocation.Offset, preserveLocation.Length);

            }


            // write our data over the mapped checkpoint file
            Marshal.Copy(checkpointData, 0, unmanagedCheckpointData.data(), (int)checkpointLength);


            // Need to set loaded bsp caches in game so that cross-bsp injections work properly
            {
                // use dr flag to figure out which offset we want
                string desiredCheckpointLoadedBSP = isCheckpointSlotA ?
                "LoadedBSPCheckpointA" :
                "LoadedBSPCheckpointB";

                // get pointer to cached bsp index
                IMultilevelPointer CheckpointLoadedBSP = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, desiredCheckpointLoadedBSP, haloProcess.processVersion);

                // Need to set it to the value that matches our checkpoint file.. 
                int CheckpointLoadedBSPOffset = PointerData.GetGameProcessData<int>(haloProcess.processType, haloProcess.haloGame, "CheckpointLoadedBSPOffset", haloProcess.processVersion);
                byte DesiredLoadedBSP = checkpointData.ElementAt(CheckpointLoadedBSPOffset);

                CheckpointLoadedBSP.writeData(haloProcess.processHandle, new byte[] { DesiredLoadedBSP });

            }

            // force a revert if user wants
            if (Properties.Settings.Default.RevertAfterInject)
            {
                ForceRevert();
            }
                

        }


        public void InjectCheckpoint(Checkpoint checkpoint)
        {
            List<HaloProcessInfo> haloProcesses = GetHaloProcessInfo();
            foreach (var haloProcess in haloProcesses)
            { 
                if (haloProcess.processType == HaloProcessType.ProjectCartographer)
                    InjectCheckpointCartographer(checkpoint, haloProcess);
                else
                    InjectCheckpointMCC(checkpoint, haloProcess);
            }
        }
    }
}
