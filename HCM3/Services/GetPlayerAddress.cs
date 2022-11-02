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
                    ReadWrite.Pointer EntityOffsetTablePointer_H1 = (ReadWrite.Pointer)this.GetRequiredPointers($"H1_EntityOffsetTable");
                    IntPtr EntityOffsetTable_H1 = (IntPtr)this.HaloMemoryService.ReadWrite.ResolvePointer(EntityOffsetTablePointer_H1);
                    Trace.WriteLine("EntityOffsetTable_H1: " + EntityOffsetTable_H1.ToString("X"));
                    ushort lowDatum_H1 = Convert.ToUInt16(playerDatum & 0xFFFF); // get lowest 2 bytes of playerDatum
                    int thingy_H1 = lowDatum_H1 * 0x0C;
                    Trace.WriteLine("lowDatum_H1: " + lowDatum_H1.ToString("X"));
                    Trace.WriteLine("thingy_H1: " + thingy_H1.ToString("X"));


                    int PlayerOffsetIndex_H1 = (int)this.GetRequiredPointers($"H1_PlayerOffsetIndex"); // 0x40
                    IntPtr PlayerOffsetPointer_H1 = IntPtr.Add(EntityOffsetTable_H1, (thingy_H1 + PlayerOffsetIndex_H1));
                    Trace.WriteLine("playerOffsetPointer_H1: " + PlayerOffsetPointer_H1.ToString("X"));
                    int PlayerOffset_H1 = (int)this.HaloMemoryService.ReadWrite.ReadInteger(new ReadWrite.Pointer(PlayerOffsetPointer_H1)).Value;


                    Trace.WriteLine("PlayerOffset_H1: " + PlayerOffset_H1.ToString("X"));
                    ReadWrite.Pointer EntityTablePointer_H1 = (ReadWrite.Pointer)this.GetRequiredPointers($"H1_EntityTable");
                    IntPtr EntityTable_H1 = (IntPtr)this.HaloMemoryService.ReadWrite.ResolvePointer(EntityTablePointer_H1);

                    int PlayerAddyIndex_H1 = (int)this.GetRequiredPointers($"H1_PlayerAddyIndex"); // 0x34
                    IntPtr playerAddy_H1 = IntPtr.Add(EntityTable_H1, (PlayerOffset_H1 + PlayerAddyIndex_H1));
                    return playerAddy_H1;
                    break;


                case (int)Dictionaries.HaloStateEnum.Halo2:
                    ReadWrite.Pointer EntityOffsetTablePointer_H2 = (ReadWrite.Pointer)this.GetRequiredPointers($"H2_EntityOffsetTable"); //  new ReadWrite.Pointer("halo2.dll", new int[] { 0x1783E50, 0x58});
                    IntPtr EntityOffsetTable_H2 = (IntPtr)this.HaloMemoryService.ReadWrite.ResolvePointer(EntityOffsetTablePointer_H2);

                    ushort lowDatum2 = Convert.ToUInt16(playerDatum & 0xFFFF); // get lowest 2 bytes of playerDatum
                    EntityOffsetTable_H2 = IntPtr.Add(EntityOffsetTable_H2, ((lowDatum2 * 0x0C) + 08));

                    uint PlayerOffsetIndex_H2 = this.HaloMemoryService.ReadWrite.ReadInteger(EntityOffsetTable_H2).Value;

                    ReadWrite.Pointer EntityTablePointer_H2 = (ReadWrite.Pointer)this.GetRequiredPointers($"H2_EntityTable"); // new ReadWrite.Pointer("halo2.dll", new int[] { 0x1783E68, 0x57 });
                    IntPtr EntityTable_H2 = (IntPtr)this.HaloMemoryService.ReadWrite.ResolvePointer(EntityTablePointer_H2);
                    //round down EntityTable last 4 bits
                    EntityTable_H2 = (IntPtr)(EntityTable_H2.ToInt64() & ~0xF);
                    return IntPtr.Add(EntityTable_H2, (int)PlayerOffsetIndex_H2);
                    break;

                default:
                    throw new Exception("GetPlayerAddress fed invalid game");
                    
            }

            throw new NotImplementedException();
        }


    }
}
