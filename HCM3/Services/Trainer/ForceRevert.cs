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






            IntPtr? RevertFlag = this.HaloMemoryService.ReadWrite.ResolvePointer((ReadWrite.Pointer?)requiredPointers["ForceRevert"]).Value;

            if (RevertFlag == null) throw new Exception("Revert flag was null!");


            //Next, a check for if the checkpoint data is null (happens in multiplayer - if a revert or inject is called when a checkpoint hasn't been made yet, the game will crash)
            if (this.CommonServices.IsMultiplayer())
            {
                try
                {
                    ReadWrite.Pointer? doubleRevertFlag = null;
                    try
                    {
                        doubleRevertFlag = (ReadWrite.Pointer?)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_DoubleRevertFlag");
                    }
                    catch { }

                    string whichLocation = "1";
                    if (doubleRevertFlag != null && this.HaloMemoryService.ReadWrite.ReadByte(doubleRevertFlag) == 1) whichLocation = "2";

                    ReadWrite.Pointer checkpointLocation = (ReadWrite.Pointer)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_CheckpointLocation" + whichLocation);
                    if (this.CommonServices.CheckpointDataIsNull(checkpointLocation))
                    {
                        Trace.WriteLine("Bailing on revert as checkpoint data was null");
                        return;
                    }

                }
                catch (Exception ex)
                {
                    Trace.WriteLine("Failed un-nullifying the checkpoint data");
                }
            }

            //only bother printing the message if overlay is enabled
            if (!Properties.Settings.Default.DisableOverlay && !this.InternalServices.PrintTemporaryMessageInternal("Revert forced.")) throw new Exception("Error printing message");

            // Set the make revert flag
            this.HaloMemoryService.ReadWrite.WriteByte(RevertFlag,
                (byte)1,
                false);



            // If game is Halo 1, we'll also want to reset the DeathFlag and DeathTimer, to prevent getting automatically reverted again if you revert while dead.
            if (gameAs2Letters == "H1")
            {
                try
                {
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
                Trace.WriteLine("Failed clearing auto-revert flags when Force Reverting, ex: " + ex.Message);
                }
            }



        }


    }
}
