﻿using HCMExternal.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCMExternal.Services.PointerData;
using System.Diagnostics;
using System.Printing.IndexedProperties;
using System.Runtime.InteropServices;
using Serilog;
using System.Windows.Navigation;

namespace HCMExternal.Services.External.Impl
{
    internal partial class ExternalService : IExternalService
    {
        // TODO: setup pointers and reusable utility functions here

        IPointerDataService PointerData { get; init; }
        public ExternalService(IPointerDataService pointerData)
        {
            PointerData = pointerData;
        }

        struct HaloProcessInfo
        {
            public Process processHandle { get; init; } // handle for doing write/read mem
            public HaloProcessType processType; // for pointer data lookup
            public HaloGame haloGame; // current HaloGame. if no game loaded, error will be thrown.
            public string processVersion; // "1.3385.0.0" etc. if we can't figure it out, null, and we'll use LatestSupportedVersion. 
        }

        ProcessModule? FindModule(Process process, string moduleName)
        {
            foreach (ProcessModule processModule in process.Modules )
            {
                if (processModule.ModuleName == moduleName)
                    return processModule;
            }
            return null;
        }

        bool is_host(HaloProcessInfo haloProcess)
        {
            if (haloProcess.processType != HaloProcessType.ProjectCartographer)
                throw new Exception("Only project cartographer supported");




            // get pointer to is_host enum
            IMultilevelPointer p_is_host_enum = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, "is_host_enum", haloProcess.processVersion);
            byte is_host_enum = p_is_host_enum.readData(haloProcess.processHandle, 1)[0];

            return (is_host_enum) switch
            {
                (0) => true, // Not in network session
                (3) => false, // is NOT network session host
                (5) => true, // is network session host
                _ => throw new Exception("Invalid value for is_host_enum: " + (int)is_host_enum)
            };




        }

        // try to find cartographer, mccsteam, and mccwinstore processes. throws if it finds none.
        List<HaloProcessInfo> GetHaloProcessInfo()
        {


            List < HaloProcessInfo > haloProcessInfoOut = new();

            List<string> haloProcessNames = new() { "halo2", "MCC-Win64-Shipping", "MCCWinStore-Win64-Shipping" };

            foreach (var haloProcess in Process.GetProcesses().Where(proc => haloProcessNames.Any(s=>s.Equals(proc.ProcessName, StringComparison.OrdinalIgnoreCase))))
            {
                HaloProcessType processType = (haloProcess.ProcessName) switch
                {
                    ("halo2") => HaloProcessType.ProjectCartographer,
                    ("MCC-Win64-Shipping") => HaloProcessType.MCCSteam,
                    ("MCCWinStore-Win64-Shipping") => HaloProcessType.MCCWinStore,
                    _ => throw new Exception("invalid haloProcess name")
                };


                var newHaloProcessInfo = new HaloProcessInfo()
                {
                    processHandle = haloProcess,
                    processType = processType,
                    haloGame = processType == HaloProcessType.ProjectCartographer ? HaloGame.ProjectCartographer : throw new NotImplementedException("Only project cartographer supported"),
                    processVersion = (processType) switch
                    {
                        (HaloProcessType.ProjectCartographer) => FindModule(haloProcess, "xlive.dll")?.FileVersionInfo.ToVerString() ?? PointerData.HighestSupportedCartographerVersion,
                        (HaloProcessType.MCCSteam) => haloProcess.MainModule?.FileVersionInfo.ToVerString() ?? PointerData.HighestSupportedMCCVersion,
                        (HaloProcessType.MCCWinStore) => PointerData.HighestSupportedMCCVersion, // can't downpatch winstore
                        _ => throw new Exception("invalid HaloProcessType enum")
                    }
                };

                if (is_host(newHaloProcessInfo))
                    haloProcessInfoOut.Add(newHaloProcessInfo);
                else
                    Log.Debug("Skipping non-host process");
               
            }


            if (haloProcessInfoOut.Count() == 0)
                throw new Exception("Could not find any halo processes");
            else
                Log.Information(string.Format("Found {0} halo porcesses", haloProcessInfoOut.Count));

            return haloProcessInfoOut;
        }



    }
}
