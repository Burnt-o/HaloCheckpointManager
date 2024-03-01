using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCMExternal.Services.DataPointersServiceNS;
using HCMExternal.ViewModels;


namespace HCMExternal.Services.CheckpointServiceNS
{
    /// <summary>
    /// Manages checkpoints in the checkpoint list, ie operations like deleting/renaming checkpoints and folders, sorting, 
    /// and decoding the checkpoint files.
    /// </summary>
    public partial class CheckpointService
    {
        public DataPointersService DataPointersService { get; init; }
        public MCCHookStateViewModel MCCHookStateViewModel { get; init; }

        public CheckpointService(DataPointersService dps, MCCHookStateViewModel hookVM) 
        {
            DataPointersService = dps;
            MCCHookStateViewModel = hookVM;
        }



    }
}
