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
            // get process
            HaloProcessInfo haloProcess = GetHaloProcessInfo();
            // get pointer
            IMultilevelPointer pointer = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, "ForceCheckpointFlag", haloProcess.processVersion);

            // write a value of 1 to it
            pointer.writeData(haloProcess.processHandle, new byte[] { 1 } );
        }
    }
}
