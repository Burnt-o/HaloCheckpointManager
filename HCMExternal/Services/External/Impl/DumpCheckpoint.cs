using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCMExternal.Models;
using HCMExternal.Services.PointerData;
using System.IO;
using System.Runtime.InteropServices;
using System.Diagnostics;
using HCMExternal.Services.External.FileMapping;

namespace HCMExternal.Services.External.Impl
{
    internal partial class ExternalService : IExternalService
    {

        public void DumpCheckpoint(SaveFolder saveFolder)
        {
            // get process
            HaloProcessInfo haloProcess = GetHaloProcessInfo();
            if (haloProcess.processType != HaloProcessType.ProjectCartographer)
                throw new NotImplementedException("Only project cartographer supports external save management");

            // get inject requirements
            InjectRequirements injectRequirements = PointerData.GetGameProcessData<InjectRequirements>(haloProcess.processType, haloProcess.haloGame, "InjectRequirements", haloProcess.processVersion);

            // TODO: check which checkpoint slot it is off dr flag
            // TODO: lookup pointer of file handle in h2 exe
            UInt32 saveFileHandle = 0x970;

            // get length of checkpoint data
            int checkpointLength = PointerData.GetGameProcessData<int>(haloProcess.processType, haloProcess.haloGame, "CheckpointLength", haloProcess.processVersion);


            // throws on failure
            var unmanagedCheckpointData = MappedCartographerSaveFactory.make(saveFileHandle, haloProcess.processHandle, checkpointLength);

            byte[] managedBuffer = new byte[checkpointLength];
            Marshal.Copy(unmanagedCheckpointData.data(), managedBuffer, 0, checkpointLength);

            // TODO: ask user where to save the checkpoint file
            File.WriteAllBytes(Directory.GetCurrentDirectory() + @"\Saves\Project Cartographer\CartographerTestDump3.bin", managedBuffer);

        }

    }
}
