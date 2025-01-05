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



            // get pointer to games checkpoint data
            IMultilevelPointer checkpointLocationA = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, "CheckpointLocationA", haloProcess.processVersion);

            // write our data over it
            checkpointLocationA.writeData(haloProcess.processHandle, checkpointData);

            // force a revert if user wanted that
            ForceRevert();

            throw new NotImplementedException("InjectCheckpoint not finished yet");
        }
    }
}
