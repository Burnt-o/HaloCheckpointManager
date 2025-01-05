using HCMExternal.Services.PointerData;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.Services.External.Impl
{
    internal partial class ExternalService : IExternalService
    {
        public void ForceDoubleRevert()
        {
            // get process
            HaloProcessInfo haloProcess = GetHaloProcessInfo();
           
            // get pointer to double revert flag
            IMultilevelPointer doubleRevertFlagPointer = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, "DoubleRevertFlag", haloProcess.processVersion);

            // read the flag
            byte[] doubleRevertFlagValue = doubleRevertFlagPointer.readData(haloProcess.processHandle, 1);


            // write back the inverse of the flag to set the current checkpoint to the other slot
            doubleRevertFlagPointer.writeData(haloProcess.processHandle, doubleRevertFlagValue[0] == 0 ? new byte[] { 1 } : new byte[] { 0 });

            // call ForceRevert
            ForceRevert();
        }
    }
}
