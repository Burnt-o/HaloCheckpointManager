using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCMExternal.Models;

namespace HCMExternal.Services.External
{
    public interface IExternalService
    {
        void ForceCheckpoint();
        void ForceRevert();
        void ForceDoubleRevert();
        void DumpCheckpoint(SaveFolder saveFolder);
        void InjectCheckpoint(Checkpoint checkpoint);
    }
}
