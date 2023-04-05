using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCM3.Services.Cheats
{
    public interface ICheat
    {

        public void DoCheat();



        // Set by data in PointerData, is basically an override for IsEnabled
        public abstract bool IsEnabledByPointerData { get; set; }

        // Used by UI to determine is cheat is actually able to applied, for example; for cheats that require overlay but overlay is disabled.
        public virtual bool IsPossibleToActivate
        {
            // Default implementation just returns IsEnabledByPointerData. 
            get { return IsEnabledByPointerData; }
        }


    }
}
