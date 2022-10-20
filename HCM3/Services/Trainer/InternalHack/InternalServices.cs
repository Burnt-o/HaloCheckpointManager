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



/*        [DllImport("kernel32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
        private static extern IntPtr GetProcAddress(IntPtr hModule, string procName);*/

        private Dictionary<string, IntPtr> InternalFunctions { get; set; }
        public bool InternalFunctionsLoaded { get; private set; }

        public HaloMemoryService HaloMemoryService { get; init; }
        public DataPointersService DataPointersService { get; init; }

        public CommonServices CommonServices { get; init; }

        public InternalServices(HaloMemoryService haloMemoryService, DataPointersService dataPointersService, CommonServices commonServices)
        {
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
            this.CommonServices = commonServices;

            InternalFunctions = new();
            InternalFunctionsLoaded = false;
        }


        public bool InjectInternal()
        {
            List<string> listOfInternalFunctions = new();
            listOfInternalFunctions.Add("ChangeDisplayText");
            listOfInternalFunctions.Add("IsTextDisplaying");
            try
            {
                SetupInternal(listOfInternalFunctions);
            }
            catch (Exception ex)
            { 
            Trace.WriteLine ("Failed setting up internal (and finding internalFunction pointers), ex: " + ex.Message);
            }
            return this.CheckInternalLoaded();
        }



        public IntPtr VirtualAllocExNear(IntPtr processHandle, int size, IntPtr location)
        {

            Trace.WriteLine("LOCATION:::::::: " + location.ToString("X"));
            //search within 2gb (2gb is actual max for 32bit jump)
            IntPtr min = IntPtr.Subtract(location, 0x8000000);
            IntPtr max = IntPtr.Add(location, 0x8000000);
            PInvokes.MEMORY_BASIC_INFORMATION mbi = new();
            uint mbiLength = (uint)System.Runtime.InteropServices.Marshal.SizeOf(mbi);

            Trace.WriteLine("Size of mbi: " + (int)System.Runtime.InteropServices.Marshal.SizeOf(mbi));


            // go forward first
            for (IntPtr Addr = location; Addr.ToInt64() < max.ToInt64(); Addr = IntPtr.Add(Addr, size))
            {
                if (PInvokes.VirtualQueryEx(processHandle, Addr, out mbi, mbiLength) == 0)
                {
                    //VirtualQuery failed
                    int lastError = Marshal.GetLastWin32Error();
                    Trace.WriteLine("Virtual query failed: " + lastError);
                    throw new Exception("Virtual Alloc Ex Near failed; virtualQuery failed, er: " + lastError);

                }
                //Trace.WriteLine("Got here at least 1");
                if (mbi.RegionSize == IntPtr.Zero) continue; //this should never happen if virtualQuery succeeded
                //Trace.WriteLine("Got here at least 2");
                //Trace.WriteLine(mbi.State.ToString("X"));
                if (mbi.State != 0x10000) continue; //not MEM_FREE
                Trace.WriteLine("Got here at least 3");
                //Virtual alloc needs 64k aligned addys, so get lowest 4 bytes using bitwise masking, then subtract that from the base address
                IntPtr tryAlloc = (IntPtr)(mbi.BaseAddress.ToUInt64() - (mbi.BaseAddress.ToUInt64() & 0xFFFF));
                IntPtr? actualAlloc = PInvokes.VirtualAllocEx(processHandle, tryAlloc, (uint)size, PInvokes.ALLOC_FLAGS.MEM_COMMIT | PInvokes.ALLOC_FLAGS.MEM_RESERVE, PInvokes.ALLOC_FLAGS.PAGE_EXECUTE_READWRITE);

                if (actualAlloc != null && actualAlloc != IntPtr.Zero) return (IntPtr)actualAlloc.Value;

            }
           

            throw new Exception("Failed to find free memory page near target location");

            

           



        }

    }
}
