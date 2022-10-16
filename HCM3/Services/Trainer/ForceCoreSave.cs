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

        public void ForceCoreSave(int selectedGame)
        {
            Trace.WriteLine("ForceCoreSave called, game: " + selectedGame);

            this.CommonServices.CheckGameIsAligned(selectedGame);
            string gameAs2Letters = Dictionaries.TabIndexTo2LetterGameCode[(int)selectedGame];
          

            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_ForceCoreSave");


            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

            this.CommonServices.PrintMessage("Core Save... Done", selectedGame);

            // Set the make coresave flag
            this.HaloMemoryService.ReadWrite.WriteData(
                (ReadWrite.Pointer?)requiredPointers["ForceCoreSave"],
                new byte[1] { (byte)0x1 },
                false);

        



        }


    }
}
