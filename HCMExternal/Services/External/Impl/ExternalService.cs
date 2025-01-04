using HCMExternal.Models;
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

        private Process? FindProcess(string processName)
        {
            Process[] ProcessList = Process.GetProcesses();
            foreach (Process proc in ProcessList)
            {
                //Needs to ignore case woo
                if (proc.ProcessName.Equals(processName, StringComparison.OrdinalIgnoreCase))
                {
                    return proc;
                }
            }
            return null;
        }

        ProcessModule FindModule(Process process, string moduleName)
        {
            foreach (ProcessModule processModule in process.Modules )
            {
                if (processModule.ModuleName == moduleName)
                    return processModule;
            }
            throw new Exception("Could not find process module of name: " + moduleName);
        }

        HaloProcessInfo GetHaloProcessInfo()
        {
            // try to find cartographer process
            // then mccsteam
            // then mccwinstore
            // if neither then throw

            HaloProcessType processType = HaloProcessType.ProjectCartographer;
            Process? haloProc = FindProcess("halo2");

            if (haloProc == null)
            {
                processType = HaloProcessType.MCCSteam;
                haloProc = FindProcess("MCC-Win64-Shipping");
            }

            if (haloProc == null)
            {
                processType = HaloProcessType.MCCWinStore;
                haloProc = FindProcess("MCCWinStore-Win64-Shipping");
            }

            if (haloProc == null)
                throw new Exception("Could not find halo game process");

            Log.Information("Found Halo Process of type: " + processType);

            // Figure out version info.
            // MCC you just check the process FileVersionInfo.
            // Project cartographer you check the xlive.dll module FileVersionInfo
            // If an error is thrown here, use LatestSupportedVersion (depending on process type)
            string? versionInfo = null;
            try
            {
                switch (processType)
                {
                    case HaloProcessType.MCCSteam:
                        versionInfo = haloProc.MainModule.FileVersionInfo.ToVerString();
                        break;
                    case HaloProcessType.MCCWinStore:
                        // leave null, we will assume highest version because you can't downpatch winstore.
                        break;
                    case HaloProcessType.ProjectCartographer:
                        versionInfo = FindModule(haloProc, "xlive.dll")?.FileVersionInfo.ToVerString();
                        break;
                }

            }
            catch (Exception ex)
            {
                Log.Error("Failed to find version info of processType: " + processType + ", using LatestSupportedVersion from pointer data instead. Error: " + ex.Message);
            }



            if (versionInfo == null)
            {
                switch (processType)
                {
                    default:  // fallthrough
                    case HaloProcessType.MCCSteam:
                    case HaloProcessType.MCCWinStore:
                        versionInfo = PointerData.HighestSupportedMCCVersion;
                        break;

                    case HaloProcessType.ProjectCartographer:
                        versionInfo = PointerData.HighestSupportedCartographerVersion;
                        break;

                }
            }

            Log.Information("Version Info string: " + versionInfo);

            HaloGame? currentlyPlayingHaloGame = null;

            try
            {
                if (processType == HaloProcessType.ProjectCartographer)
                    currentlyPlayingHaloGame = HaloGame.ProjectCartographer;
                else
                    throw new NotImplementedException();
            }
            catch(Exception ex)
            {
                Log.Error("Failed to figure out what halo game was currently playing in mcc, error: " + ex.Message);
                currentlyPlayingHaloGame = (HaloGame)Properties.Settings.Default.LastSelectedGameTab;
            }


            return new HaloProcessInfo() { haloGame = currentlyPlayingHaloGame ?? throw new Exception("how, even?"), processHandle = haloProc, processType = processType, processVersion = versionInfo };
        }



    }
}
