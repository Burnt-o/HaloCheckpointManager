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



                // Set the make core load flag
            this.HaloMemoryService.ReadWrite.WriteByte(
                (ReadWrite.Pointer?)requiredPointers["ForceCoreLoad"],
                (byte)1,
                false);

        



        }


    }
}
