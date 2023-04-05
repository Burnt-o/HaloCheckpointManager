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
    public struct TriggerDataInfo
    {
		public UInt64 pFirstTrigger; // Pointer to the first trigger
		public UInt64 TriggerStride; // How many bytes between each trigger 0x60 for ce (assembly tells ya)
		public UInt64 TriggerCount; // How many triggers there are


		// Offsets for reading the trigger data
		public Int32 OffsetTriggerName;
		public UInt64 TriggerNameLength;
		public Int32 OffsetTriggerCenterPosition;
		public Int32 OffsetTriggerExtent;
		public Int32 OffsetTriggerRotation;
		public Int32 OffsetTriggerUpVector;

		// For testing validity
		public Int32  OffsetTriggerTestValue;
		public UInt32 TriggerTestValue;

		// For trigger search hooking
		public UInt64 Tickcounter;
		public UInt64 pTriggerHitTest;

	}

}
