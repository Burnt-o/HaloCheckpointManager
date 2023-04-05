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
    internal class GenericToggleBit : AbstractToggleCheat
    {


        public GenericToggleBit(string pointerName, string nameOfCheat, bool offIsApplied, ServicesGroup servicesGroup) : base(servicesGroup)
        {
            this.PointerName = pointerName;
            this.NameOfCheat = nameOfCheat;
            this.OffIsApplied = offIsApplied; // True in the case that if the bit we read is 0, then the cheat is applied. Otherwise the reverse.

            this.EnabledMessage = $"{NameOfCheat} enabled.";
            this.DisabledMessage = $"{NameOfCheat} disabled.";
            this.ActiveMessage = $"{NameOfCheat}";
        }

        private string PointerName { get; init; }
        public string NameOfCheat { get; init; }
        private bool OffIsApplied { get; init; }



        private bool DoCommonThing(HaloState haloState, string thingToDo)
        {
            string er;
            Trace.WriteLine("GenericToggleBit doing common thing for " + thingToDo + ", cheat: " + NameOfCheat);
            ReadWrite.Pointer? bytePointer = (ReadWrite.Pointer?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_{PointerName}Flag", haloState);


            // If we couldn't get the pointer, cheat probably isn't applied
            if (bytePointer == null)
            {
                er = "Didn't have required pointers so cheat can't be active";
                if (thingToDo == "IsCheatApplied") { Trace.WriteLine(er); return false; } else { throw new Exception(er); };
            }


            int? byteFlagBitPosition = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_{PointerName}Bit", haloState);
            if (byteFlagBitPosition == null) byteFlagBitPosition = 0; // If no bit position provided, assume bit position 0

            // Read the byte where the skull bit resides
            byte? byteFlagValue = this.HaloMemoryService.ReadWrite.ReadByte(bytePointer);



            if (byteFlagValue == null)
            {
                er = $"Couldn't read byteFlagValue for {NameOfCheat}";
                if (thingToDo == "IsCheatApplied") { Trace.WriteLine(er); return false; } else { throw new Exception(er); };
            };

            // Convert the byte to a bit array
            BitArray bitArray = new BitArray(new byte[1] { byteFlagValue.Value });

            // Read the needed bit of the byte
            bool currentBitValue = bitArray.Get(byteFlagBitPosition.Value);

            if (thingToDo == "IsCheatApplied") return (currentBitValue ^ OffIsApplied);

            if ((currentBitValue) ^ OffIsApplied)
            {
                if (thingToDo == "RemoveCheat") throw new Exception($"Cheat {NameOfCheat} already removed");
            }
            else
            {
                if (thingToDo == "ApplyCheat") throw new Exception($"Cheat {NameOfCheat} already applied");
            }

            // Set that bit to the opposite value
            bitArray.Set(byteFlagBitPosition.Value, !currentBitValue);
            // Convert it back to a byte
            byte newByteValue = Helpers.Helpers.ConvertBitArrayToByte(bitArray);

            // Write the byte back
            return this.HaloMemoryService.ReadWrite.WriteByte(
                bytePointer,
                newByteValue,
                false);
        }


        public override void ApplyCheat(HaloState? haloState = null)
        {
            if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
            DoCommonThing(haloState, "ApplyCheat");
        }

        public override void RemoveCheat(HaloState? haloState = null)
        {
            if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
            try
            {
                DoCommonThing(haloState, "RemoveCheat");
            }
            catch (Exception ex)
            {
                Trace.WriteLine("Error removing " + this.PointerName + ", ex: " + ex.ToString());
            }

        }

        public override bool IsCheatApplied(HaloState? haloState = null)
        {
            if (haloState == null) haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();
            try
            {
                return DoCommonThing(haloState, "IsCheatApplied");
            }
            catch (Exception ex)
            {
                Trace.WriteLine("Error checking if applied " + this.PointerName + ", ex: " + ex.ToString());
                return false;
            }
        }

       




    }
}
