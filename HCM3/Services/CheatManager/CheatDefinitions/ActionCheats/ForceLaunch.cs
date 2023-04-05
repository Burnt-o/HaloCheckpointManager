using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using HCM3.Services;
using HCM3.Helpers;
using BurntMemory;

namespace HCM3.Services.Cheats
{
    internal class ForceLaunch : AbstractActionCheat
    {

        // Launches the player forward in the direction they're facing, with some user-inputted speed boost
        public ForceLaunch(ServicesGroup servicesGroup) : base(servicesGroup)
        {
        }

        protected override void ApplyCheat(HaloState haloState)
        {
            this.InternalServices.LoadPlayerDataIfNotLoaded(haloState);
            this.InternalServices.CallInternalFunction("ForceLaunch", Properties.Settings.Default.LaunchSpeed);

            return;

        }




    }
}
