using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using HCM3.Models;
using HCM3.Helpers;
using BurntMemory;
using System.Threading;
using System.Collections;
namespace HCM3.Services.Trainer
{
    public partial class TrainerServices
    {

        public void FlipSkull(string skullName)
        {

            {
                Trace.WriteLine("SkullFlip called");

                this.HaloMemoryService.HaloState.UpdateHaloState();
                int loadedGame = this.CommonServices.GetLoadedGame();
                string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

                ReadWrite.Pointer skullFlagPointer = (ReadWrite.Pointer)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_Skull{skullName}Flag");

                // Read the flag
                byte? skullFlag = this.HaloMemoryService.ReadWrite.ReadByte(skullFlagPointer);

                if (skullFlag == null) throw new Exception("Couldn't read skull flag");

                int skullFlagBitPosition = (int)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_Skull{skullName}Bit");

                byte[] bytearray = new byte[1] { skullFlag.Value };
                BitArray ba = new BitArray(bytearray);

                Trace.WriteLine("SkullFlagBitPosition: " + skullFlagBitPosition);

                bool currentSkullStatus = ba.Get(skullFlagBitPosition);

                if (currentSkullStatus)
                {
                    if (!this.InternalServices.PrintTemporaryMessageInternal($"Turning {skullName} Off. ")) throw new Exception("Error printing message");
                }
                else
                {
                    if (!this.InternalServices.PrintTemporaryMessageInternal($"Turning {skullName} On. ")) throw new Exception("Error printing message");
                }
                
                ba.Set(skullFlagBitPosition, !currentSkullStatus);
                byte newValue = ConvertToByte(ba);

                // Set
                this.HaloMemoryService.ReadWrite.WriteByte(
                    skullFlagPointer,
                    newValue,
                    false);

            }

        }
    }
}
