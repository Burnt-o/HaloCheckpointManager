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
namespace HCM3.Services.Trainer
{
    public partial class TrainerServices
    {

        public void FlipDoubleRevert()
        {

            {
                Trace.WriteLine("ForceRevert called");

                this.HaloMemoryService.HaloState.UpdateHaloState();
                int loadedGame = this.CommonServices.GetLoadedGame();
                string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

                ReadWrite.Pointer? doubleRevertFlagPointer = (ReadWrite.Pointer)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_DoubleRevertFlag");
                if (doubleRevertFlagPointer == null) throw new Exception("Couldn't double revert, was missing DRF pointer");

                // Read the flag
                byte? doubleRevertFlag = this.HaloMemoryService.ReadWrite.ReadByte(doubleRevertFlagPointer);

                if (doubleRevertFlag == null || (doubleRevertFlag != 0 && doubleRevertFlag != 1)) throw new Exception("Couldn't read double revert flag");

                // Set the double revert flag to the inverse value
                this.HaloMemoryService.ReadWrite.WriteData(
                    doubleRevertFlagPointer,
                    new byte[1] { (byte)(doubleRevertFlag == 0 ? 1 : 0) },
                    false);

            }

        }
    }
}
