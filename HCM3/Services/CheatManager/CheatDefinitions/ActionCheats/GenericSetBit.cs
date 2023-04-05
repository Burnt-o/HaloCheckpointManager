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
    internal class GenericSetBit : AbstractActionCheat
    {
     
        // Used for many action cheats, such as forcing a checkpoint, a revert, core save, and core load.
        public GenericSetBit(string pointerName, string enabledMessage, bool offIsApplied, ServicesGroup servicesGroup) : base(servicesGroup) 
        {
            this.PointerName = pointerName;
            this.EnabledMessage = enabledMessage;
            this.OffIsApplied = offIsApplied;   
    }

        private string PointerName { get; init; }

    private bool OffIsApplied { get; init; }

    protected override void ApplyCheat(HaloState haloState)
        {
            ReadWrite.Pointer? bytePointer = (ReadWrite.Pointer?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_{PointerName}", haloState);
            if (bytePointer == null) throw new Exception($"Couldn't get pointer for {PointerName}");

            int? byteFlagBitPosition = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_{PointerName}Bit", haloState);
            if (byteFlagBitPosition == null) byteFlagBitPosition = 0; // If no bit position provided, assume bit position 0

            // Read the byte where the skull bit resides
            byte? byteFlagValue = this.HaloMemoryService.ReadWrite.ReadByte(bytePointer);

            if (byteFlagValue == null) throw new Exception($"Couldn't read byteFlagValue for {PointerName}");

            // Convert the byte to a bit array
            BitArray bitArray = new BitArray(new byte[1] { byteFlagValue.Value });

            // Read the needed bit of the byte
            bool currentBitValue = bitArray.Get(byteFlagBitPosition.Value);

            // Set that bit to the correct value
            bitArray.Set(byteFlagBitPosition.Value, !OffIsApplied);
            // Convert it back to a byte
            byte newByteValue = Helpers.Helpers.ConvertBitArrayToByte(bitArray);

            this.CheatManagerService.SendMessage(EnabledMessage, haloState);

            // Write the byte back
            this.HaloMemoryService.ReadWrite.WriteByte(
                bytePointer,
                newByteValue,
                false);



        }







    }
}
