using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using HCM3.Models;

namespace HCM3.Services
{
    public class HaloMemoryService
    {
        public HaloState HaloState;
        public ReadWrite ReadWrite;
        public DebugManager DebugManager;
        public DLLInjector DLLInjector;
        public SpeedhackManager SpeedhackManager;

        public HaloMemoryService(DataPointersService dataPointersService)
        { 
        HaloState = new HaloState(dataPointersService, this);
        ReadWrite = new ReadWrite(HaloState);
        DebugManager = new DebugManager(HaloState, ReadWrite);
        DLLInjector = new DLLInjector(HaloState, ReadWrite);
        SpeedhackManager = new SpeedhackManager(HaloState, ReadWrite, DLLInjector);
        }

    }
}
