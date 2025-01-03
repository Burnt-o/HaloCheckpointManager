using HCMExternal.Services.PointerData;
using HCMExternal.ViewModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.Services.CheckpointIO.Impl
{

    /// <summary>
    /// Manages checkpoints in the checkpoint list, ie operations like deleting/renaming checkpoints and folders, sorting, 
    /// and decoding the checkpoint files.
    /// </summary>
    public partial class CheckpointIOService : ICheckpointIOService
    {
        public IPointerDataService PointerData { get; init; }

        public CheckpointIOService(IPointerDataService dps)
        {
            PointerData = dps;
        }

    }
}
