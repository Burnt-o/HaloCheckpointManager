using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Helpers;
using BurntMemory;
using System.Diagnostics;

namespace HCM3.Services.Trainer
{
    public partial class TrainerServices
    {
        // This if for patches that don't actually modify game behaviour, but are useful to the function of HCM
        // Example: patching the game so it doesn't check the checksums on checkpoints (allows people to load checkopints made by other players)


        public void SetupPatches()
        {
            listOfPatches = new();


            listOfPatches.Add("H2PlayerData", new PatchObject("H2", "H2_PlayerData_DetourInfo"));


        }


        public void ApplyPatches(object? sender, HaloStateEvents.HaloStateChangedEventArgs e)
        {
            return;
            int loadedGame = e.NewHaloState;
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

            

            foreach (KeyValuePair<string, PatchObject> kvp in listOfPatches)
            {
                try
                {
                    if (gameAs2Letters == kvp.Value.Game)
                    {
                        DetourInfoObject detourInfoObject = (DetourInfoObject)this.CommonServices.GetRequiredPointers(kvp.Value.PointerName);
                        bool originalCodeIsOriginal = this.PersistentCheatService.DetourCheckOG(detourInfoObject);


                        if (!originalCodeIsOriginal)
                        {
                            this.PersistentCheatService.DetourRemove(detourInfoObject, null);
                        }


                        IntPtr detourHandle = this.PersistentCheatService.DetourApply(detourInfoObject);
                        kvp.Value.PatchHandle = detourHandle;
                        kvp.Value.Applied = true;


                    }
                }
                catch (Exception ex)
                { 
                Trace.WriteLine("Error applying patch: " + ex.Message);
                }
            }
        }

        public Dictionary<string, PatchObject> listOfPatches { get; set; }


        public void ApplyPatch(string patchName)
        {

                PatchObject ourPatch = listOfPatches[patchName];
                DetourInfoObject detourInfoObject = (DetourInfoObject)this.CommonServices.GetRequiredPointers(ourPatch.PointerName);
                bool originalCodeIsOriginal = this.PersistentCheatService.DetourCheckOG(detourInfoObject);


                if (!originalCodeIsOriginal)
                {
                    this.PersistentCheatService.DetourRemove(detourInfoObject, null);
                }

                IntPtr detourHandle = this.PersistentCheatService.DetourApply(detourInfoObject);
                ourPatch.PatchHandle = detourHandle;
                ourPatch.Applied = true;

        }

        public bool IsPatchApplied(string patchName)
        {
            PatchObject ourPatch = listOfPatches[patchName];
            DetourInfoObject detourInfoObject = (DetourInfoObject)this.CommonServices.GetRequiredPointers(ourPatch.PointerName);
            return !this.PersistentCheatService.DetourCheckOG(detourInfoObject);
        }

        public void RemovePatch(string patchName)
        {
            PatchObject ourPatch = listOfPatches[patchName];
            DetourInfoObject detourInfoObject = (DetourInfoObject)this.CommonServices.GetRequiredPointers(ourPatch.PointerName);
            bool originalCodeIsOriginal = this.PersistentCheatService.DetourCheckOG(detourInfoObject);

            if (!originalCodeIsOriginal)
            {
                this.PersistentCheatService.DetourRemove(detourInfoObject, ourPatch.PatchHandle);
            }

            ourPatch.Applied = false;
        }

        //only run on HCM shutdown
        public void RemovePatches()
        {
            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.HaloMemoryService.HaloState.CurrentHaloState;
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

            foreach (KeyValuePair<string, PatchObject> kvp in listOfPatches)
            {
                try
                {
                    if (gameAs2Letters == kvp.Value.Game)
                    {
                        DetourInfoObject detourInfoObject = (DetourInfoObject)this.CommonServices.GetRequiredPointers(kvp.Value.PointerName);
                        bool originalCodeIsOriginal = this.PersistentCheatService.DetourCheckOG(detourInfoObject);

                        if (!originalCodeIsOriginal)
                        {
                            this.PersistentCheatService.DetourRemove(detourInfoObject, null);
                        }

                        kvp.Value.Applied = false;


                    }
                }
                catch (Exception ex)
                {
                    Trace.WriteLine("Error removing patch: " + ex.Message);
                }
            }


        }


    }
}
