using HCMExternal.Models;
using HCMExternal.Services.PointerData;
using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.Services.External.Impl
{
    internal partial class ExternalService : IExternalService
    {

        public void DisableCheckpoints(bool toggleValue)
        {
            List<HaloProcessInfo> haloProcesses = GetHaloProcessInfo();

            foreach (var haloProcess in haloProcesses)
            {
                // get pointer to part of code that writes 1 into the "force checkpoint" slot
                IMultilevelPointer naturalCheckpointCodePointer = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, "NaturalCheckpointCode", haloProcess.processVersion);

                // write the flag (protected)
                naturalCheckpointCodePointer.writeData(haloProcess.processHandle, toggleValue ? new byte[] { 0 } : new byte[] { 1 }, true);
            }


        }
    }           
}
