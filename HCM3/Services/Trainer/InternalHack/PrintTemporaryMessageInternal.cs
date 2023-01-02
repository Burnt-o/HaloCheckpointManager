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

        public bool PrintTemporaryMessageInternal(string str)
        {
            if (!CheckInternalLoaded()) return false;

            uint? exitCode = null;

            try
            {
                exitCode = CallInternalFunction("PrintTemporaryMessage", str);
            }
            catch (Exception ex)
            {
                Trace.WriteLine("Failed to call internal function PrintTemporaryMessage" + ex.ToString());
                return false;
            }
            return (exitCode.HasValue && exitCode == 1);
        }
                    

    }
}
