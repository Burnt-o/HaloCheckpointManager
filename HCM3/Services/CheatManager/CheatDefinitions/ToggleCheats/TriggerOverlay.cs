using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using System.Diagnostics;
using HCM3.Services.HaloMemory;
using HCM3.Helpers;
using HCM3.Services.Internal;

namespace HCM3.Services.Cheats
{
    internal class TriggerOverlay : AbstractToggleCheat
    {
        public TriggerOverlay(ServicesGroup servicesGroup) : base(servicesGroup)
        {
        }



        public override void UpdateCheat(HaloState? haloState = null)
        {
            if (IsCheatApplied(null))
            {
                ApplyCheat(null);
            }
        }


        private IntPtr? DetourHandle { get; set; } = null;

        public override void ApplyCheat(HaloState? haloState = null)
        {
            if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();

            this.InternalServices.LoadPlayerDataIfNotLoaded(haloState);
            this.InternalServices.LoadCameraDataIfNotLoaded(haloState);
            this.InternalServices.LoadTriggerDataIfNotLoaded(haloState);

            InternalReturn returnVal = this.InternalServices.CallInternalFunction("EnableTriggerOverlay", null); 
            if (returnVal != InternalReturn.True) throw new Exception("CallInternalFunction(EnableTriggerOverlay)returned bad value, " + returnVal);


        }

        public override bool IsCheatApplied(HaloState? haloState = null)
        {
            return this.InternalServices.CallInternalFunction("IsTriggerOverlayEnabled", null) == InternalReturn.True;
        }

        

        public override void RemoveCheat(HaloState? haloState = null)
        {
            InternalReturn returnVal = this.InternalServices.CallInternalFunction("DisableTriggerOverlay", null);
            if (returnVal != InternalReturn.True) throw new Exception("CallInternalFunction(DisableTriggerOverlay) returned bad value, " + returnVal);
        }

    }
}
