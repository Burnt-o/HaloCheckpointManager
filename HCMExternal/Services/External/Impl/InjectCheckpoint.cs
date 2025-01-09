using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCMExternal.Models;
using HCMExternal.Services.PointerData;

namespace HCMExternal.Services.External.Impl
{
    internal partial class ExternalService : IExternalService
    {
        public void InjectCheckpoint(Checkpoint checkpoint)
        {
            // get process
            HaloProcessInfo haloProcess = GetHaloProcessInfo();



            // load checkpoint data
            byte[] checkpointData = File.ReadAllBytes(checkpoint.CheckpointPath);

            { 
                // get length of checkpoint data
                int checkpointLength = PointerData.GetGameProcessData<int>(haloProcess.processType, haloProcess.haloGame, "CheckpointLength", haloProcess.processVersion);

                // confirm loaded length matches expected length
                if (checkpointData.Length != checkpointLength)
                    throw new Exception(string.Format("Checkpoint data at path {0} had unexpected length: 0x{1:X} instead of 0x{2:X}", checkpoint.CheckpointPath, checkpointData.Length, checkpointLength));
            }

            // get inject requirements
            InjectRequirements injectRequirements = PointerData.GetGameProcessData<InjectRequirements>(haloProcess.processType, haloProcess.haloGame, "InjectRequirements", haloProcess.processVersion);

            string desiredCheckpointSlot = "CheckpointLocationA";


            if (injectRequirements.singleCheckpoint == false)
            {
                // todo: read double-revert-flag and set desiredCheckpointSlot appropiately
                throw new NotImplementedException("double checkpoint slot games not impl yet");
            }

            // get pointer to games checkpoint data
            IMultilevelPointer checkpointLocation = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, desiredCheckpointSlot, haloProcess.processVersion);


            if (injectRequirements.preserveLocations)
            {
                // A "preserve location" means to keep some of the bytes of the previous checkpoint in memory instead of overwriting the whole thing.
                // This is generally for player and session specific data that would cause game crashes if overwritten
                byte[] previousCheckpointData = checkpointLocation.readData(haloProcess.processHandle, checkpointData.Length);

                // Get preserve locations
                PreserveLocationArray preserveLocations = PointerData.GetGameProcessData<PreserveLocationArray>(haloProcess.processType, haloProcess.haloGame, "PreserveLocationArray", haloProcess.processVersion);

                foreach (PreserveLocation preserveLocation in preserveLocations.data)
                {
                    // copy previousCheckpointData into our checkpointData buffer at the specified length and offset
                    Array.Copy(previousCheckpointData, preserveLocation.Offset, checkpointData, preserveLocation.Offset, preserveLocation.Length);
                }

                if (injectRequirements.SHA)
                {
                    // Need to recalculate SHA checksum
                    throw new NotImplementedException("SHA checksum calculation not implemented yet");
                }

            }

            // write our data over it
            checkpointLocation.writeData(haloProcess.processHandle, checkpointData, true);


            if (injectRequirements.BSP)
            {
                // Need to set the checkpoint-cache-bsp index ingame to match the checkpoints BSP index to prevent a fatal crash. 

               // throw new NotImplementedException("BSP cache index setting not implemented yet");
            }


            // force a revert if user wanted that
            //ForceRevert();

            throw new NotImplementedException("InjectCheckpoint not finished yet");
        }
    }
}
