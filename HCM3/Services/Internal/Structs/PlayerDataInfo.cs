using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace HCM3.Services.Internal
{
    /// <summary>
    /// Contains the address of the PlayerData detour, and offsets for the data that is stored there.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct PlayerDataInfo
    {
        // Pointers
        public UInt64 pPlayerDatum = 0;
        public UInt64 pPlayerVehiDatum = 0;
        public UInt64 pTickCounter = 0;
        public UInt64 pViewHorizontal = 0;
        public UInt64 pViewVertical = 0;

        public UInt64 pTestValue = 0;
        public UInt32 TestValue = 0;

        public Int64 CurrentGame = -1;

        // How to read the data at the PlayerVehiAddress
        // Is populated by PlayerDataOffsetData via PointerData
        public Int64 OffsetEntityTestValue;
        public UInt32 EntityTestValue;

        public Int64 OffsetEntityPosition;
        public Int64 OffsetEntityVelocity;
        public Int64 OffsetEntityHealth;
        public Int64 OffsetEntityShield;
        public Int64 OffsetPlayerVehicleDatum; // H1 use only
        public Int64 OffsetEntityTrigPosition;




    }

}
