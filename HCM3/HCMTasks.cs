using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;

namespace HCM3
{
    public class HCMTasks
    {
        public HaloState HaloState { get; init; }
        public ReadWrite ReadWrite { get; init; }
        public DebugManager DebugManager { get; init; }
        public DLLInjector DLLInjector { get; init; }
        public SpeedhackManager SpeedhackManager { get; init; }

        public HCMTasks()
            {
            HaloState = new ();
            ReadWrite = new (HaloState);
            DebugManager = new (HaloState, ReadWrite);
            DLLInjector = new (HaloState, ReadWrite);
            SpeedhackManager = new (HaloState, ReadWrite, DLLInjector);
            }


        public void H1Checkpoint()
        { 
        
        }
    }
}
