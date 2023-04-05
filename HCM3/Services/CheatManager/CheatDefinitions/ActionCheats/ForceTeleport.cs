using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using HCM3.Services;
using HCM3.Helpers;
using BurntMemory;
using HCM3.Services.Internal;

namespace HCM3.Services.Cheats
{
    internal class ForceTeleport : AbstractActionCheat
    {

        // Launches the player forward in the direction they're facing, with some user-inputted speed boost
        public ForceTeleport(ServicesGroup servicesGroup) : base(servicesGroup)
        {
        }

        protected override void ApplyCheat(HaloState haloState)
        {
            this.InternalServices.LoadPlayerDataIfNotLoaded(haloState);

            ForceTeleportArguments ftargs = new();
            ftargs.TeleportModeForward = Properties.Settings.Default.TeleportModeForward ? (uint)1 : (uint)0; // Need to cast from bool to uint so we can pass it to internal
            ftargs.TeleportIgnoreZ = Properties.Settings.Default.TeleportIgnoreZ ? (uint)1 : (uint)0; // Need to cast from bool to uint so we can pass it to internal
            ftargs.TeleportX = Properties.Settings.Default.TeleportX;
            ftargs.TeleportY = Properties.Settings.Default.TeleportY;
            ftargs.TeleportZ = Properties.Settings.Default.TeleportZ;
            ftargs.TeleportLength = Properties.Settings.Default.TeleportLength;

            this.InternalServices.CallInternalFunction("ForceTeleport", ftargs);

            return;

        }




    }
}
