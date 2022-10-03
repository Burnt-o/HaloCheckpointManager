using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;

namespace HCM3.Model
{
    internal class HaloMemory
    {
        public HaloState HaloState;
        public ReadWrite ReadWrite;
        public DebugManager DebugManager;
        public DLLInjector DLLInjector;
        public SpeedhackManager SpeedhackManager;

        public HaloMemory(MainModel mainModel)
        { 
        HaloState = new HaloState(mainModel);
        ReadWrite = new ReadWrite(HaloState);
        DebugManager = new DebugManager(HaloState, ReadWrite);
        DLLInjector = new DLLInjector(HaloState, ReadWrite);
        SpeedhackManager = new SpeedhackManager(HaloState, ReadWrite, DLLInjector);
        }

    }
}
