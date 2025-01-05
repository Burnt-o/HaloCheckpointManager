using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCMExternal.Models;
using HCMExternal.Services.PointerData;
using System.IO;

namespace HCMExternal.Services.External.Impl
{
    internal partial class ExternalService : IExternalService
    {
        public void DumpCheckpoint(SaveFolder saveFolder)
        {
            // get process
            HaloProcessInfo haloProcess = GetHaloProcessInfo();

            // get pointer to checkpoint data
            IMultilevelPointer checkpointLocationA = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, "CheckpointLocationA", haloProcess.processVersion);

            // get length of checkpoint data
            int checkpointLength = PointerData.GetGameProcessData<int>(haloProcess.processType, haloProcess.haloGame, "CheckpointLength", haloProcess.processVersion);

            // read checkpoint data
            byte[] checkpointData = checkpointLocationA.readData(haloProcess.processHandle, checkpointLength);

            // TODO prompt user with dialog to name the checkpoint file

            // save checkpoint data to disk 
            File.WriteAllBytes(Directory.GetCurrentDirectory() + @"\Saves\Project Cartographer\CartographerTestDump.bin", checkpointData);

            throw new NotImplementedException("DumpCheckpoint not finished yet");
        }
    }
}
