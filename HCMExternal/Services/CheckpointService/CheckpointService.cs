using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCMExternal.Services.DataPointersServiceNS;
using HCMExternal.Services.MCCStateServiceNS;


namespace HCMExternal.Services.CheckpointServiceNS
{
    /// <summary>
    /// Manages checkpoints in the checkpoint list, ie operations like deleting/renaming checkpoints and folders, sorting, 
    /// and decoding the checkpoint files.
    /// </summary>
    public partial class CheckpointService
    {
        public MCCStateService MCCStateService { get; init; }
        public DataPointersService DataPointersService { get; init; }
        //public InternalServices InternalServices { get; init; }
        public CheckpointService(MCCStateService mps, DataPointersService dps) // TODO: Will need internalService for inject/dump
        {
            MCCStateService = mps;
            DataPointersService = dps;
        }



    }
}
