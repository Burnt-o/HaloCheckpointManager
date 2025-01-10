using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.Services.External.FileMapping
{
    public interface IMappedCartographerSave
    {
        public IntPtr data(); // needs to be marshalled if you would like to make a copy
        public UInt32 size();
    }
}
