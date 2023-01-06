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

                IntPtr? MCCHCMHandle = null;
                IntPtr? MCCSpeedhackHandle = null;
                try
                {
                    MCCHCMHandle = BurntMemory.DLLInjector.InjectDLL("HCMInternal.dll", MCCProcess);
                    MCCSpeedhackHandle = BurntMemory.DLLInjector.InjectDLL("Speedhack.dll", MCCProcess);


                }
                catch (Exception e)
                {
                    Trace.WriteLine("Failed injection in setupInternal. This is very likely to cause issues since we won't have a handle to HCMInternal");
                }


                if (MCCHCMHandle == null || MCCSpeedhackHandle == null)
                {
                    Trace.WriteLine("Attempting handle fix");
                    System.Diagnostics.ProcessModuleCollection modules = System.Diagnostics.Process.GetProcessById((int)this.HaloMemoryService.HaloState.ProcessID).Modules;
                    foreach (System.Diagnostics.ProcessModule module in modules)
                    {
                        if (module.ModuleName != null && module.ModuleName.Contains("HCMInternal"))
                        {
                            MCCHCMHandle = module.BaseAddress;
                            Trace.WriteLine("HCMHCMHandle fixed");
                        }
                        if (module.ModuleName != null && module.ModuleName.Contains("Speedhack"))
                        {
                            MCCSpeedhackHandle = module.BaseAddress;
                            Trace.WriteLine("MCCSpeedhackHandle");
                        }
                    }
                }


                if (MCCHCMHandle == null) throw new Exception("MCCHCMHandle was null");
                if (MCCSpeedhackHandle == null) throw new Exception("MCCSpeedhackHandle was null");

                byte? MCCHCMstartByte = this.HaloMemoryService.ReadWrite.ReadByte(MCCHCMHandle);
                byte? MCCSpeedhackHandlestartByte = this.HaloMemoryService.ReadWrite.ReadByte(MCCSpeedhackHandle);
                Trace.WriteLine("Verify; MCCHCMHandle start byte: " + MCCHCMstartByte?.ToString("X") ?? "null");
                Trace.WriteLine("Verify; MCCSpeedhackHandle start byte: " + MCCSpeedhackHandlestartByte?.ToString("X") ?? "null");

                IntPtr speedhackPointerSet = PInvokes.GetProcAddress(SpeedhackInternalHandle, "setAllToSpeed");
                if (speedhackPointerSet == IntPtr.Zero) throw new Exception("Couldn't find function pointer: " + "speedhack setAllToSpeed");
                Int64 speedhackOffsetSet = (Int64)speedhackPointerSet - (Int64)SpeedhackInternalHandle;
                this.setAllToSpeed = IntPtr.Add(MCCSpeedhackHandle.Value, (int)speedhackOffsetSet);

                IntPtr speedhackPointerGet = PInvokes.GetProcAddress(SpeedhackInternalHandle, "getSpeed");
                if (speedhackPointerGet == IntPtr.Zero) throw new Exception("Couldn't find function pointer: " + "speedhack getSpeed");
                Int64 speedhackOffsetGet = (Int64)speedhackPointerGet - (Int64)SpeedhackInternalHandle;
                this.getSpeed = IntPtr.Add(MCCSpeedhackHandle.Value, (int)speedhackOffsetGet);


                Trace.WriteLine("MCCHCMHandle: " + MCCHCMHandle?.ToString("X") ?? "null");

                foreach (string functionName in internalFunctionNames)
                {
                    Trace.WriteLine("Evaluating internal function: " + functionName);
                    // Now evaluate the function offsets inside the HCMProcess's HCMInternal.dll

                    // Use GetProcAddress to find our function pointers
                    IntPtr functionPointer = PInvokes.GetProcAddress(HCMInternalHandle, functionName);
                    if (functionPointer == IntPtr.Zero) throw new Exception("Couldn't find function pointer: " + functionName);
                    // Subtract the module handle from the function pointer to get the offset.
                    // Reminder; we're doing this because the offset will be the same with the HCMInternal that gets injected into HCM
                    // (But we can only run GetProcAddress on stuff loaded in our own process)
                    Int64 functionOffset = (Int64)functionPointer - (Int64)HCMInternalHandle;
                    Trace.WriteLine("functionOffset: " + functionOffset.ToString("X"));
                    // Now we get the internal function pointer by adding functionOffset to the handle to MCC's HCMInternal.dll
                    IntPtr internalFunctionPointer = IntPtr.Add(MCCHCMHandle.Value, (int)functionOffset);
                    Trace.WriteLine("internalFunctionPointer: " + internalFunctionPointer.ToString("X"));
                    byte? startByte = this.HaloMemoryService.ReadWrite.ReadByte(internalFunctionPointer);
                    Trace.WriteLine("start byte of function: " + startByte?.ToString("X") ?? "null");

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

                if (!Properties.Settings.Default.DisableOverlay)
                {

                    // Now need to "hook" d3dgxi Present
                    Trace.WriteLine("HOOKING PRESENT POINTER - IF THIS GOES WRONG, MCC WILL CRASH");
                    IntPtr? resolvedPresentPtr = this.HaloMemoryService.ReadWrite.ResolvePointer(presentPtr);
                    Trace.WriteLine("Address of present pointer: " + NullableIntPtrToHexString(resolvedPresentPtr));
                    ulong? valueOfOriginalPresent = this.HaloMemoryService.ReadWrite.ReadQword(resolvedPresentPtr);
                    Trace.WriteLine("Value of original present: " + (valueOfOriginalPresent != null ? valueOfOriginalPresent.Value.ToString("X") : "null"));
                    IntPtr hookedPresent = InternalFunctions["hkPresent"];
                    Trace.WriteLine("Address of hkPresent (allegedly): " + NullableIntPtrToHexString(hookedPresent));

                    if (valueOfOriginalPresent != null && valueOfOriginalPresent.Value == (ulong)hookedPresent.ToInt64())
                    {
                        Trace.WriteLine("Hook appears to already be applied,  no need to apply it again: bailing");
                        return;
                    }


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
                    System.Threading.Thread.Sleep(50);
                    Trace.WriteLine("Unpausing MCC process");
                }
                else
                {
                    try
                    {
                        Trace.WriteLine("Attempting to remove hook on user request");
                        ReadWrite.Pointer? PresentPointer = (ReadWrite.Pointer)DataPointersService.GetPointer("PresentPointer_" + HaloMemoryService.HaloState.MCCType, HaloMemoryService.HaloState.CurrentAttachedMCCVersion);
                        if (PresentPointer == null) throw new Exception("Couldn't get PresentPointer");
                        IntPtr? PresentPointerResPtr = HaloMemoryService.ReadWrite.ResolvePointer(PresentPointer);
                        if (PresentPointerResPtr == null) throw new Exception("Couldn't resolve PresentPointer");
                        UInt64 PresentPointerRes = (UInt64)PresentPointerResPtr.Value.ToInt64();
                        Trace.WriteLine("DISABLING PRESENT HOOK: resolved present point: " + PresentPointerRes.ToString("X"));
                        CallInternalFunction("RemoveHook", PresentPointerRes);
                    }
                    catch (Exception ex)
                    {
                        Trace.WriteLine("Error removing hook: " + ex.ToString());
                    }

                }


                


                string NullableIntPtrToHexString(IntPtr? ptr)
                {
                    if (ptr == null) return "Null";
                    return ptr.Value.ToString("X");
                }

            }
        }

    }
}
