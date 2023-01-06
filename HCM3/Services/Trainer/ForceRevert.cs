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

        public void ForceRevert()
        {
            

            Trace.WriteLine("ForceRevert called");

            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];


            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_ForceRevert");


            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);


            //only bother printing the message if overlay is enabled
            if (!Properties.Settings.Default.DisableOverlay && !this.InternalServices.PrintTemporaryMessageInternal("Revert forced.")) throw new Exception("Error printing message");


            IntPtr RevertFlag = this.HaloMemoryService.ReadWrite.ResolvePointer((ReadWrite.Pointer?)requiredPointers["ForceRevert"]).Value;

            // Set the make revert flag
            this.HaloMemoryService.ReadWrite.WriteByte(RevertFlag,
                (byte)1,
                false);



            // If game is Halo 1, we'll also want to reset the DeathFlag and DeathTimer, to prevent getting automatically reverted again if you revert while dead.
            if (gameAs2Letters == "H1")
            {
                int deathFlagOffset = (int)this.CommonServices.GetRequiredPointers("H1_DeathFlagOffset");
                int deathTimerOffset = (int)this.CommonServices.GetRequiredPointers("H1_DeathTimerOffset");

                IntPtr deathFlag = IntPtr.Add(RevertFlag, deathFlagOffset);
                IntPtr deathTimer = IntPtr.Add(RevertFlag, deathTimerOffset);

                // Set both to 0
                this.HaloMemoryService.ReadWrite.WriteByte(deathFlag,(byte)0,false);
                this.HaloMemoryService.ReadWrite.WriteByte(deathTimer,(byte)0, false);

            }



        }


    }
}
