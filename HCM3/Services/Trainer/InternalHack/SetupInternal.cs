using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using BurntMemory;

namespace HCM3.Services.Trainer
{
    public partial class InternalServices
    {

        private object lockSetupInternal = new object();
        public void SetupInternal(List<string> internalFunctionNames)
        {
            lock (lockSetupInternal)
            {
                Trace.WriteLine("INJECTING INTERNAL");

                ReadWrite.Pointer presentPtr = (ReadWrite.Pointer)this.DataPointersService.GetPointer("PresentPointer_" + this.HaloMemoryService.HaloState.MCCType, this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion);
              

                if (!internalFunctionNames.Any()) throw new ArgumentException("Wasn't passed a list of internal functions whose name we need to find");

                // Inject HCMInternal into our own process. 
                // We want to do this to get the function offsets of the exported functions we want to call in the DLL that's injected into MCC (they will be the same offsets both here and in MCC)

                Process HCMProcess = Process.GetCurrentProcess();
                IntPtr HCMInternalHandle = BurntMemory.DLLInjector.InjectDLL("HCMInternal.dll", HCMProcess);
                IntPtr SpeedhackInternalHandle = BurntMemory.DLLInjector.InjectDLL("Speedhack.dll", HCMProcess);

                // Inject HCMInternal into MCC process
                uint pID = this.HaloMemoryService.HaloState.ProcessID ?? throw new Exception("Couldn't get process ID");
                Process MCCProcess = Process.GetProcessById((int)pID);

                IntPtr MCCHCMHandle;
                IntPtr MCCSpeedhackHandle;
                try
                {
                    //PInvokes.DebugActiveProcess(pID);
                    MCCHCMHandle = BurntMemory.DLLInjector.InjectDLL("HCMInternal.dll", MCCProcess);
                    MCCSpeedhackHandle = BurntMemory.DLLInjector.InjectDLL("Speedhack.dll", MCCProcess);


                }
                finally
                {
                    //SO turns out pausing the process threads will cause an error, something to do with not being able to access nt.dll 
                    //PInvokes.DebugActiveProcessStop(pID);
                }


                IntPtr speedhackPointerSet = PInvokes.GetProcAddress(SpeedhackInternalHandle, "setAllToSpeed");
                if (speedhackPointerSet == IntPtr.Zero) throw new Exception("Couldn't find function pointer: " + "speedhack setAllToSpeed");
                Int64 speedhackOffsetSet = (Int64)speedhackPointerSet - (Int64)SpeedhackInternalHandle;
                this.setAllToSpeed = IntPtr.Add(MCCSpeedhackHandle, (int)speedhackOffsetSet);

                IntPtr speedhackPointerGet = PInvokes.GetProcAddress(SpeedhackInternalHandle, "getSpeed");
                if (speedhackPointerGet == IntPtr.Zero) throw new Exception("Couldn't find function pointer: " + "speedhack getSpeed");
                Int64 speedhackOffsetGet = (Int64)speedhackPointerGet - (Int64)SpeedhackInternalHandle;
                this.getSpeed = IntPtr.Add(MCCSpeedhackHandle, (int)speedhackOffsetGet);

                foreach (string functionName in internalFunctionNames)
                {
                    // Now evaluate the function offsets inside the HCMProcess's HCMInternal.dll

                    // Use GetProcAddress to find our function pointers
                    IntPtr functionPointer = PInvokes.GetProcAddress(HCMInternalHandle, functionName);
                    if (functionPointer == IntPtr.Zero) throw new Exception("Couldn't find function pointer: " + functionName);

                    // Subtract the module handle from the function pointer to get the offset.
                    // Reminder; we're doing this because the offset will be the same with the HCMInternal that gets injected into HCM
                    // (But we can only run GetProcAddress on stuff loaded in our own process)
                    Int64 functionOffset = (Int64)functionPointer - (Int64)HCMInternalHandle;

                    // Now we get the internal function pointer by adding functionOffset to the handle to MCC's HCMInternal.dll
                    IntPtr internalFunctionPointer = IntPtr.Add(MCCHCMHandle, (int)functionOffset);

                    // And add to our dictionary
                    if (InternalFunctions.ContainsKey(functionName))
                    {
                        InternalFunctions[functionName] = internalFunctionPointer;
                    }
                    else
                    {
                        InternalFunctions.Add(functionName, internalFunctionPointer);
                    }
                    
                }
                InternalFunctionsLoaded = true;


                // Now need to "hook" d3dgxi Present
                Trace.WriteLine("HOOKING PRESENT POINTER - IF THIS GOES WRONG, MCC WILL CRASH");
                IntPtr? resolvedPresentPtr = this.HaloMemoryService.ReadWrite.ResolvePointer(presentPtr);
                Trace.WriteLine("Address of present pointer: " + NullableIntPtrToHexString(resolvedPresentPtr));
                ulong? valueOfOriginalPresent = this.HaloMemoryService.ReadWrite.ReadQword(resolvedPresentPtr);
                Trace.WriteLine("Value of original present: " + (valueOfOriginalPresent != null ? valueOfOriginalPresent.Value.ToString("X") : "null"));
                IntPtr hookedPresent = InternalFunctions["hkPresent"];
                Trace.WriteLine("Address of hkPresent (allegedly): " + NullableIntPtrToHexString(hookedPresent));



                IntPtr? hcmInternal = null;

                try
                {

                    System.Diagnostics.ProcessModuleCollection modules = System.Diagnostics.Process.GetProcessById((int)this.HaloMemoryService.HaloState.ProcessID).Modules;
                    foreach (System.Diagnostics.ProcessModule module in modules)
                    {
                        if (module.ModuleName != null && module.ModuleName.Contains("HCMInternal"))
                        {
                            hcmInternal = module.BaseAddress;
                            break;
                        }
                    }
                }
                catch (Exception e)
                { 
                }

                IntPtr? verifyHook = null;
                if (hcmInternal != null)
                {
                    verifyHook = IntPtr.Add(hcmInternal.Value, 0x2C3C0);
                }
                Trace.WriteLine("verifyHook: " + NullableIntPtrToHexString(verifyHook));
                byte? startofhook1 = this.HaloMemoryService.ReadWrite.ReadByte(hookedPresent);
                byte? startofhook2 = this.HaloMemoryService.ReadWrite.ReadByte(verifyHook);

                if (hookedPresent != verifyHook || startofhook1 != 0x40 || startofhook2 != 0x40)
                {
                    Trace.WriteLine("We got a serious issue");

                    Trace.WriteLine("startofhook1: " + startofhook1);
                    Trace.WriteLine("startofhook2: " + startofhook2);

                    //Attempt at a fix
                    if (startofhook2 == 0x40)
                    {
                        Trace.WriteLine("Attempting fix, change hookPresent address");
                        hookedPresent = verifyHook.Value;
                    }
                }

                PInvokes.DebugActiveProcess(pID);
                Trace.WriteLine("Pausing MCC process");
                this.HaloMemoryService.ReadWrite.WriteQword(presentPtr, (ulong)hookedPresent.ToInt64(), true);
                PInvokes.DebugActiveProcessStop(pID);
                Trace.WriteLine("Unpausing MCC process");




                string NullableIntPtrToHexString(IntPtr? ptr)
                {
                    if (ptr == null) return "Null";
                    return ptr.Value.ToString("X");
                }

            }
        }

    }
}
