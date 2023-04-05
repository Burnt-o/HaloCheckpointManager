using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using System.Diagnostics;
using HCM3.Services.HaloMemory;
using HCM3.Helpers;

namespace HCM3.Services.Cheats
{
    internal class GenericDetourCheat : AbstractToggleCheat
    {


        public GenericDetourCheat(List<string> detourPointerNames, string enableMessage, string disableMessage, string activeMessage, ServicesGroup servicesGroup) : base(servicesGroup) 
        { 
        DetourPointerNames = detourPointerNames;
            EnabledMessage = enableMessage;
            DisabledMessage = disableMessage;
            ActiveMessage = activeMessage;
            DetourHandles = new();
        }

        public List<string> DetourPointerNames;

        public Dictionary<string, IntPtr?> DetourHandles;

        // Apply cheat throws on failure
        public override void ApplyCheat(HaloState? haloState = null)
        {
            if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
            RemoveCheat(haloState); // To make sure handles are deallocated

                foreach (var DetourPointerName in DetourPointerNames)
                {
                    DetourInfoObject detourInfo = (DetourInfoObject)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_{DetourPointerName}", haloState);
                    DetourHandles.Add(DetourPointerName, this.HaloMemoryService.DetourApply(detourInfo));
                }
        }


        public override void RemoveCheat(HaloState? haloState = null)
        {
            if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
            foreach (var DetourPointerName in DetourPointerNames)
            {
                try
                {
                    DetourInfoObject? detourInfo = (DetourInfoObject?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_{DetourPointerName}", haloState);
                    if (detourInfo == null) throw new Exception("detourInfo was null"); // no pointer so cheat isn't active anyway
                    this.HaloMemoryService.DetourRemove(detourInfo, DetourHandles[DetourPointerName]);
                    DetourHandles.Remove(DetourPointerName);
                }
                catch (Exception ex) { Trace.WriteLine($"Failed removing detour {DetourPointerName}, " + ex.ToString());  }

            }
            DetourHandles.Clear();

        }


        public override bool IsCheatApplied(HaloState? haloState = null)
        {
            try
            {
                if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
                string DetourPointerName = DetourPointerNames.FirstOrDefault(); // Just test the first detour

                DetourInfoObject? detourInfo = (DetourInfoObject?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_{DetourPointerName}", haloState);
                if (detourInfo == null)
                {
                    Trace.WriteLine("GenericDetourCheat.IsCheatApplied: detourInfo was null so returning false"); 
                    return false;
                }
                    
                byte[]? actualCode = this.HaloMemoryService.ReadWrite.ReadBytes(detourInfo.OriginalCodeLocation, detourInfo.OriginalCodeBytes.Length);
                if (actualCode == null)
                {
                    Trace.WriteLine("GenericDetourCheat.IsCheatApplied: couldn't read bytes at og code, game not loaded? this shouldn't happen, I think. returning false.");
                    return false;
                }


                for (int i = 0; i < actualCode.Length; i++)
                {
                    // If the code at hook location doesn't match the original bytes and is not zero, then cheat is probably still active
                    if (actualCode[i] != detourInfo.OriginalCodeBytes[i] && actualCode[i] != 0) return true;
                }
                return false;

            }
            catch (Exception ex)
            {
                Trace.WriteLine("GenericDetourCheat.IsCheatApplied error: " + ex.ToString());
                return false;
            }

        }



    }
}
