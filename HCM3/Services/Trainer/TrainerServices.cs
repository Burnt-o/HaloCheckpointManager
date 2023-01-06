using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using HCM3.Helpers;
using BurntMemory;
using System.Threading;

namespace HCM3.Services.Trainer
{
    public partial class TrainerServices
    {
        public HaloMemoryService HaloMemoryService { get; init; }
        public DataPointersService DataPointersService { get; init; }

        public CommonServices CommonServices { get; init; }
        public InternalServices InternalServices { get; init; }

        public PersistentCheatService PersistentCheatService { get; init; }
        public TrainerServices(HaloMemoryService haloMemoryService, DataPointersService dataPointersService, CommonServices commonServices, InternalServices internalServices, PersistentCheatService persistentCheatService)
        {
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
            this.CommonServices = commonServices;
            this.InternalServices = internalServices;
            this.PersistentCheatService = persistentCheatService;

            this.SetupPatches();
            HaloStateEvents.HALOSTATECHANGED_EVENT += ApplyPatches;
        }

        public IntPtr GetPlayerVehiObjectAddress()
        {


            Trace.WriteLine("GetPlayerVehiObjectAddress called");
            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

            if (gameAs2Letters == "H1")
            {
                List<string> requiredPointerNames = new();
                requiredPointerNames.Add($"{gameAs2Letters}_PlayerDatum");
                requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_VehicleIndex");
                requiredPointerNames.Add($"{gameAs2Letters}_PlayerData_VehicleNullValue");

                Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

                uint playerDatum = this.HaloMemoryService.ReadWrite.ReadInteger((ReadWrite.Pointer)requiredPointers["PlayerDatum"]).Value;
                IntPtr playerAddy = this.CommonServices.GetAddressFromDatum(playerDatum);


                //check if player in vehicle, if so we want to modify that vehicle instead of the player
                uint playerVehicleDatum = (uint)this.HaloMemoryService.ReadWrite.ReadInteger(IntPtr.Add(playerAddy, (int)requiredPointers["PlayerData_VehicleIndex"]));
                if (playerVehicleDatum != (uint)requiredPointers["PlayerData_VehicleNullValue"])
                {
                    playerAddy = this.CommonServices.GetAddressFromDatum(playerVehicleDatum);
                }

                Trace.WriteLine("PlayerVehiObjectAddress: " + playerAddy.ToString("X"));

                return playerAddy;
            }
            else if (gameAs2Letters == "H2")
            {
                if (this.listOfPatches["H2PlayerData"].PatchHandle == null || !IsPatchApplied("H2PlayerData", true))
                {
                    if (IsPatchApplied("H2PlayerData", true))
                    {
                        RemovePatch("H2PlayerData", true);
                    }
                    ApplyPatch("H2PlayerData", true);
                    Thread.Sleep(30);
                }

                IntPtr? detourHandle = this.listOfPatches["H2PlayerData"].PatchHandle;
                if (detourHandle == null) throw new Exception("Couldn't get detourHandle of H2PlayerData");
                IntPtr? playerAddy = (IntPtr?)this.HaloMemoryService.ReadWrite.ReadQword(detourHandle.Value + 0x50);
                if (playerAddy == null) throw new Exception("couldn't parse player addy from detour handle t H2PlayerData");
                Trace.WriteLine("H2 player info test: " + playerAddy.Value);
                return playerAddy.Value;

            }
            else if (gameAs2Letters == "H3")
            {
                if (this.listOfPatches["H3PlayerData"].PatchHandle == null || !IsPatchApplied("H3PlayerData", true))
                {
                    if (IsPatchApplied("H3PlayerData", true))
                    {
                        RemovePatch("H3PlayerData", true);
                    }
                    ApplyPatch("H3PlayerData", true);
                    Thread.Sleep(30);
                }

                IntPtr? detourHandle = this.listOfPatches["H3PlayerData"].PatchHandle;
                if (detourHandle == null) throw new Exception("Couldn't get detourHandle of H3PlayerData");
                IntPtr? playerAddy = (IntPtr?)this.HaloMemoryService.ReadWrite.ReadQword(detourHandle.Value + 0x50);
                if (playerAddy == null) throw new Exception("couldn't parse player addy from detour handle t H3PlayerData");
                Trace.WriteLine("H3 player info test: " + playerAddy.Value.ToString("X"));
                return playerAddy.Value;

            }
            else if (gameAs2Letters == "OD")
            {
                if (this.listOfPatches["ODPlayerData"].PatchHandle == null || !IsPatchApplied("ODPlayerData", true))
                {
                    if (IsPatchApplied("ODPlayerData", true))
                    {
                        RemovePatch("ODPlayerData", true);
                    }
                    ApplyPatch("ODPlayerData", true);
                    Thread.Sleep(30);
                }

                IntPtr? detourHandle = this.listOfPatches["ODPlayerData"].PatchHandle;
                if (detourHandle == null) throw new Exception("Couldn't get detourHandle of ODPlayerData");
                IntPtr? playerAddy = (IntPtr?)this.HaloMemoryService.ReadWrite.ReadQword(detourHandle.Value + 0x50);
                if (playerAddy == null) throw new Exception("couldn't parse player addy from detour handle t ODPlayerData");
                return playerAddy.Value;

            }
            else if (gameAs2Letters == "HR")
            {
                if (this.listOfPatches["HRPlayerData"].PatchHandle == null || !IsPatchApplied("HRPlayerData", true))
                {
                    if (IsPatchApplied("HRPlayerData", true))
                    {
                        RemovePatch("HRPlayerData", true);
                    }
                    ApplyPatch("HRPlayerData", true);
                    Thread.Sleep(30);
                }

                IntPtr? detourHandle = this.listOfPatches["HRPlayerData"].PatchHandle;
                if (detourHandle == null) throw new Exception("Couldn't get detourHandle of HRPlayerData");
                IntPtr? playerAddy = (IntPtr?)this.HaloMemoryService.ReadWrite.ReadQword(detourHandle.Value + 0x50);
                if (playerAddy == null) throw new Exception("couldn't parse player addy from detour handle t HRPlayerData");
                return playerAddy.Value;

            }

            throw new NotImplementedException();




        }

        //https://stackoverflow.com/a/560180
        private byte ConvertToByte(System.Collections.BitArray bits)
        {
            if (bits.Count != 8)
            {
                throw new ArgumentException("illegal number of bits");
            }

            byte b = 0;
            if (bits.Get(0)) b++;
            if (bits.Get(1)) b += 2;
            if (bits.Get(2)) b += 4;
            if (bits.Get(3)) b += 8;
            if (bits.Get(4)) b += 16;
            if (bits.Get(5)) b += 32;
            if (bits.Get(6)) b += 64;
            if (bits.Get(7)) b += 128;
            return b;
        }




        public void GenericPrint(string messageToPrint, bool needToDisableCheckpointText = false)
        {
            if (!Properties.Settings.Default.DisableOverlay)
            {
                if (!this.HaloMemoryService.HaloState.OverlayHooked) throw new Exception("Overlay wasn't hooked");
                if (!this.InternalServices.PrintTemporaryMessageInternal(messageToPrint)) throw new Exception("Error printing message");
            }
            else
            {
                //throw new Exception ("Overlay needs to be enabled for this cheat to work")
                if (!this.CommonServices.PrintMessage(messageToPrint, needToDisableCheckpointText))
                {
                    throw new Exception("Failed to print in-game message");
                }
            }
        }



    }
}
