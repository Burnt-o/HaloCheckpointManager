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

        public void ForceCoreSave()
        {


            Trace.WriteLine("ForceCoreSave called");

            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_ForceCoreSave");


            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

            GenericPrint("Core save forced.");

            // Set the make coresave flag
            this.HaloMemoryService.ReadWrite.WriteByte(
                (ReadWrite.Pointer?)requiredPointers["ForceCoreSave"],
                (byte)1,
                false);

        



        }


    }
}
