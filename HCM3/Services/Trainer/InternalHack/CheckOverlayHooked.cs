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

        public bool CheckOverlayHooked()
        {
            try
            {
                uint? exitCode = CallInternalFunction("IsOverlayHooked", null);
                return exitCode == 1;
            }
            catch (Exception ex)
            {
                Trace.WriteLine("HCM internal overlay didn't seem to be hooked, " + ex.Message);
                return false;
            }
        }

    }
}
