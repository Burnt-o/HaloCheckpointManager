using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Services.HaloMemory;
using HCM3.Helpers;
using System.Diagnostics;
using BurntMemory;

namespace HCM3.Services.Internal
{

    public partial class InternalServices
    {
        private IntPtr? hookEntityLoop_DetourHandle = null;

        /// <summary>
        /// Populates PlayerDataStruct and sends it to HCMInternal. Applys detour so internal hooks the EntityLoop. 
        /// </summary>
        public void LoadPlayerDataIfNotLoaded(HaloState haloState)
        {

            // Part 1: Populate PlayerDataStruct and send it to HCMInternal
            // Check if data already loaded
            InternalReturn returnVal = this.CallInternalFunction("IsPlayerDataLoaded", null);
            if (returnVal == InternalReturn.True)
            {
                Trace.WriteLine("PlayerData already loaded");
            }
            else
            {
                Trace.WriteLine("Loading player data into internal");
                // Construct a player data struct
                PlayerDataInfo playerDataInfo = new PlayerDataInfo();

                // Load Pointers
                playerDataInfo.pPlayerDatum = ReadPointer("PlayerDatum", haloState);

                bool usePlayerVehiDatum = (haloState.GameState != GameStateEnum.Halo1 && haloState.GameState != GameStateEnum.Halo2);
                playerDataInfo.pPlayerVehiDatum = usePlayerVehiDatum ? ReadPointer("PlayerVehiDatum", haloState) : 0;

                playerDataInfo.pTickCounter = ReadPointer("CurrentTickCount", haloState);
                playerDataInfo.pViewHorizontal = ReadPointer("ViewHori", haloState);
                playerDataInfo.pViewVertical = ReadPointer("ViewVert", haloState);

                playerDataInfo.pTestValue = ReadPointer("pTestValue", haloState); //points to the "leve" or "maps" string that should never change (unless dll reloaded or exit to mainmenu)
                playerDataInfo.TestValue = (uint?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_TestValue", haloState) ?? throw new Exception($"Couldn't get TestValue");

                playerDataInfo.CurrentGame = (Int64)haloState.GameState; // Internal needs to know which game

                // Offsets describing how to interpret player data
                playerDataInfo.OffsetEntityTestValue = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetEntityTestValue", haloState) ?? throw new Exception($"Couldn't get pointer to OffsetEntityTestValue");
                playerDataInfo.EntityTestValue = (uint?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_EntityTestValue", haloState) ?? throw new Exception($"Couldn't get pointer to EntityTestValue");

                playerDataInfo.OffsetEntityPosition = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetEntityPosition", haloState) ?? throw new Exception($"Couldn't get pointer to OffsetEntityPosition");
                playerDataInfo.OffsetEntityVelocity = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetEntityVelocity", haloState) ?? throw new Exception($"Couldn't get pointer to OffsetEntityVelocity");
                playerDataInfo.OffsetEntityHealth = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetEntityHealth", haloState) ?? throw new Exception($"Couldn't get pointer to OffsetEntityHealth");
                playerDataInfo.OffsetEntityShield = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetEntityShield", haloState) ?? throw new Exception($"Couldn't get pointer to OffsetEntityShield");

                playerDataInfo.OffsetPlayerVehicleDatum = (haloState.GameState == GameStateEnum.Halo1) ? (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetPlayerVehicleDatum", haloState) ?? throw new Exception($"Couldn't get pointer to OffsetPlayerVehicleDatum") : 0;

                playerDataInfo.OffsetEntityTrigPosition = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetEntityTrigPosition", haloState) ?? throw new Exception($"Couldn't get pointer to OffsetEntityTrigPosition");
    
                returnVal = this.CallInternalFunction("LoadPlayerData", playerDataInfo);
                if (returnVal != InternalReturn.True) throw new Exception("Failed calling LoadPlayerData: " + returnVal);
            }


            // Part 2: Hook entityloop.
            DetourInfoObject hookEntityLoop_DetourInfo = (DetourInfoObject)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_HookEntityLoop_DetourInfo", haloState) ?? throw new Exception($"Couldn't get pointer to HookEntityLoop_DetourInfo");
            if (this.HaloMemoryService.DetourCheckOG(hookEntityLoop_DetourInfo))
            {
                hookEntityLoop_DetourHandle = this.HaloMemoryService.DetourApply(hookEntityLoop_DetourInfo);
                System.Threading.Thread.Sleep(50); // give time for entityloop to be run so playerdata is populated for whatever is calling this
            }
            else
            {
                // Commenting this out cos.. well if not OG it's PROBABLY already applied, thus no need to apply it again eh
            }


        }

  

    }
}
