﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using BurntMemory;
using System.Runtime.InteropServices;

namespace HCM3.Services.Trainer
{
    public partial class PersistentCheatService
    {
        public IntPtr VirtualAllocExNear(IntPtr processHandle, int size, IntPtr location)
        {
            int incrementor = 1000;
            Trace.WriteLine("LOCATION:::::::: " + location.ToString("X"));
            //search within 2gb (2gb is actual max for 32bit jump)
            IntPtr min = IntPtr.Subtract(location, 0x7000000);
            IntPtr max = IntPtr.Add(location, 0x7000000);
            PInvokes.MEMORY_BASIC_INFORMATION mbi = new();
            uint mbiLength = (uint)System.Runtime.InteropServices.Marshal.SizeOf(mbi);

            Trace.WriteLine("Size of mbi: " + (int)System.Runtime.InteropServices.Marshal.SizeOf(mbi));


            // go forward first
            for (IntPtr Addr = location; Addr.ToInt64() < max.ToInt64(); Addr = IntPtr.Add(Addr, incrementor))
            {
                if (PInvokes.VirtualQueryEx(processHandle, Addr, out mbi, mbiLength) == 0)
                {
                    //VirtualQuery failed
                    int lastError = Marshal.GetLastWin32Error();
                    Trace.WriteLine("Virtual query failed: " + lastError);
                    //throw new Exception("Virtual Alloc Ex Near failed; virtualQuery failed, er: " + lastError + ", add: " + Addr.ToString("X") + ", starting loc: " + location.ToString("X"));
                    continue;
                }



                if (mbi.State != 0x10000) //not MEM_FREE
                {
                    // Set incrementor to regionSize (+1000 in case regionsize was really small)
                    incrementor = (int)mbi.RegionSize + 1000;
                    if (incrementor <= 0) Trace.WriteLine("INCREMENTOR IS NEGATIVE AHHHHHHHH");
                    Trace.WriteLine("incrementer: " + incrementor.ToString("X") + ", regionsize: " + ((int)mbi.RegionSize).ToString("X") + ", size: " + size + ", mbi state: " + mbi.State.ToString("X"));
                    continue;
                }

                // memory region not big enough
                if (mbi.RegionSize.ToUInt32() < (uint)size)
                {
                    Trace.WriteLine("Region size wasn't big enough!");
                    continue;
                }

                //Virtual alloc needs 64k aligned addys, so get lowest 4 bytes using bitwise masking, then subtract that from the base address
                IntPtr tryAlloc = (IntPtr)(mbi.BaseAddress.ToUInt64() - (mbi.BaseAddress.ToUInt64() & 0xFFFF));
                IntPtr? actualAlloc = PInvokes.VirtualAllocEx(processHandle, tryAlloc, (uint)size, PInvokes.ALLOC_FLAGS.MEM_COMMIT | PInvokes.ALLOC_FLAGS.MEM_RESERVE, PInvokes.ALLOC_FLAGS.PAGE_EXECUTE_READWRITE);

                if (actualAlloc != null && actualAlloc != IntPtr.Zero) return (IntPtr)actualAlloc.Value;

            }

            Trace.WriteLine("let's try going backward");
            incrementor = 1000;
            // now go backwards
            for (IntPtr Addr = location; Addr.ToInt64() < max.ToInt64(); Addr = IntPtr.Subtract(Addr, incrementor))
            {
                if (PInvokes.VirtualQueryEx(processHandle, Addr, out mbi, mbiLength) == 0)
                {
                    //VirtualQuery failed
                    int lastError = Marshal.GetLastWin32Error();
                    Trace.WriteLine("Virtual query failed: " + lastError);
                    throw new Exception("Virtual Alloc Ex Near failed; virtualQuery failed, er: " + lastError);

                }



                if (mbi.State != 0x10000) //not MEM_FREE
                {
                    // Set incrementor to regionSize (+1000 in case regionsize was really small)
                    incrementor = (int)mbi.RegionSize + 1000;
                    if (incrementor <= 0)
                    {
                        throw new Exception("INCREMENTOR IS NEGATIVE AHHHHHHHH");
                    }

                    Trace.WriteLine("incrementer: " + incrementor.ToString("X") + ", regionsize: " + ((int)mbi.RegionSize).ToString("X") + ", size: " + size + ", mbi state: " + mbi.State.ToString("X"));
                    continue;
                }

                // memory region not big enough
                if (mbi.RegionSize.ToUInt32() < (uint)size)
                {
                    Trace.WriteLine("Region size wasn't big enough!");
                    continue;
                }

                //Virtual alloc needs 64k aligned addys, so get lowest 4 bytes using bitwise masking, then subtract that from the base address
                IntPtr tryAlloc = (IntPtr)(mbi.BaseAddress.ToUInt64() - (mbi.BaseAddress.ToUInt64() & 0xFFFF));
                IntPtr? actualAlloc = PInvokes.VirtualAllocEx(processHandle, tryAlloc, (uint)size, PInvokes.ALLOC_FLAGS.MEM_COMMIT | PInvokes.ALLOC_FLAGS.MEM_RESERVE, PInvokes.ALLOC_FLAGS.PAGE_EXECUTE_READWRITE);

                if (actualAlloc != null && actualAlloc != IntPtr.Zero) return (IntPtr)actualAlloc.Value;

            }


            throw new Exception("Failed to find free memory page near target location");







        }
    }
}