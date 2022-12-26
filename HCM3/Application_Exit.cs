﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using HCM3.Services;
using HCM3.Services.Trainer;
using Microsoft.Extensions.DependencyInjection;
using BurntMemory;
using System.Diagnostics;
namespace HCM3
{
    public partial class App : Application
    {
        private void Application_Exit(object? sender, ExitEventArgs? e)
        {
            this.Logger.Flush();

            var PersistentCheatService = _serviceProvider.GetService<PersistentCheatService>();
            PersistentCheatService.RemoveAllCheats();

            var TrainerServices = _serviceProvider.GetService<TrainerServices>();
            TrainerServices.RemoveAllPatches();

            var HotkeyManager = _serviceProvider.GetService<HotkeyManager>();
            HotkeyManager.SerializeBindings();


            try
            {
                var HaloMemoryService = _serviceProvider.GetService<HaloMemoryService>();

                var DataPointersService = _serviceProvider.GetService<DataPointersService>();
                ReadWrite.Pointer PresentPointer = (ReadWrite.Pointer)DataPointersService.GetPointer("PresentPointer", HaloMemoryService.HaloState.CurrentAttachedMCCVersion);
                UInt64 PresentPointerRes = (UInt64)HaloMemoryService.ReadWrite.ResolvePointer(PresentPointer).Value.ToInt64();
                Trace.WriteLine("DISABLING PRESENT HOOK: resolved present point: " + PresentPointerRes.ToString("X"));
                var InternalServices = _serviceProvider.GetService<InternalServices>();
                InternalServices.CallInternalFunction("RemoveHook", PresentPointerRes);
            }
            catch
            { }

        }
    }
}