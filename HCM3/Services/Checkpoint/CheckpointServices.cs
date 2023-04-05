using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Services.Cheats;
using HCM3.Services.HaloMemory;
using HCM3.Helpers;
using HCM3.Services.Internal;

namespace HCM3.Services
{
    public partial class CheckpointServices
    {
        public HaloMemoryService HaloMemoryService { get; init; }
        public DataPointersService DataPointersService { get; init; }
        public InternalServices InternalServices { get; init; }
        public CheckpointServices(HaloMemoryService haloMemoryService, DataPointersService dataPointersService, InternalServices internalServices)
        { 
        this.HaloMemoryService = haloMemoryService;
        this.DataPointersService = dataPointersService;
            this.InternalServices = internalServices;
        }


        //public void CheckGameIsAligned(int selectedGame)
        //{
        //    // Update HaloState
        //    this.HaloMemoryService.HaloState.UpdateHaloState();

        //    // Check that we're loaded into the game that matches the tab whose checkpoint we're trying to dump
        //    Dictionaries.HaloStateEnum game = (Dictionaries.HaloStateEnum)this.HaloMemoryService.HaloState.CurrentHaloState;

        //    if ((int)game != selectedGame)
        //    {
        //        throw new InvalidOperationException("HCM didn't detect that you were in the right game: \n" +
        //            "Expected: " + Dictionaries.GameToDLLname[selectedGame] + "\n" +
        //            "Actual: " + Dictionaries.GameToDLLname[(int)game] + "\n" +
        //            "Are you sure you have the right tab selected in HCM?"
        //            );
        //    }


        //    string? MCCversion = this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion;

        //    if (MCCversion == null)
        //    {
        //        throw new InvalidOperationException("HCM couldn't detect which version of MCC was running");
        //    }
        //}

    }
}
