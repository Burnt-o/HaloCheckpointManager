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
        public bool GetSpeedHackInternal()
        {

            if (!this.InternalInjected()) return false;

            uint? returnCode = CallInternalFunction("GetSpeedHack", null);
            Trace.WriteLine("GetSpeedHackInternal returned: " + returnCode.Value);
            return returnCode == 0; // GetSpeedhack will return 0 if speedhack is not equal to 1.00, 1 if it is.
            // We want this to return true when the speedhack is "enabled", false if value is 1.00
        }

    }
}
