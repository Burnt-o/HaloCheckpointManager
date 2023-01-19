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

        private List<string> ListOfInternalFunction { get; set; }

        private bool firstTime = true;

        public InternalServices(HaloMemoryService haloMemoryService, DataPointersService dataPointersService)
        {
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;

            ListOfInternalFunction = new();
            ListOfInternalFunction.Add("ChangeDisplayText");
            ListOfInternalFunction.Add("IsTextDisplaying");
            ListOfInternalFunction.Add("IsOverlayHooked");
            ListOfInternalFunction.Add("PrintTemporaryMessage");
            ListOfInternalFunction.Add("hkPresent");
            ListOfInternalFunction.Add("hkResizeBuffers");
            ListOfInternalFunction.Add("RemoveHook");
            ListOfInternalFunction.Add("EnableHook");
            ListOfInternalFunction.Add("EnableDisplayInfo");
            ListOfInternalFunction.Add("DisableDisplayInfo");


            InternalFunctions = new();
            InternalFunctionsLoaded = false;
        }


        public bool InternalInjected()
        {
            //We'll get a ptr from internal functions. If this fails, internal not injected.
            //Then we read a single byte at the pointer. For hkPresent (doesn't matter what we chose) the starting byte should be 0x4D. so if it matches then internal is indeed injected.

            IntPtr functionPointer;
            if (!InternalFunctions.TryGetValue("hkPresent", out functionPointer)) return false;

            byte? testByte = this.HaloMemoryService.ReadWrite.ReadByte(functionPointer);
            return testByte == 0x40;
        }

        public bool InjectInternal()
        {

            Trace.WriteLine("Attempting to inject internal");
                lock (InjectInternalLock)
                {

                    try
                    {
                        SetupInternal(ListOfInternalFunction);
                    }
                    catch (Exception ex)
                    {
                        Trace.WriteLine("Failed setting up internal (and finding internalFunction pointers), ex: " + ex.ToString());
                    }
                }

                bool success = this.CheckInternalLoaded();

                if (!success) return false;

                Thread.Sleep(50);
                return this.CheckOverlayHooked();


        }



        
    }
}
