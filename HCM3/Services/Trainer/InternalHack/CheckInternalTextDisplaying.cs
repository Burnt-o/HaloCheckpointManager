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

        public bool CheckInternalTextDisplaying()
        {
            if (!CheckInternalLoaded()) return false;

            uint? exitCode = null;

            try
            {
                exitCode = CallInternalFunction("IsTextDisplaying", null);
            }
            catch (Exception ex)
            {
                Trace.WriteLine("Failed to call internal function IsTextDisplaying" + ex.Message);
                return false;
            }
            return (exitCode.HasValue && exitCode == 1);

        }

    }
}
