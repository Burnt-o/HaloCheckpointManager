using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using BurntMemory;

namespace HCM3.Services.Internal
{
    public partial class InternalServices
    {

        // We load HCMInternal into both MCC and HCMExternal processes (this). We use GetProcAddress to find the function pointers in this process,
        // subtract it from our processes DLL handle, then add it to the MCC-HCMInternal handle to get the address of the function pointers in MCC.
        private bool FindInternalFunctions(IntPtr MCC_HCMInternalHandle)
        {
            try
            {
                this.HCMInternalFunctionPointers.Clear();

                Process MCCProcess = Process.GetProcessById((int)this.HaloMemoryService.HaloMemoryManager.ProcessID);
                Process OurProcess = Process.GetCurrentProcess();

                IntPtr? Our_HCMInternalHandle = null;
                // Check if we've already injected HCMInternal into our process
                foreach (ProcessModule module in OurProcess.Modules)
                {
                    if (module.ModuleName == "HCMInternal.dll")
                    {
                        Our_HCMInternalHandle = module.BaseAddress;
                        break;
                    }
                }

                // If we haven't, inject it and grab the handle
                if (Our_HCMInternalHandle == null) 
                {
                    Our_HCMInternalHandle = BurntMemory.DLLInjector.InjectDLL("HCMInternal.dll", OurProcess);
                }

                Trace.WriteLine("Our_HCMInternalHandle: " + Our_HCMInternalHandle.Value.ToString("X"));
                Trace.WriteLine("MCC_HCMInternalHandle: " + HCMInternalHandle.Value.ToString("X"));

                // Loop over each functionName that we want to find
                foreach(string functionName in this.HCMInternalFunctions)
                {
                    Trace.WriteLine("Evaluating internal function: " + functionName);

                    // Use GetProcAddress to find function pointers of the HCMInternal in OUR process
                    IntPtr functionPointer = PInvokes.GetProcAddress(Our_HCMInternalHandle.Value, functionName);
                    if (functionPointer == IntPtr.Zero) throw new Exception("Couldn't find function pointer: " + functionName);

                    Trace.WriteLine("functionPointer for our_HCM is " + functionPointer.ToString("X"));

                    // Subtract the module handle from the function pointer to get the offset.
                    // Reminder; we're doing this because the offset will be the same with the HCMInternal that gets injected into HCM
                    // (But we can only run GetProcAddress on stuff loaded in our own process)
                    Int64 functionOffset = (Int64)functionPointer - (Int64)Our_HCMInternalHandle;
                    Trace.WriteLine("functionOffset: " + functionOffset.ToString("X"));

                    // Now we get the internal function pointer by adding functionOffset to the handle to MCC's HCMInternal.dll
                    IntPtr internalFunctionPointer = IntPtr.Add(MCC_HCMInternalHandle, (int)functionOffset);
                    Trace.WriteLine("internalFunctionPointer: " + internalFunctionPointer.ToString("X"));

                    // Check that it's valid
                    byte? startByte = this.HaloMemoryService.ReadWrite.ReadByte(internalFunctionPointer);
                    Trace.WriteLine("start byte of function: " + startByte?.ToString("X") ?? "null");
                    if (startByte == null) throw new Exception("Got invalid function pointer for function: " + functionName);

                    // Add it to our dictionary
                    if (HCMInternalFunctionPointers.ContainsKey(functionName))
                    {
                        HCMInternalFunctionPointers[functionName] = internalFunctionPointer;
                    }
                    else
                    {
                        HCMInternalFunctionPointers.Add(functionName, internalFunctionPointer);
                    }

                }
                return true;

            }
            catch (Exception ex)
            {
                Trace.WriteLine("Failed finding internal functions, " + ex.ToString());
                this.HCMInternalFunctionPointers.Clear();
                return false;
            }

        }


        }
    }
