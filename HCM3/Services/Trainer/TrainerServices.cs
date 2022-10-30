using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using HCM3.Helpers;
using BurntMemory;

namespace HCM3.Services.Trainer
{
    public partial class TrainerServices
    {
        public HaloMemoryService HaloMemoryService { get; init; }
        public DataPointersService DataPointersService { get; init; }

        public CommonServices CommonServices { get; init; }
        public InternalServices InternalServices { get; init; }
        public TrainerServices(HaloMemoryService haloMemoryService, DataPointersService dataPointersService, CommonServices commonServices, InternalServices internalServices)
        {
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
            this.CommonServices = commonServices;
            this.InternalServices = internalServices;
        }

        public IntPtr GetPlayerVehiObjectAddress()
        {


            Trace.WriteLine("GetPlayerVehiObjectAddress called");
            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];


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


    }
}
