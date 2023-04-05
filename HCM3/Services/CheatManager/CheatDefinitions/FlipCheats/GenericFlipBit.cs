using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using System.Diagnostics;
using HCM3.Services.HaloMemory;
using HCM3.Helpers;
using System.Collections;

namespace HCM3.Services.Cheats
{
    internal class GenericFlipBit : AbstractFlipCheat
    {
     

        public GenericFlipBit(string pointerName, string nameOfCheat, bool offIsApplied, ServicesGroup servicesGroup) : base(servicesGroup) 
        {
            this.PointerName = pointerName;
            this.NameOfCheat = nameOfCheat;
            this.OffIsApplied = offIsApplied; // True in the case that if the bit we read is 0, then the cheat is applied. Otherwise the reverse.

            this.EnabledMessage = $"Turning {NameOfCheat} on.";
            this.DisabledMessage = $"Turning {NameOfCheat} off.";
        }

        private string PointerName { get; set; }
        public string NameOfCheat { get; set; }
        private bool OffIsApplied { get; init; }

        protected override void FlipCheat(HaloState haloState)
        {
            ReadWrite.Pointer? bytePointer = (ReadWrite.Pointer?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_{PointerName}Flag", haloState);

            int? byteFlagBitPosition = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_{PointerName}Bit", haloState);
            if (byteFlagBitPosition == null) byteFlagBitPosition = 0; // If no bit position provided, assume bit position 0

            if (bytePointer == null) throw new Exception("Couldn't get FlipByte flag pointer");
            
            // Read the byte where the skull bit resides
            byte? byteFlagValue = this.HaloMemoryService.ReadWrite.ReadByte(bytePointer);

            if (byteFlagValue == null) throw new Exception("Couldn't read FlipByte flag");

            // Convert the byte to a bit array
            BitArray bitArray = new BitArray(new byte[1] { byteFlagValue.Value });

            // Read the needed bit of the byte
            bool currentBitValue = bitArray.Get(byteFlagBitPosition.Value);

            if (currentBitValue ^ OffIsApplied)
            {
                this.CheatManagerService.SendMessage(EnabledMessage, haloState);
            }
            else
            {
                this.CheatManagerService.SendMessage(DisabledMessage, haloState);
            }

            // Set that bit to the opposite value
            bitArray.Set(byteFlagBitPosition.Value, !currentBitValue);
            // Convert it back to a byte
            byte newByteValue = Helpers.Helpers.ConvertBitArrayToByte(bitArray);

            // Write the byte back
            this.HaloMemoryService.ReadWrite.WriteByte(
                bytePointer,
                newByteValue,
                false);
        }





    }
}
