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


/*                ReadWrite.Pointer? ResizeBuffersPointer = (ReadWrite.Pointer)DataPointersService.GetPointer("ResizeBuffersPointer_" + HaloMemoryService.HaloState.MCCType, HaloMemoryService.HaloState.CurrentAttachedMCCVersion);
                if (ResizeBuffersPointer == null) throw new Exception("Couldn't get ResizeBuffersPointer");
                IntPtr? ResizeBuffersPointerResPtr = HaloMemoryService.ReadWrite.ResolvePointer(ResizeBuffersPointer);
                if (ResizeBuffersPointerResPtr == null) throw new Exception("Couldn't resolve ResizeBuffersPointerResPtr");
                UInt64 ResizeBuffersPointerRes = (UInt64)ResizeBuffersPointerResPtr.Value.ToInt64();
                Trace.WriteLine("DISABLING RESIZEBUFFERS HOOK: resolved resizeBuffers point: " + ResizeBuffersPointerRes.ToString("X"));
*/


                CallInternalFunction("RemovePresentHook", PresentPointerRes);
                //CallInternalFunction("RemoveResizeBuffersHook", ResizeBuffersPointerRes); //actually not necessary

            }
            catch (Exception ex)
            {
                Trace.WriteLine("Error removing hook: " + ex.ToString());
            }

        }

    }
}
