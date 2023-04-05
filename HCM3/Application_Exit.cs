using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using HCM3.Services;
using HCM3.Services.Cheats;
using HCM3.Services.HaloMemory;

using Microsoft.Extensions.DependencyInjection;
using BurntMemory;
using System.Diagnostics;
using HCM3.Services.Internal;
namespace HCM3
{
    public partial class App : Application
    {
        private void Application_Exit(object? sender, ExitEventArgs? e)
        {
            Trace.WriteLine("Attempting application exit");
            bool shouldRemoveOverlay = true;
            try
            {
                var CheatManagerService = _serviceProvider.GetService<CheatManagerService>();




                foreach (KeyValuePair<string, ICheat> kvp in CheatManagerService.ListOfCheats)
                {
                    try
                    {
                        if (kvp.Value.GetType().BaseType == typeof(AbstractToggleCheat))
                        {
                            if (((AbstractToggleCheat)kvp.Value).IsCheatApplied()) // Don't bother trying to remove it if it's not applied
                            {
                                ((AbstractToggleCheat)kvp.Value).RemoveCheat(); // Remove it
                                if (((AbstractToggleCheat)kvp.Value).IsCheatApplied()) // Did we fail? if so then we won't remove overlay
                                {
                                    shouldRemoveOverlay = false; // Don't remove overlay if we failed to remove one of the cheats
                                }
                            }
 
                        }


                    }
                    catch (Exception ex)
                    {
                        Trace.WriteLine("Weird error while looping over abstracttogglecheats to remove, " + ex.ToString());
                    }
                    
                }


                var HaloMemoryService = _serviceProvider.GetService<HaloMemoryService>();
                HaloMemoryService.PatchManager.RemoveAllPatches();

            }
            catch (Exception ex)
            {
                Trace.WriteLine("Failed removing cheats or patches - probably MCC was closed. ex: " + ex.ToString());
            }

            var HotkeyManager = _serviceProvider.GetService<HotkeyManager>();
            HotkeyManager.SerializeBindings();


            if (shouldRemoveOverlay)
            {
                try
                {
                    var HaloMemoryService = _serviceProvider.GetService<HaloMemoryService>();

                    var DataPointersService = _serviceProvider.GetService<DataPointersService>();
                    ReadWrite.Pointer? PresentPointer = (ReadWrite.Pointer)DataPointersService.GetPointer("PresentPointer_" + HaloMemoryService.HaloMemoryManager.MCCType, HaloMemoryService.HaloMemoryManager.CurrentAttachedMCCVersion);
                    if (PresentPointer == null) throw new Exception("Couldn't get PresentPointer");
                    IntPtr? PresentPointerResPtr = HaloMemoryService.ReadWrite.ResolvePointer(PresentPointer);
                    if (PresentPointerResPtr == null) throw new Exception("Couldn't resolve PresentPointer");
                    UInt64 PresentPointerRes = (UInt64)PresentPointerResPtr.Value.ToInt64();
                    Trace.WriteLine("DISABLING PRESENT HOOK: resolved present point: " + PresentPointerRes.ToString("X"));
                    var InternalServices = _serviceProvider.GetService<InternalServices>();
                    InternalReturn disableOverlayReturn = InternalServices.CallInternalFunction("DisableOverlay", PresentPointerRes);
                    Trace.WriteLine($"DisableOverlay returned {disableOverlayReturn}");
                    Trace.WriteLine("Succesfully exited application");
                }
                catch (Exception ex)
                {
                    Trace.WriteLine("error shutting down: " + ex.ToString());
                }
            }



            this.Logger.Flush();
        }
    }
}
