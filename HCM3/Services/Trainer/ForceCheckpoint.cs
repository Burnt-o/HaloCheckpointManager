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


            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);


            GenericPrint("Checkpoint forced.", true);

            // Set the make checkpoint flag
            this.HaloMemoryService.ReadWrite.WriteByte(
                (ReadWrite.Pointer?)requiredPointers["ForceCheckpoint"],
                (byte)1,
                false);

            //as an unrelated test
            //try
            //{
            //    ReadWrite.Pointer pdatum = new ReadWrite.Pointer("halo2.dll", new int[] { 0xD523A4 });
            //    uint pdatumactual = this.HaloMemoryService.ReadWrite.ReadInteger(pdatum).Value;

            //    IntPtr playerAddy = this.CommonServices.GetAddressFromDatum(pdatumactual);
            //    Trace.WriteLine("H2 Player addy1: " + playerAddy.ToString("X"));

            //    pdatumactual = pdatumactual + 1;
            //    IntPtr playerAddy2 = this.CommonServices.GetAddressFromDatum(pdatumactual);
            //    Trace.WriteLine("H2 Player addy2: " + playerAddy2.ToString("X"));
            //}
            //catch (Exception ex)
            //{ 
            //Trace.WriteLine("Failed reading player addy: " + ex.ToString());
            //}



        }


    }
}
