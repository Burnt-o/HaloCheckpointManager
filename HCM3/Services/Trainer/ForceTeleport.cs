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

        public void TeleportToLocation(float x, float y, float z)
        {


            Trace.WriteLine("TeleportToLocation called");
            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];


            IntPtr playerVehiObject = this.GetPlayerVehiObjectAddress();

            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_Xpos");
            requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_Ypos");
            requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_Zpos");

            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

            IntPtr playerXposPtr = IntPtr.Add(playerVehiObject, (int)requiredPointers["PlayerData_Xpos"]);
            IntPtr playerYposPtr = IntPtr.Add(playerVehiObject, (int)requiredPointers["PlayerData_Ypos"]);
            IntPtr playerZposPtr = IntPtr.Add(playerVehiObject, (int)requiredPointers["PlayerData_Zpos"]);

            GenericPrint($"Teleporting to {x}, {y}, {z}");

            this.HaloMemoryService.ReadWrite.WriteFloat(playerXposPtr, x);
            this.HaloMemoryService.ReadWrite.WriteFloat(playerYposPtr, y);
            this.HaloMemoryService.ReadWrite.WriteFloat(playerZposPtr, z);



        }


        public (float, float, float) TeleportGetPosition()
        {
            Trace.WriteLine("TeleportToLocation called");
            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];


            IntPtr playerVehiObject = this.GetPlayerVehiObjectAddress();

            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_Xpos");
            requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_Ypos");
            requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_Zpos");

            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

            IntPtr playerXposPtr = IntPtr.Add(playerVehiObject, (int)requiredPointers["PlayerData_Xpos"]);
            IntPtr playerYposPtr = IntPtr.Add(playerVehiObject, (int)requiredPointers["PlayerData_Ypos"]);
            IntPtr playerZposPtr = IntPtr.Add(playerVehiObject, (int)requiredPointers["PlayerData_Zpos"]);

            float oldXpos = this.HaloMemoryService.ReadWrite.ReadFloat(playerXposPtr).Value;
            float oldYpos = this.HaloMemoryService.ReadWrite.ReadFloat(playerYposPtr).Value;
            float oldZpos = this.HaloMemoryService.ReadWrite.ReadFloat(playerZposPtr).Value;

            return (oldXpos, oldYpos, oldZpos);
        }


        public void TeleportForward(float length, bool ignoreZ)
        {




            Trace.WriteLine("TeleportForward called");
            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];







            IntPtr playerVehiObject = this.GetPlayerVehiObjectAddress();
            Trace.WriteLine("PlayerVehicleObject: " + playerVehiObject.ToString("X"));
            if (playerVehiObject == IntPtr.Zero) throw new Exception("playerVehiObject was IntPtr.Zero!");

            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_ViewHori");
            requiredPointerNames.Add($"{gameAs2Letters}_ViewVert");
            requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_Xpos");
            requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_Ypos");
            requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_Zpos");

            if (gameAs2Letters == "H2")
            {
                requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_VisualOffset");
            }

            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

            IntPtr playerXposPtr = IntPtr.Add(playerVehiObject, (int)requiredPointers["PlayerData_Xpos"]);
            IntPtr playerYposPtr = IntPtr.Add(playerVehiObject, (int)requiredPointers["PlayerData_Ypos"]);
            IntPtr playerZposPtr = IntPtr.Add(playerVehiObject, (int)requiredPointers["PlayerData_Zpos"]);

            float oldXpos = this.HaloMemoryService.ReadWrite.ReadFloat(playerXposPtr).Value;
            float oldYpos = this.HaloMemoryService.ReadWrite.ReadFloat(playerYposPtr).Value;
            float oldZpos = this.HaloMemoryService.ReadWrite.ReadFloat(playerZposPtr).Value;

            //these units are in radians by default in all games.
            float viewHori = this.HaloMemoryService.ReadWrite.ReadFloat((ReadWrite.Pointer)requiredPointers["ViewHori"]).Value;
            float viewVert = this.HaloMemoryService.ReadWrite.ReadFloat((ReadWrite.Pointer)requiredPointers["ViewVert"]).Value;

            // Now, let's do some math to figure out what the new position should be.
            float normalisedX = (float)(Math.Cos(viewVert) * Math.Cos(viewHori));
            float normalisedY = (float)(Math.Cos(viewVert) * Math.Sin(viewHori));
            float normalisedZ = (float)(Math.Sin(viewVert));

            if (ignoreZ)
            {
                //renormalise X and Y without Z
                float currentTotal = Math.Abs(normalisedX) + Math.Abs(normalisedY);
                float renormalisationFactor = 1 / currentTotal;
                normalisedX = normalisedX * renormalisationFactor;
                normalisedY = normalisedY * renormalisationFactor;
            }

            float newXpos = oldXpos + (normalisedX * length);
            float newYpos = oldYpos + (normalisedY * length);
            float newZpos = oldZpos + (normalisedZ * length);


            GenericPrint($"Teleporting to {newXpos}, {newYpos}, {newZpos}");



            // Write the new positions
            this.HaloMemoryService.ReadWrite.WriteFloat(playerXposPtr, newXpos);
            this.HaloMemoryService.ReadWrite.WriteFloat(playerYposPtr, newYpos);
            if (!ignoreZ) this.HaloMemoryService.ReadWrite.WriteFloat(playerZposPtr, newZpos);

            if (gameAs2Letters == "H2")
            {
                //repeat position writing at +0x48
                playerXposPtr = IntPtr.Add(playerXposPtr, (int)requiredPointers["PlayerData_VisualOffset"]);
                playerYposPtr = IntPtr.Add(playerYposPtr, (int)requiredPointers["PlayerData_VisualOffset"]);
                playerZposPtr = IntPtr.Add(playerZposPtr, (int)requiredPointers["PlayerData_VisualOffset"]);

                this.HaloMemoryService.ReadWrite.WriteFloat(playerXposPtr, newXpos);
                this.HaloMemoryService.ReadWrite.WriteFloat(playerYposPtr, newYpos);
                if (!ignoreZ) this.HaloMemoryService.ReadWrite.WriteFloat(playerZposPtr, newZpos);
            }


        }



       


    }
}
