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

        public void ForceRevert(int selectedGame)
        {
            Trace.WriteLine("ForceRevert called, game: " + selectedGame);

            this.CommonServices.IsGameCorrect(selectedGame);
            string gameAs2Letters = Dictionaries.TabIndexTo2LetterGameCode[(int)selectedGame];
          

            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_ForceRevert");


            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);
            
                // Set the make revert flag
            this.HaloMemoryService.ReadWrite.WriteData(
                (ReadWrite.Pointer?)requiredPointers["ForceRevert"],
                new byte[1] { (byte)0x1 },
                false);

          



        }


    }
}
