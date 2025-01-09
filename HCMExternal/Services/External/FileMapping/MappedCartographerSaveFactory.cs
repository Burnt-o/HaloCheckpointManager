using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCMExternal.Services.External.FileMapping.Impl;
using System.Diagnostics;

namespace HCMExternal.Services.External.FileMapping
{
    public static class MappedCartographerSaveFactory
    {
        public static IMappedCartographerSave make(UInt32 saveFileHandle, Process cartographerProcess, int expectedSize)
        {
            return new MappedCartographerSave(saveFileHandle, cartographerProcess, expectedSize);
        }
    }
}
