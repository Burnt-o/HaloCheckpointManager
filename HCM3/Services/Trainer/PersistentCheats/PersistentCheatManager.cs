using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCM3.Services.Trainer
{
    public class PersistentCheatManager
    {
        public PC_ToggleInvuln PC_ToggleInvuln { get; init; }


        public PersistentCheatManager(PC_ToggleInvuln pC_ToggleInvuln)
        {
            this.PC_ToggleInvuln = pC_ToggleInvuln;
        }

    }



    

}
