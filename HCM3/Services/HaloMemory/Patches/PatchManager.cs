using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Helpers;
using BurntMemory;
using System.Diagnostics;
using HCM3.Services.HaloMemory;

namespace HCM3.Services.HaloMemory
{
    public class PatchManager
    {
        // This if for patches that don't actually modify game behaviour, but are useful to the function of HCM
        // Example: patching the game so it doesn't check the checksums on checkpoints (allows people to load checkopints made by other players)

        public HaloMemoryService HaloMemoryService;
        public DataPointersService DataPointersService;


        public PatchManager(HaloMemoryService haloMemoryService, DataPointersService dataPointersService)
        { 
        this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;

            this.SetupPatches();
            HaloStateEvents.HALOSTATECHANGED_EVENT += ApplyPatches;
        }

        public void SetupPatches()
        {
            ListOfPatches = new();

            //todo add patch for h1
            //todo change patches to just store player addressess + viewangle address + tickcounter address, not the data itself
            ListOfPatches.Add("H2PlayerData", new PatchStateObject(GameStateEnum.Halo2, "H2_PlayerData_DetourInfo", true));
            ListOfPatches.Add("H3PlayerData", new PatchStateObject(GameStateEnum.Halo3, "H3_PlayerData_DetourInfo", true));
            ListOfPatches.Add("ODPlayerData", new PatchStateObject(GameStateEnum.Halo3ODST, "OD_PlayerData_DetourInfo", true));
            ListOfPatches.Add("HRPlayerData", new PatchStateObject(GameStateEnum.HaloReach, "HR_PlayerData_DetourInfo", true));

            ListOfPatches.Add("H3ChecksumFix", new PatchStateObject(GameStateEnum.Halo3, "H3_ChecksumFix_PatchInfo", false));
            ListOfPatches.Add("ODChecksumFix", new PatchStateObject(GameStateEnum.Halo3ODST, "OD_ChecksumFix_PatchInfo", false));
            ListOfPatches.Add("HRChecksumFix", new PatchStateObject(GameStateEnum.HaloReach, "HR_ChecksumFix_PatchInfo", false));
            ListOfPatches.Add("H4ChecksumFix", new PatchStateObject(GameStateEnum.Halo4, "H4_ChecksumFix_PatchInfo", false));

        }

        private bool errorShown = false;

        private object lockApplyPatches = new object();
        public void ApplyPatches(object? sender, HaloStateEvents.HaloStateChangedEventArgs e)
        {
            Trace.WriteLine("Got here eh 1");
            lock (lockApplyPatches)
            {
                HaloState haloState = e.NewHaloState;
                Trace.WriteLine("Got here eh 2");
                //if (haloState.GameState == GameStateEnum.Unattached || haloState.GameState == GameStateEnum.Menu) return;

                foreach (KeyValuePair<string, PatchStateObject> kvp in ListOfPatches)
                {
                    try
                    {
                        if (haloState.GameState == kvp.Value.Game)
                        {
                            Trace.WriteLine("Found patch (" + kvp.Key + ") that we want to make sure is applied.");
                            if (kvp.Value.IsDetour)
                            {
                               
                                DetourInfoObject detourInfoObject = (DetourInfoObject)this.DataPointersService.GetRequiredPointers(kvp.Value.PointerName, haloState);
                                bool originalCodeIsOriginal = this.HaloMemoryService.DetourCheckOG(detourInfoObject);


                                if (!originalCodeIsOriginal)
                                {
                                    IntPtr? handleToPreviousDetour = null;
                                    if (kvp.Value.Applied)
                                    {
                                        handleToPreviousDetour = kvp.Value.PatchHandle;
                                    }
                                    this.HaloMemoryService.DetourRemove(detourInfoObject, handleToPreviousDetour);
                                }


                                IntPtr detourHandle = this.HaloMemoryService.DetourApply(detourInfoObject);
                                kvp.Value.PatchHandle = detourHandle;
                                kvp.Value.Applied = true;
                            }
                            else //simple patch
                            {

                                if (!IsPatchApplied(haloState, kvp.Key, false))
                                {
                                    Trace.WriteLine("Patch (" + kvp.Key + ") wasn't applied so applying now.");
                                    ApplyPatch(haloState, kvp.Key, false);
                                }
                                kvp.Value.Applied = true;
                            }

                        }
                    }
                    catch (Exception ex)
                    {
                        Trace.WriteLine("Error applying patch (" + kvp.Key + "): " + ex.ToString());
                        if (!errorShown)
                        {
                            errorShown = true;
                            System.Windows.MessageBox.Show("An error occured while applying a HCM patch (" + kvp.Key + "), this may cause some cheat functions to not work correctly. Error: \n\n" + ex.ToString());
                        }

                    }
                }
            }
 
        }

