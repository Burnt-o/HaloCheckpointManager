using HCMExternal.Services.PointerData;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.Services.External.Impl
{
    internal partial class ExternalService : IExternalService
    {
        public void ForceCheckpoint()
        {
            List<HaloProcessInfo> haloProcesses = GetHaloProcessInfo();
            foreach (var haloProcess in haloProcesses)
            {
                IMultilevelPointer pointer = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, "ForceCheckpointFlag", haloProcess.processVersion);

                // write a value of 1 to it
                pointer.writeData(haloProcess.processHandle, new byte[] { 1 });
            }
        }
    }
}
