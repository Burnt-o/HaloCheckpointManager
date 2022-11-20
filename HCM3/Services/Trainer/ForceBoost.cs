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




        public void BoostForward(float addSpeed)
        {
            if (!this.HaloMemoryService.HaloState.OverlayHooked) throw new Exception("Overlay wasn't hooked");

            Trace.WriteLine("BoostForward called");
            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];


            IntPtr playerVehiObject = this.GetPlayerVehiObjectAddress();

            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_ViewHori");
            requiredPointerNames.Add($"{gameAs2Letters}_ViewVert");
            requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_Xvel");
            requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_Yvel");
            requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_Zvel");

            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

            Trace.WriteLine("before adding negagtive offset: " + playerVehiObject.ToString("X"));
            IntPtr playerXvelPtr = IntPtr.Add(playerVehiObject, (int)requiredPointers["PlayerData_Xvel"]);
            Trace.WriteLine("after adding negagtive offset: " + playerXvelPtr.ToString("X"));
            IntPtr playerYvelPtr = IntPtr.Add(playerVehiObject, (int)requiredPointers["PlayerData_Yvel"]);
            IntPtr playerZvelPtr = IntPtr.Add(playerVehiObject, (int)requiredPointers["PlayerData_Zvel"]);

            float oldXvel = this.HaloMemoryService.ReadWrite.ReadFloat(playerXvelPtr).Value;
            float oldYvel = this.HaloMemoryService.ReadWrite.ReadFloat(playerYvelPtr).Value;
            float oldZvel = this.HaloMemoryService.ReadWrite.ReadFloat(playerZvelPtr).Value;

            //these units are in radians by default in all games.
            float viewHori = this.HaloMemoryService.ReadWrite.ReadFloat((ReadWrite.Pointer)requiredPointers["ViewHori"]).Value;
            float viewVert = this.HaloMemoryService.ReadWrite.ReadFloat((ReadWrite.Pointer)requiredPointers["ViewVert"]).Value;

            // Now, let's do some math to figure out what the new velocities should be.
            float normalisedX = (float)(Math.Cos(viewVert) * Math.Cos(viewHori));
            float normalisedY = (float)(Math.Cos(viewVert) * Math.Sin(viewHori));
            float normalisedZ = (float)(Math.Sin(viewVert));


            float newXvel = oldXvel + (normalisedX * addSpeed);
            float newYvel = oldYvel + (normalisedY * addSpeed);
            float newZvel = oldZvel + (normalisedZ * addSpeed);

            if (!this.InternalServices.PrintTemporaryMessageInternal($"Launching with {addSpeed} force")) throw new Exception("Error printing message");

            // Write the new velocities
            this.HaloMemoryService.ReadWrite.WriteFloat(playerXvelPtr, newXvel);
            this.HaloMemoryService.ReadWrite.WriteFloat(playerYvelPtr, newYvel);
            this.HaloMemoryService.ReadWrite.WriteFloat(playerZvelPtr, newZvel);

        }



       


    }
}
