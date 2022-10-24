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
        public void SetSpeedHackInternal(double speed)
        {
            if (speed < 0 || speed > 10000) throw new Exception("Invalid speed value");

            Trace.WriteLine("Trying to set speed to: " + speed);
            CallInternalFunction("SetSpeedHack", speed);
        }

    }
}
