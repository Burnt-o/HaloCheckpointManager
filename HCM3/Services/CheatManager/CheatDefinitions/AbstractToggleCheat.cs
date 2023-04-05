using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Helpers;
using System.ComponentModel;
using HCM3.Services.HaloMemory;
using System.Diagnostics;
using HCM3.Services.Internal;
using System.Windows;

namespace HCM3.Services.Cheats
{
    abstract public class AbstractToggleCheat :  ICheat
    {
        protected HaloMemoryService HaloMemoryService;
        protected InternalServices InternalServices;
        protected DataPointersService DataPointersService;
        protected CheatManagerService CheatManagerService;

        public bool IsEnabledByPointerData { get; set; }


        public AbstractToggleCheat(ServicesGroup servicesGroup)
        {
            HaloMemoryService = servicesGroup.HaloMemoryService;
            InternalServices = servicesGroup.InternalServices;
            DataPointersService = servicesGroup.DataPointersService;
            CheatManagerService = servicesGroup.CheatManagerService;
        }


        // For binding by ToggleControlViewModel






        public abstract void ApplyCheat(HaloState haloState = null); //promoted from protected to public as cheatmanager needs to access it for reapplication on HaloStateChange events
        public abstract void RemoveCheat(HaloState? haloState = null);
        public abstract bool IsCheatApplied(HaloState? haloState = null);

        // Used by toggle cheats that have settings, so if they're turned on they can re-apply the cheat with changed settings
        public virtual void UpdateCheat(HaloState? haloState = null)
        {
            return;
        }


        //Toggle cheat so toggle logic
        public void DoCheat()
        {
            HaloState haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
            if (IsCheatApplied(haloState))
            {
                RemoveCheat(haloState);
                // Don't need successful cheat message to turn cheat off
                try
                {
                    this.CheatManagerService.SendToggleCheatMessage(this, haloState, false);
                }
                catch (Exception ex)
                { 
                Trace.WriteLine("Error sending message when removing cheat: " + ex.ToString());
                }
                
            }
            else
            {
                // Need sending the cheat message to be successful, so send it first
                this.CheatManagerService.SendToggleCheatMessage(this, haloState, true);
                ApplyCheat(haloState);
            }
        }

        // Used by UI to determine is cheat is actually able to applied, for example; for cheats that require overlay but overlay is disabled.
        public virtual bool IsPossibleToActivate
        {
            // Default implementation just returns IsEnabledByPointerData. 
            get { return IsEnabledByPointerData; }
        }


        public string EnabledMessage { get; set; }
        public string DisabledMessage { get; set;  }
        public string ActiveMessage { get; set; }

    }
}
