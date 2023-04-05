using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCM3.Services.HaloMemory
{
    public partial class HaloMemoryService
    {
        public bool DetourCheckOG(DetourInfoObject detourInfo)
        {
            byte[] originalCode = detourInfo.OriginalCodeBytes;

            byte[] actualBytes = this.ReadWrite.ReadBytes(detourInfo.OriginalCodeLocation, originalCode.Length);

            for (int i = 0; i < actualBytes.Length; i++)
            {
                if (originalCode[i] != actualBytes[i]) return false;
            }


            return true;
        }

    }
}
