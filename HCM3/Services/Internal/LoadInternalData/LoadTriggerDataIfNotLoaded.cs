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
        private IntPtr? triggerSearchDetourHandle = null;

        /// <summary>
        /// Populates TriggerDataInfo and sends it to HCMInternal. Applys detour to TriggerSearch so internal can hook it. 
        /// </summary>
        public void LoadTriggerDataIfNotLoaded(HaloState haloState)
        {

            // Check if data already loaded
            InternalReturn returnVal = this.CallInternalFunction("IsTriggerDataLoaded", null);
            if (returnVal == InternalReturn.True)
            {
                Trace.WriteLine("Trigger already loaded");
            }
            else
            {
                Trace.WriteLine("Loading trigger data into internal");
                // Construct a player data struct
                TriggerDataInfo triggerDataInfo = new TriggerDataInfo();

                // First, get MAPADDRESS sub and add values
                ReadWrite.Pointer pMAPADDRESS_sub = (ReadWrite.Pointer?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_MAPADDRESS_sub", haloState) ?? throw new Exception($"Couldn't get pointer to pMAPADDRESS_sub");
                UInt64 MAPADDRESS_sub = this.HaloMemoryService.ReadWrite.ReadQword(pMAPADDRESS_sub) ?? throw new Exception("Couldn't read value at pMAPADDRESS_sub");

                ReadWrite.Pointer pMAPADDRESS_add = (ReadWrite.Pointer?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_MAPADDRESS_add", haloState) ?? throw new Exception($"Couldn't get pointer to pMAPADDRESS_add");
                UInt64 MAPADDRESS_add = this.HaloMemoryService.ReadWrite.ReadQword(pMAPADDRESS_add) ?? throw new Exception("Couldn't read value at pMAPADDRESS_add");

                // Get map address of scenario tag, and of the triggers field within that scenario tag.
                ReadWrite.Pointer pMAPADDRESS_scenarioTag = (ReadWrite.Pointer?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_MAPADDRESS_scenarioTag", haloState) ?? throw new Exception($"Couldn't get pointer to pMAPADDRESS_scenarioTag");
                UInt64 MAPADDRESS_scenarioTag = this.HaloMemoryService.ReadWrite.ReadQword(pMAPADDRESS_scenarioTag) ?? throw new Exception("Couldn't read value at pMAPADDRESS_scenarioTag");
                ReadWrite.Pointer pMAPADDRESS_triggerArray = (ReadWrite.Pointer?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_MAPADDRESS_triggerArray", haloState) ?? throw new Exception($"Couldn't get pointer to pMAPADDRESS_triggerArray");
                UInt64 MAPADDRESS_triggerArray = this.HaloMemoryService.ReadWrite.ReadQword(pMAPADDRESS_triggerArray) ?? throw new Exception("Couldn't read value at pMAPADDRESS_triggerArray");

                // Finally, add and sub the add and sub values from the map addresses to get the real address of each.
                IntPtr pScenarioTag = (IntPtr)(MAPADDRESS_scenarioTag - MAPADDRESS_sub + MAPADDRESS_add);
                IntPtr pTriggerArray = (IntPtr)(MAPADDRESS_triggerArray - MAPADDRESS_sub + MAPADDRESS_add);

                // Add OffsetTriggerCount to ScenarioTag, and read the value to get the count of triggers in the triggerArray
                int OffsetTriggerCount = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetTriggerCount", haloState) ?? throw new Exception($"Couldn't get OffsetTriggerCount");
                triggerDataInfo.TriggerCount = this.HaloMemoryService.ReadWrite.ReadInteger(IntPtr.Add(pScenarioTag, OffsetTriggerCount)) ?? throw new Exception("Couldn't read value of triggerCount");

                // Add OffsetFirstTrigger to pTriggerArray, and convert to ulong
                int OffsetFirstTrigger = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetFirstTrigger", haloState) ?? throw new Exception($"Couldn't get OffsetFirstTrigger");
                triggerDataInfo.pFirstTrigger = (UInt64)IntPtr.Add(pTriggerArray, OffsetFirstTrigger);

                // Load all the other relavent offsets
                triggerDataInfo.OffsetTriggerName = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetTriggerName", haloState) ?? throw new Exception($"Couldn't get OffsetTriggerName");
                triggerDataInfo.TriggerNameLength = (ulong?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_TriggerNameLength", haloState) ?? throw new Exception($"Couldn't get TriggerNameLength");
                triggerDataInfo.TriggerStride = (ulong?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_TriggerStride", haloState) ?? throw new Exception($"Couldn't get TriggerDataSize");

                triggerDataInfo.OffsetTriggerCenterPosition = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetTriggerCenterPosition", haloState) ?? throw new Exception($"Couldn't get OffsetTriggerCenterPosition");
                triggerDataInfo.OffsetTriggerExtent = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetTriggerExtent", haloState) ?? throw new Exception($"Couldn't get OffsetTriggerExtent");
                triggerDataInfo.OffsetTriggerRotation = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetTriggerRotation", haloState) ?? throw new Exception($"Couldn't get OffsetTriggerRotation");
                triggerDataInfo.OffsetTriggerUpVector = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetTriggerUpVector", haloState) ?? throw new Exception($"Couldn't get OffsetTriggerUpVector");

                triggerDataInfo.OffsetTriggerTestValue = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetTriggerTestValue", haloState) ?? throw new Exception($"Couldn't get OffsetTriggerTestValue");
                triggerDataInfo.TriggerTestValue = (uint?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_TriggerTestValue", haloState) ?? throw new Exception($"Couldn't get TriggerTestValue");

                ReadWrite.Pointer Tickcounter = (ReadWrite.Pointer?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_CurrentTickCount", haloState) ?? throw new Exception($"Couldn't get pointer to CurrentTickCount");
                triggerDataInfo.Tickcounter = (UInt64)this.HaloMemoryService.ReadWrite.ResolvePointer(Tickcounter);

                ReadWrite.Pointer TriggerHitTest = (ReadWrite.Pointer?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_TriggerHitTest", haloState) ?? throw new Exception($"Couldn't get pointer to TriggerHitTest");
                triggerDataInfo.pTriggerHitTest = (UInt64)this.HaloMemoryService.ReadWrite.ResolvePointer(TriggerHitTest);

                returnVal = this.CallInternalFunction("LoadTriggerData", triggerDataInfo);
                if (returnVal != InternalReturn.True) throw new Exception("Failed calling LoadTriggerData: " + returnVal);
            }


            // Part 2: Hook TriggerSearch (the game function that checks if objects are in triggers)
            //DetourInfoObject playerDataAdvDetourInfo = (DetourInfoObject)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_PlayerDataAdvanced_DetourInfo", haloState) ?? throw new Exception($"Couldn't get pointer to PlayerDataAdvanced_DetourInfo");
            //if (this.HaloMemoryService.DetourCheckOG(playerDataAdvDetourInfo))
            //{
            //    playerDataAdvDetourHandle = this.HaloMemoryService.DetourApply(playerDataAdvDetourInfo);
            //    System.Threading.Thread.Sleep(50); // give time for entityloop to be run so playerdata is populated for whatever is calling this
            //}
            //else
            //{
            //    // Commenting this out cos.. well if not OG it's PROBABLY already applied, thus no need to apply it again eh
            //    //this.HaloMemoryService.DetourRemove(playerDataAdvDetourInfo, playerDataAdvDetourHandle);
            //    //playerDataAdvDetourHandle = null;
            //    //playerDataAdvDetourHandle = this.HaloMemoryService.DetourApply(playerDataAdvDetourInfo);
            //}


        }




    }
}
