using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace HCM3.Services.Internal
{
    /// <summary>
    /// Pointer to camera and how to interpret it, for passing to HCMInternal
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct CameraDataInfo
    {
        public UInt64 pCamera;
        public UInt64 pTickCounter;
        public Int64 OffsetCameraPosition;
        public Int64 OffsetCameraDirection;
        public Int64 OffsetCameraVerticalFOV;
        public Int64 OffsetCameraTestValue;
        public UInt32 CameraTestValue;



    }

}
