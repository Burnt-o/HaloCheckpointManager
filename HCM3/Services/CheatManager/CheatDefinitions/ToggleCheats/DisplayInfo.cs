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
    internal class DisplayInfo : AbstractToggleCheat
    {
        public DisplayInfo(ServicesGroup servicesGroup) : base(servicesGroup)
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


            DisplayInfoArguments diArgs = new();
            diArgs.ScreenX = Properties.Settings.Default.DIScreenX;
            diArgs.ScreenY = Properties.Settings.Default.DIScreenY;
            diArgs.SignificantDigits = Properties.Settings.Default.DISigDig;
            diArgs.FontSize = Properties.Settings.Default.DIFontSize;
            
            InternalReturn returnVal = this.InternalServices.CallInternalFunction("EnableDisplayInfo", diArgs); 
            if (returnVal != InternalReturn.True) throw new Exception("CallInternalFunction(EnableDisplayInfo) returned bad value, " + returnVal);


        }

        public override bool IsCheatApplied(HaloState? haloState = null)
        {
            return this.InternalServices.CallInternalFunction("IsDisplayInfoEnabled", null) == InternalReturn.True && this.InternalServices.CallInternalFunction("IsPlayerDataLoaded", null) == InternalReturn.True;
        }

        

        public override void RemoveCheat(HaloState? haloState = null)
        {
            InternalReturn returnVal = this.InternalServices.CallInternalFunction("DisableDisplayInfo", null);
            if (returnVal != InternalReturn.True) throw new Exception("CallInternalFunction(DisableDisplayInfo) returned bad value, " + returnVal);
        }

    }
}
