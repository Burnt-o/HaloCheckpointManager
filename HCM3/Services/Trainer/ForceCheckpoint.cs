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

        public void ForceCheckpoint()
        {


            Trace.WriteLine("ForceCheckpoint called");
            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];
          

            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_ForceCheckpoint");
            requiredPointerNames.Add($"{gameAs2Letters}_CPMessageCall");
            requiredPointerNames.Add($"{gameAs2Letters}_CPMessageCallLength");

            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);


            // Read Bytes at CPMessageCall, so we can restore them later
            byte[]? originalCPMessageCall = this.HaloMemoryService.ReadWrite.ReadBytes((ReadWrite.Pointer?)requiredPointers["CPMessageCall"], (int)requiredPointers["CPMessageCallLength"]);

            if (originalCPMessageCall == null) throw new Exception("couldn't read original checkpoint message call");

            // Nop message call bytes
            byte[] nop = new byte[originalCPMessageCall.Length];
            Array.Fill(nop, (byte)0x90);
            bool success = this.HaloMemoryService.ReadWrite.WriteBytes((ReadWrite.Pointer?)requiredPointers["CPMessageCall"], nop, true);

            // Call PrintMessage("Custom Checkpoint... Done")
            success = success && this.CommonServices.PrintMessage("Custom Checkpoint... Done", loadedGame);


            if (!success) throw new Exception("Error while formatting checkpoint message string");

                // Set the make checkpoint flag
            this.HaloMemoryService.ReadWrite.WriteData(
                (ReadWrite.Pointer?)requiredPointers["ForceCheckpoint"],
                new byte[1] { (byte)0x1 },
                false);

            // wait 50ms
            Thread.Sleep(50);

            // Restore message call bytes
            this.HaloMemoryService.ReadWrite.WriteBytes((ReadWrite.Pointer?)requiredPointers["CPMessageCall"], originalCPMessageCall, true);



        }


    }
}
