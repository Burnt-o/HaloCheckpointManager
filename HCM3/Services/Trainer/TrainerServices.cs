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
                if (this.listOfPatches["H2PlayerData"].PatchHandle == null || !IsPatchApplied("H2PlayerData"))
                {
                    if (IsPatchApplied("H2PlayerData"))
                    {
                        RemovePatch("H2PlayerData");
                    }
                    ApplyPatch("H2PlayerData");
                    Thread.Sleep(30);
                }

                IntPtr? detourHandle = this.listOfPatches["H2PlayerData"].PatchHandle;
                if (detourHandle == null) throw new Exception("Couldn't get detourHandle of H2PlayerData");
                IntPtr? playerAddy = (IntPtr?)this.HaloMemoryService.ReadWrite.ReadQword(detourHandle.Value + 0x50);
                if (playerAddy == null) throw new Exception("couldn't parse player addy from detour handle t H2PlayerData");
                return playerAddy.Value;

            }
            else if (gameAs2Letters == "H3")
            {
                if (this.listOfPatches["H3PlayerData"].PatchHandle == null || !IsPatchApplied("H3PlayerData"))
                {
                    if (IsPatchApplied("H3PlayerData"))
                    {
                        RemovePatch("H3PlayerData");
                    }
                    ApplyPatch("H3PlayerData");
                    Thread.Sleep(30);
                }

                IntPtr? detourHandle = this.listOfPatches["H3PlayerData"].PatchHandle;
                if (detourHandle == null) throw new Exception("Couldn't get detourHandle of H3PlayerData");
                IntPtr? playerAddy = (IntPtr?)this.HaloMemoryService.ReadWrite.ReadQword(detourHandle.Value + 0x50);
                if (playerAddy == null) throw new Exception("couldn't parse player addy from detour handle t H3PlayerData");
                return playerAddy.Value;

            }
            else if (gameAs2Letters == "OD")
            {
                if (this.listOfPatches["ODPlayerData"].PatchHandle == null || !IsPatchApplied("ODPlayerData"))
                {
                    if (IsPatchApplied("ODPlayerData"))
                    {
                        RemovePatch("ODPlayerData");
                    }
                    ApplyPatch("ODPlayerData");
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
                if (this.listOfPatches["HRPlayerData"].PatchHandle == null || !IsPatchApplied("HRPlayerData"))
                {
                    if (IsPatchApplied("HRPlayerData"))
                    {
                        RemovePatch("HRPlayerData");
                    }
                    ApplyPatch("HRPlayerData");
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


    }
}
