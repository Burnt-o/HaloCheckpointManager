using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using BurntMemory;

namespace HCM3.Services.HaloMemory
{
    public partial class HaloMemoryService
    {
        public IntPtr DetourApply(DetourInfoObject detourInfo)
        {

            // Get location of original code where we're going to apply the hook
            Trace.WriteLine("detourinfo OGLocation: " + detourInfo.OriginalCodeLocation.Offsets[0].ToString());
            IntPtr ogCodeHandle = this.ReadWrite.ResolvePointer(detourInfo.OriginalCodeLocation) ?? throw new Exception("Couldn't read location of origianl code");

            // Get process Handle
            uint pID = this.HaloMemoryManager.ProcessID ?? throw new Exception("Couldn't get process ID");
            Process MCCProcess = Process.GetProcessById(((int?)pID) ?? throw new Exception("Couldn't get process info"));
            IntPtr processHandle = MCCProcess.Handle;

            // Allocate some memory for our detour that hook will jump to
            // We want this to be near the originalCodeLocation (within 2 gb) we our relative jmp instruction will be <= 6 bytes long
            IntPtr detourHandle = this.VirtualAllocExNear(processHandle, detourInfo.SizeToAlloc, ogCodeHandle);
            // Add $detourHandle to the SymbolPointers
            Dictionary<string, object> symbolPointers = new();
            symbolPointers = symbolPointers.Concat(detourInfo.SymbolPointers).ToDictionary(x=>x.Key, x=>x.Value);
            symbolPointers.Add("$detourHandle", new ReadWrite.Pointer(detourHandle));
            symbolPointers.Add("@detourHandle", new ReadWrite.Pointer(detourHandle));

            // Assemble hook bytes
            byte[] originalCodeBytes = detourInfo.OriginalCodeBytes;
            byte[] hookCodeBytes = new byte[originalCodeBytes.Length];
            Array.Fill(hookCodeBytes, (byte)0x90); // Fill with NOP instructions for now




            byte[] tempHookCodeBytes = DetourAssemble(detourInfo.HookCodeASM, ogCodeHandle, symbolPointers, out int hookBytesAssembled, out _);
            if (tempHookCodeBytes.Length > hookCodeBytes.Length) throw new Exception("Assembled hook code longer than code to replace"); ;
            Array.Copy(tempHookCodeBytes, hookCodeBytes, tempHookCodeBytes.Length);

            // Assemble detour bytes
            byte[] detourCodeBytes = DetourAssemble(detourInfo.DetourCodeASM, detourHandle, symbolPointers, out int detourBytesAssembled, out _);

            // For debugging purposes
            Trace.WriteLine("\nHookBytes: \n");
            foreach (byte b in hookCodeBytes) { Trace.Write(b.ToString("X2")); }

            Trace.WriteLine("\nDetourBytes: \n");
            foreach (byte b in detourCodeBytes) { Trace.Write(b.ToString("X2")); }


            // Confirm that the code at ogCodeHandle is currently the original code, before we change it.
            byte[] actualCodeBytes = this.ReadWrite.ReadBytes(new ReadWrite.Pointer(ogCodeHandle));
            for (int i = 0; i < actualCodeBytes.Length; i++)
            {
                if (actualCodeBytes[i] != originalCodeBytes[i]) throw new Exception("Original Code not the correct value: corrupt game state?");
            }


            // Suspend all threads to prevent them executing the code as it's being written.
            try
            {
                PInvokes.DebugActiveProcess(pID);
                // Now write our detour code at the allocated memory address
                this.ReadWrite.WriteBytes(new ReadWrite.Pointer(detourHandle), detourCodeBytes, true);
                Trace.WriteLine("Wrote detour code to allocated: " + detourHandle.ToString("X"));
                // Then write the hook
               this.ReadWrite.WriteBytes(new ReadWrite.Pointer(ogCodeHandle), hookCodeBytes, true);
                // Resume threads
            }
            finally
            {
                PInvokes.DebugActiveProcessStop(pID);
            }
            

            // Return detourHandle so the IPersistentCheat can deallocate it when needed
            return detourHandle;
        }

    }
}