        public Dictionary<string, PatchStateObject> ListOfPatches { get; set; }


        private object lockApplyPatch = new object();
        public void ApplyPatch(HaloState haloState, string patchName, bool isDetour)
        {
            lock (lockApplyPatch)
            {
                PatchStateObject ourPatch = ListOfPatches[patchName];
                if (isDetour)
                {

                    DetourInfoObject detourInfoObject = (DetourInfoObject)this.DataPointersService.GetRequiredPointers(ourPatch.PointerName, haloState);
                    bool originalCodeIsOriginal = this.HaloMemoryService.DetourCheckOG(detourInfoObject);


                    if (!originalCodeIsOriginal)
                    {
                        this.HaloMemoryService.DetourRemove(detourInfoObject, null);
                    }

                    IntPtr detourHandle = this.HaloMemoryService.DetourApply(detourInfoObject);
                    ourPatch.PatchHandle = detourHandle;
                    ourPatch.Applied = true;
                }
                else //simple patch
                {
                    PatchInfo patchInfo = (PatchInfo)this.DataPointersService.GetRequiredPointers(ourPatch.PointerName, haloState);
                    this.HaloMemoryService.ReadWrite.WriteBytes(patchInfo.OriginalCodeLocation, patchInfo.PatchedCodeBytes, true);
                }
            }
        }

        public bool IsPatchApplied(HaloState haloState, string patchName, bool isDetour)
        {
            PatchStateObject ourPatch = ListOfPatches[patchName];

            if (isDetour)
            {
                DetourInfoObject detourInfoObject = (DetourInfoObject)this.DataPointersService.GetRequiredPointers(ourPatch.PointerName, haloState);
                return !this.HaloMemoryService.DetourCheckOG(detourInfoObject);
            }
            else //simple patch
            {
                PatchInfo patchInfo = (PatchInfo)this.DataPointersService.GetRequiredPointers(ourPatch.PointerName, haloState);
                bool codeIsPatched = true;

                byte[] currentCode = this.HaloMemoryService.ReadWrite.ReadBytes(patchInfo.OriginalCodeLocation, patchInfo.OriginalCodeBytes.Length);

                for (int i = 0; i < currentCode.Length; i++)
                {
                    if (currentCode[i] != patchInfo.PatchedCodeBytes[i])
                    {
                        codeIsPatched = false;
                        break;
                    }
                }
                return codeIsPatched;
            }
        }

        public void RemovePatch(HaloState haloState, string patchName, bool isDetour)
        {
            PatchStateObject ourPatch = ListOfPatches[patchName];

            if (isDetour)
            {
                DetourInfoObject detourInfoObject = (DetourInfoObject)this.DataPointersService.GetRequiredPointers(ourPatch.PointerName, haloState);
                bool originalCodeIsOriginal = this.HaloMemoryService.DetourCheckOG(detourInfoObject);

                if (!originalCodeIsOriginal)
                {
                    this.HaloMemoryService.DetourRemove(detourInfoObject, ourPatch.PatchHandle);
                }

                ourPatch.Applied = false;
            }
            else //simple patch
            {
                PatchInfo patchInfo = (PatchInfo)this.DataPointersService.GetRequiredPointers(ourPatch.PointerName, haloState);
                this.HaloMemoryService.ReadWrite.WriteBytes(patchInfo.OriginalCodeLocation, patchInfo.OriginalCodeBytes, true);
            }
        }

        //only run on HCM shutdown
        public void RemoveAllPatches()
        {
            HaloState haloState = this.HaloMemoryService.HaloMemoryManager.UpdateHaloState();

            foreach (KeyValuePair<string, PatchStateObject> kvp in ListOfPatches)
            {
                try
                {
                    if (haloState.GameState == kvp.Value.Game)
                    {
                        DetourInfoObject detourInfoObject = (DetourInfoObject)this.DataPointersService.GetRequiredPointers(kvp.Value.PointerName, haloState);
                        bool originalCodeIsOriginal = this.HaloMemoryService.DetourCheckOG(detourInfoObject);

                        if (!originalCodeIsOriginal)
                        {
                            this.HaloMemoryService.DetourRemove(detourInfoObject, null);
                        }

                        kvp.Value.Applied = false;


                    }
                }
                catch (Exception ex)
                {
                    Trace.WriteLine("Error removing patch: " + ex.ToString());
                }
            }


        }


    }
}
