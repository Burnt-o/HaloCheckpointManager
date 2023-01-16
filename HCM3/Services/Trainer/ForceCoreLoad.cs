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

        public void ForceCoreLoad()
        {
            Trace.WriteLine("ForceCoreLoad called");

            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];


            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_ForceCoreLoad");


            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

            //only bother printing message if overlay enabled
            if (!Properties.Settings.Default.DisableOverlay && !this.InternalServices.PrintTemporaryMessageInternal("Core load forced.")) throw new Exception("Error printing message");

            // Set the make core load flag
            this.HaloMemoryService.ReadWrite.WriteByte(
                (ReadWrite.Pointer?)requiredPointers["ForceCoreLoad"],
                (byte)1,
                false);


            // If game is Halo 1, we'll also want to reset the DeathFlag and DeathTimer, to prevent getting automatically reverted again if you coreload while dead.
            if (gameAs2Letters == "H1")
            {
                try
                {
                    ReadWrite.Pointer? RevertFlagPtr = (ReadWrite.Pointer?)this.CommonServices.GetRequiredPointers("H1_ForceRevert");
                    IntPtr? RevertFlag = this.HaloMemoryService.ReadWrite.ResolvePointer(RevertFlagPtr);

                    int deathFlagOffset = (int)this.CommonServices.GetRequiredPointers("H1_DeathFlagOffset");
                    int deathTimerOffset = (int)this.CommonServices.GetRequiredPointers("H1_DeathTimerOffset");

                    IntPtr deathFlag = IntPtr.Add(RevertFlag.Value, deathFlagOffset);
                    IntPtr deathTimer = IntPtr.Add(RevertFlag.Value, deathTimerOffset);

                    // Set both to 0
                    this.HaloMemoryService.ReadWrite.WriteByte(deathFlag, (byte)0, false);
                    this.HaloMemoryService.ReadWrite.WriteByte(deathTimer, (byte)0, false);
                }
                catch (Exception ex)
                {
                    Trace.WriteLine("Failed clearing auto-revert flags when core-loading, ex: " + ex.Message);
                }
               

            }


        }


    }
}
