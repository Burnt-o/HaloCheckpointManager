using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Services.Internal;
using System.Diagnostics;

namespace HCM3.Services.Cheats
{
    public class Speedhack : AbstractToggleCheat
    {
        public Speedhack(ServicesGroup servicesGroup) : base(servicesGroup)
        {
            this.EnabledMessage = $"Speedhack enabled";
            this.DisabledMessage = $"Speedhack disabled";
            this.ActiveMessage = "Speedhack";
        }

        public override void ApplyCheat(HaloState? haloState = null)
        {
            if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();

            double speedToApply;

            if (Properties.Settings.Default.DisableOverlay)
            {
                speedToApply = 10;
            }
            else
            {
                speedToApply = Properties.Settings.Default.SpeedHackValue;
                Trace.WriteLine("speedToApply is apparently: " + speedToApply);
                    if (speedToApply == 1.00) // If speedhack value is 1.00 then might as well leave it turned off.
                {
                    throw new Exception("Speedhack value of one is just equivalent to turning the cheat off");
                    }; 
                    
            }

            if (speedToApply <= 0) throw new Exception("Invalid speed value - must be positive");
            InternalReturn returnVal = this.InternalServices.CallInternalFunction("SetSpeedhack", speedToApply);
            if (returnVal != InternalReturn.True)
            {
                throw new Exception("Tried to SetSpeedhack to " + speedToApply + " but failed, returnVal: " + returnVal);
            }

        }

        public override bool IsCheatApplied(HaloState? haloState = null)
        {
            if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
            try
            {
                if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
                InternalReturn returnVal = this.InternalServices.CallInternalFunction("SpeedIsUnmodified", null);
                bool isCheatApplied = !(returnVal != InternalReturn.False);

                Trace.WriteLine("SpeedIsUnmodified return val is " + returnVal + ", therefore returning: " + isCheatApplied);

                return isCheatApplied; // if error, we assume speedhack is not applied
            }
            catch (Exception ex)
            {
                Trace.WriteLine("Something went wrong determining if speedhack IsCheatApplied, " + ex.ToString());
                return false;
            }

        }

        public override void RemoveCheat(HaloState? haloState = null)
        {
         InternalReturn returnVal = this.InternalServices.CallInternalFunction("SetSpeedhack", (double)1.0);
            Trace.WriteLine("Trying to remove speedhack, SetSpeedhack(1.0) returned: " + returnVal);

        }


        public override void UpdateCheat(HaloState? haloState = null)
        {
            if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
                ApplyCheat(haloState);
        }

    }
}
