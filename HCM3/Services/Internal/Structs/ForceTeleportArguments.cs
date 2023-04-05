using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace HCM3.Services.Internal
{
    
    /// <summary>
    /// Set of arguments passed to HCMInternals ForceTeleport function
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ForceTeleportArguments
    {
        public UInt32 TeleportModeForward; // actually a bool
        public UInt32 TeleportIgnoreZ; // actually a bool
        public float TeleportX;
        public float TeleportY;
        public float TeleportZ;
        public float TeleportLength;


    }
}
