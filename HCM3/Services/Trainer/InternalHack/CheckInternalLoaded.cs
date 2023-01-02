using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace HCM3.Services.Trainer
{
    public partial class InternalServices
    {

        public bool CheckInternalLoaded()
        {
            try
            {
                if (this.HaloMemoryService.HaloState.Attached == false)
                {
                    Trace.WriteLine("HCMInternal not loaded because HCM not attached to MCC!");
                    return false;
                }
                Process MCCProcess = Process.GetProcessById((int)this.HaloMemoryService.HaloState.ProcessID);

                foreach (ProcessModule module in MCCProcess.Modules)
                {
                    if (module.ModuleName == "HCMInternal.dll") return true;
                }
                return false;

            }
            catch (Exception ex)
            {
                Trace.WriteLine("Couldn't find HCMInternal in MCC, " + ex.ToString());
                return false;
            }
        }

    }
}
