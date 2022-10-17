using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Threading;

namespace HCM3.Services
{
    public class InternalServices
    {



        [DllImport("kernel32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
        private static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

        public HaloMemoryService HaloMemoryService { get; init; }
        public DataPointersService DataPointersService { get; init; }

        public CommonServices CommonServices { get; init; }

        public InternalServices(HaloMemoryService haloMemoryService, DataPointersService dataPointersService, CommonServices commonServices)
        {
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
            this.CommonServices = commonServices;
        }


        public IntPtr? handleToInternalDLL { get; set; }

        public IntPtr? fpChangeText { get; set; }

        public bool InjectHCMInternal()
        {
            if (!this.HaloMemoryService.DLLInjector.InjectDLL("HCMInternal.dll", out IntPtr? moduleHandle)) throw new Exception("failed to inject?");

            if (moduleHandle == null) throw new Exception("module handle null?");


            //now load HCM into our own process
            IntPtr loadLibraryAddr = PInvokes.GetProcAddress(PInvokes.GetModuleHandle("kernel32.dll"), "LoadLibraryA");
            // Search for the named dll in the apps local folder.
            string currentfolder = System.AppDomain.CurrentDomain.BaseDirectory;
            string dllPath = currentfolder + "HCMInternal.dll";
            Trace.WriteLine("DLL path: " + dllPath);
            IntPtr allocMemAddress2;
            int sizeToAlloc = ((dllPath.Length + 1) * Marshal.SizeOf(typeof(char)));

            allocMemAddress2 = PInvokes.VirtualAllocEx(Process.GetCurrentProcess().Handle, IntPtr.Zero, (uint)sizeToAlloc, PInvokes.ALLOC_FLAGS.MEM_COMMIT | PInvokes.ALLOC_FLAGS.MEM_RESERVE, PInvokes.ALLOC_FLAGS.PAGE_READWRITE);
            PInvokes.WriteProcessMemory(Process.GetCurrentProcess().Handle, allocMemAddress2, Encoding.Default.GetBytes(dllPath), sizeToAlloc, out _);
            PInvokes.CreateRemoteThread(Process.GetCurrentProcess().Handle, IntPtr.Zero, 0, (IntPtr)loadLibraryAddr, allocMemAddress2, 0, IntPtr.Zero);
            Thread.Sleep(100);
            handleToInternalDLL = moduleHandle;

            // now we need to get info on the HCMInternal loaded within HCM, which we run GetProcAddress on the functions we want the IntPtrs to, then subtract the module address to get the offset
            IntPtr? ourDLL = null;
            using (Process curProcess = Process.GetCurrentProcess())
            {
                foreach (ProcessModule module in curProcess.Modules)
                {
                    Trace.WriteLine("module: " + module.ModuleName);
                    if (module.ModuleName == "HCMInternal.dll")
                    {
                        ourDLL = module.BaseAddress;
                        break;
                    }
                }
            }

            if (ourDLL == null) throw new Exception("couldn't find HCMInternal inside HCM");
            Trace.WriteLine("ourDLL IntPtr: " + ((Int64)ourDLL).ToString("X"));
            IntPtr? functionPointer = GetProcAddress((IntPtr)ourDLL, "ChangeText");

            if (functionPointer == IntPtr.Zero)
            {
                Trace.WriteLine("trying second thing");
                functionPointer = GetProcAddress((IntPtr)ourDLL, "ChangeText = ChangeText");
            }

            if (functionPointer == null || functionPointer == IntPtr.Zero) throw new Exception("couldn't find exported function in ourDLL");


            Trace.WriteLine("functionPointer IntPtr: " + ((Int64)functionPointer).ToString("X"));
            //now substract handleToInternalDLL from functionPointer to get offset
            Int64 fpOffset = (Int64)functionPointer - (Int64)ourDLL;
            Trace.WriteLine("fpOffset: " + fpOffset);
            IntPtr fpInternal = IntPtr.Add((IntPtr)handleToInternalDLL, (int)fpOffset);

            //now as a test, I want to try calling the function
            //We can do this with createremote thread, but first we need to write the parameters somewhere in memory
            string textParameter = "cummiesss";
            IntPtr MCCHandle = (IntPtr)this.HaloMemoryService.HaloState.processHandle;
            int sizeToAlloc2 = ((textParameter.Length + 1) * Marshal.SizeOf(typeof(char)));
            IntPtr? allocMemAddress = PInvokes.VirtualAllocEx(MCCHandle, IntPtr.Zero, (uint)sizeToAlloc2, PInvokes.ALLOC_FLAGS.MEM_COMMIT | PInvokes.ALLOC_FLAGS.MEM_RESERVE, PInvokes.ALLOC_FLAGS.PAGE_READWRITE);
            if (allocMemAddress == null) throw new Exception("couldn't alloc memory for parameter");

            PInvokes.VirtualProtectEx(MCCHandle, (IntPtr)allocMemAddress, sizeToAlloc2, PInvokes.PAGE_READWRITE, out uint lpflOldProtect);
            PInvokes.WriteProcessMemory(MCCHandle, (IntPtr)allocMemAddress, Encoding.Default.GetBytes(textParameter), sizeToAlloc2, out _);
            PInvokes.VirtualProtectEx(MCCHandle, (IntPtr)allocMemAddress, sizeToAlloc2, lpflOldProtect, out _);

            IntPtr? threadID = PInvokes.CreateRemoteThread(MCCHandle, IntPtr.Zero, 0, fpInternal, (IntPtr)allocMemAddress, 0, IntPtr.Zero);
            if (threadID == null) return false;

            uint waitFor = PInvokes.WaitForSingleObject((IntPtr)threadID, 3000);
            PInvokes.CloseHandle((IntPtr)threadID);
            PInvokes.VirtualFreeEx(MCCHandle, (IntPtr)allocMemAddress, textParameter.Length, PInvokes.AllocationType.Release);
            if (waitFor == 0x00000080) // WAIT_ABANDONED
            {
                Trace.WriteLine("callInternalFunc: Wait abandoned!");
            }
            else if (waitFor == 0x00000102) // WAIT_TIMEOUT
            {
                Trace.WriteLine("callInternalFunc: Wait timeout!");
            }
            Trace.WriteLine("callInternalFunc: remote thread completed!");
            PInvokes.GetExitCodeThread((IntPtr)threadID, out uint exitCode);
            Trace.WriteLine("callInternalFunc: remote thread exit code: " + exitCode);

            if (exitCode == 0)
            {
                //Failed!
                int lastError = Marshal.GetLastWin32Error();
                Trace.WriteLine("callInternalFunc: ERROR! info: " + lastError);
            }
            return true;


        }

    }
}
