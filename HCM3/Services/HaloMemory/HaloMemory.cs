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
        public SpeedhackManager SpeedhackManager;

        public HaloMemoryService(DataPointersService dataPointersService, HotkeyManager hotkeyManager)
        { 
        HaloState = new HaloState(dataPointersService, this, hotkeyManager);
        ReadWrite = new ReadWrite(HaloState);
        DebugManager = new DebugManager(HaloState, ReadWrite);
        SpeedhackManager = new SpeedhackManager(HaloState, ReadWrite);
        }

    }
}
