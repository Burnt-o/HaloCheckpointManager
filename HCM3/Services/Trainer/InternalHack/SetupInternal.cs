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

        public void SetupInternal(List<string> internalFunctionNames)
        {
            if (!internalFunctionNames.Any()) throw new ArgumentException("Wasn't passed a list of internal functions whose name we need to find");

            // Inject HCMInternal into our own process. 
            // We want to do this to get the function offsets of the exported functions we want to call in the DLL that's injected into MCC (they will be the same offsets both here and in MCC)

                Process HCMProcess = Process.GetCurrentProcess();
                IntPtr HCMInternalHandle = BurntMemory.DLLInjector.InjectDLL("HCMInternal.dll", HCMProcess);

            // Inject HCMInternal into MCC process

            Process MCCProcess = Process.GetProcessById((int)this.HaloMemoryService.HaloState.ProcessID);
            IntPtr MCCInternalHandle = BurntMemory.DLLInjector.InjectDLL("HCMInternal.dll", MCCProcess);

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
                IntPtr internalFunctionPointer = IntPtr.Add(MCCInternalHandle, (int)functionOffset);

                // And add to our dictionary
                InternalFunctions.Add(functionName, internalFunctionPointer);
            }
            InternalFunctionsLoaded = true;
        }


    }
}
