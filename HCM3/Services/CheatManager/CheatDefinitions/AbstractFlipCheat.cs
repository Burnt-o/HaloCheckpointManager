using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Helpers;
using System.ComponentModel;
using HCM3.Services.HaloMemory;
using HCM3.Services.Internal;

namespace HCM3.Services.Cheats
{
    abstract internal class AbstractFlipCheat : INotifyPropertyChanged, ICheat
    {
        protected HaloMemoryService HaloMemoryService;
        protected InternalServices InternalServices;
        protected DataPointersService DataPointersService;
        protected CheatManagerService CheatManagerService;

        public bool IsEnabledByPointerData { get; set; }


        public AbstractFlipCheat(ServicesGroup servicesGroup)
        {
            HaloMemoryService = servicesGroup.HaloMemoryService;
            InternalServices = servicesGroup.InternalServices;
            DataPointersService = servicesGroup.DataPointersService;
            CheatManagerService = servicesGroup.CheatManagerService;

        }

        public event PropertyChangedEventHandler? PropertyChanged;


        protected abstract void FlipCheat(HaloState haloState);



        public void DoCheat()
        {
            HaloState haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
            FlipCheat(haloState);
        }


        // Used by UI to determine is cheat is actually able to applied, for example; for cheats that require overlay but overlay is disabled.
        public virtual bool IsPossibleToActivate
        {
            // Default implementation just returns IsEnabledByPointerData. 
            get { return IsEnabledByPointerData; }
        }


        public string EnabledMessage { get; set; }
        public string DisabledMessage { get; set; }
    }
}
