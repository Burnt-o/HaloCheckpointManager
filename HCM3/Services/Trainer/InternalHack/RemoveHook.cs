using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using System.Diagnostics;

namespace HCM3.Services.Trainer
{
    public partial class InternalServices
    {

        public void RemoveHook()
        {
            this.HaloMemoryService.HaloState.OverlayHooked = false;

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
                //CallInternalFunction("RemoveResizeBuffersHook", ResizeBuffersPointerRes); //actually not necessary

            }
            catch (Exception ex)
            {
                Trace.WriteLine("Error removing hook: " + ex.ToString());
            }

        }

    }
}
