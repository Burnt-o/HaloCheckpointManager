using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Helpers;
using BurntMemory;
using System.Diagnostics;

namespace HCM3.Services
{
    public partial class CommonServices
    {

        public IntPtr GetAddressFromDatum(uint playerDatum)
        {
            switch (this.HaloMemoryService.HaloState.CurrentHaloState)
            {
                case (int)Dictionaries.HaloStateEnum.Halo1:
                    ReadWrite.Pointer EntityOffsetTablePointer = (ReadWrite.Pointer)this.GetRequiredPointers($"H1_EntityOffsetTable");
                    IntPtr EntityOffsetTable = (IntPtr)this.HaloMemoryService.ReadWrite.ResolvePointer(EntityOffsetTablePointer);
                    Trace.WriteLine("EntityOffsetTable: " + EntityOffsetTable.ToString("X"));
                    ushort lowDatum = Convert.ToUInt16(playerDatum & 0xFFFF); // get lowest 2 bytes of playerDatum
                    int thingy = lowDatum * 0x0C;
                    Trace.WriteLine("lowDatum: " + lowDatum.ToString("X"));
                    Trace.WriteLine("thingy: " + thingy.ToString("X"));


                    int PlayerOffsetIndex = (int)this.GetRequiredPointers($"H1_PlayerOffsetIndex"); // 0x40
                    IntPtr PlayerOffsetPointer = IntPtr.Add(EntityOffsetTable, (thingy + PlayerOffsetIndex));
                    Trace.WriteLine("playerOffsetPointer: " + PlayerOffsetPointer.ToString("X"));
                    int PlayerOffset = (int)this.HaloMemoryService.ReadWrite.ReadInteger(new ReadWrite.Pointer(PlayerOffsetPointer)).Value;


                    Trace.WriteLine("PlayerOffset: " + PlayerOffset.ToString("X"));
                    ReadWrite.Pointer EntityTablePointer = (ReadWrite.Pointer)this.GetRequiredPointers($"H1_EntityTable");
                    IntPtr EntityTable = (IntPtr)this.HaloMemoryService.ReadWrite.ResolvePointer(EntityTablePointer);

                    int PlayerAddyIndex = (int)this.GetRequiredPointers($"H1_PlayerAddyIndex"); // 0x34
                    IntPtr playerAddy = IntPtr.Add(EntityTable, (PlayerOffset + PlayerAddyIndex));
                    return playerAddy;
                    break;

                default:
                    throw new Exception("GetPlayerAddress fed invalid game");
                    
            }

            throw new NotImplementedException();
        }


    }
}
