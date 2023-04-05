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
    abstract internal class AbstractActionCheat : INotifyPropertyChanged, ICheat
    {
        protected HaloMemoryService HaloMemoryService;
        protected InternalServices InternalServices;
        protected DataPointersService DataPointersService;
        protected CheatManagerService CheatManagerService;



        public bool IsEnabledByPointerData { get; set; }

        public AbstractActionCheat(ServicesGroup servicesGroup)
        {
            HaloMemoryService = servicesGroup.HaloMemoryService;
            InternalServices = servicesGroup.InternalServices;
            DataPointersService = servicesGroup.DataPointersService;
            CheatManagerService = servicesGroup.CheatManagerService;

        }

        public event PropertyChangedEventHandler? PropertyChanged;


        protected abstract void ApplyCheat(HaloState haloState);

        // Used by UI to determine is cheat is actually able to applied, for example; for cheats that require overlay but overlay is disabled.
        public virtual bool IsPossibleToActivate
        {
            // Default implementation just returns IsEnabledByPointerData. 
            get { return IsEnabledByPointerData; }
        }

        // Single action cheat so just check for message success before attempting to apply
        public void DoCheat()
        {
            HaloState haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
            ApplyCheat(haloState);
        }

        public string EnabledMessage { get; set; }

    }
}
