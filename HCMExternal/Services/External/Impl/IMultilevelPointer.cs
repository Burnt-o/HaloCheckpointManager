using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.Services.PointerData
{
    public interface IMultilevelPointer
    {

        // Given a process, resolves a multilevel pointer.
        // Throws exceptions on resolution failure. Ie bad offsets, bad pointer read, no perms, etc
        IntPtr Resolve(Process process);

        void writeData(Process process, byte[] data, bool protectedMemory = false);

        byte[] readData(Process process, int dataLength);



    }
}
