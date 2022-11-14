using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Threading;

namespace HCM3.Services.Trainer
{
    public partial class InternalServices
    {

        private Dictionary<string, IntPtr> InternalFunctions { get; set; }

        //Accessed by SetSpeedHack
        private IntPtr setAllToSpeed { get; set; }

        //Accessed by GetSpeedHack
        private IntPtr getSpeed { get; set; }
        public bool InternalFunctionsLoaded { get; private set; }

        public HaloMemoryService HaloMemoryService { get; init; }

        private readonly object InjectInternalLock = new object();

        public DataPointersService DataPointersService { get; init; }

        public InternalServices(HaloMemoryService haloMemoryService, DataPointersService dataPointersService)
        {
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;

            InternalFunctions = new();
            InternalFunctionsLoaded = false;
        }




        public bool InjectInternal()
        {
            lock (InjectInternalLock)
            {
                List<string> listOfInternalFunctions = new();
                listOfInternalFunctions.Add("ChangeDisplayText");
                listOfInternalFunctions.Add("IsTextDisplaying");
                listOfInternalFunctions.Add("PrintTemporaryMessage");
                listOfInternalFunctions.Add("hkPresent");
                try
                {
                    SetupInternal(listOfInternalFunctions);
                }
                catch (Exception ex)
                {
                    Trace.WriteLine("Failed setting up internal (and finding internalFunction pointers), ex: " + ex.Message);
                }
            }
            return this.CheckInternalLoaded();
        }



        
    }
}
