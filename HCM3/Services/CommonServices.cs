using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Models;
using HCM3.Helpers;
using BurntMemory;
using System.Diagnostics;

namespace HCM3.Services
{
    public partial class CommonServices
    {
        public HaloMemoryService HaloMemoryService { get; init; }
        public DataPointersService DataPointersService { get; init; }
        public CommonServices(HaloMemoryService haloMemoryService, DataPointersService dataPointersService)
        {
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
        }

        public int GetLoadedGame()
        {
            this.HaloMemoryService.HaloState.UpdateHaloState();
            int game = this.HaloMemoryService.HaloState.CurrentHaloState;
            if (game < 0 || game > 6)
            {
                Dictionaries.HaloStateEnum gameEnum = (Dictionaries.HaloStateEnum)game;
                throw new Exception("Game wasn't valid! AttachState: " + gameEnum.ToString());
            }
            return game;
        }

        public void CheckGameIsAligned(int selectedGame)
        {
            // Update HaloState
            this.HaloMemoryService.HaloState.UpdateHaloState();

            // Check that we're loaded into the game that matches the tab whose checkpoint we're trying to dump
            Dictionaries.HaloStateEnum game = (Dictionaries.HaloStateEnum)this.HaloMemoryService.HaloState.CurrentHaloState;

            if ((int)game != selectedGame)
            {
                throw new InvalidOperationException("HCM didn't detect that you were in the right game: \n" +
                    "Expected: " + Dictionaries.GameTo2LetterGameCode[selectedGame] + "\n" +
                    "Actual: " + game.ToString()
                    );
            }


            string? MCCversion = this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion;

            if (MCCversion == null)
            {
                throw new InvalidOperationException("HCM couldn't detect which version of MCC was running");
            }
        }


        //override for if you just want a single pointer
        public object GetRequiredPointers(string requiredPointerName)
        {
            string? MCCversion = this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion;

            if (MCCversion == null)
            {
                throw new InvalidOperationException("HCM couldn't detect which version of MCC was running");
            }

            object? pointer = this.DataPointersService.GetPointer(requiredPointerName, MCCversion);

            return pointer ?? throw new InvalidOperationException("HCM doesn't have offsets loaded to perform this operation with this version of MCC."
                    + $"\nSpecifically: {requiredPointerName}"
                    ); ;
        }

        public Dictionary<string, object> GetRequiredPointers(List<string> requiredPointerNames, bool throwOnFailure = true)
        {
            string? MCCversion = this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion;

            if (MCCversion == null)
            {
                throw new InvalidOperationException("HCM couldn't detect which version of MCC was running");
            }

            Dictionary<string, object> requiredPointers = new();
            foreach (string requiredPointerName in requiredPointerNames)
            {
                object? pointer = this.DataPointersService.GetPointer(requiredPointerName, MCCversion);
                if (pointer == null)
                {
                    if (throwOnFailure)
                    {
                        throw new InvalidOperationException("HCM doesn't have offsets loaded to perform this operation with this version of MCC."
                            + $"\nSpecifically: {requiredPointerName}"
                            );
                    }
                    else
                    {
                        continue;
                    }

                }
                requiredPointers.Add(requiredPointerName[3..], pointer); // Cut off the gamecode part so we can just refer to the rest of the name later
            }
            return requiredPointers;
        }


        public bool PrintMessage(string message, int selectedGame)
        {
            CheckGameIsAligned(selectedGame);
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)selectedGame];

            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_CurrentTickCount");
            requiredPointerNames.Add($"{gameAs2Letters}_PrintMessageText");
            requiredPointerNames.Add($"{gameAs2Letters}_PrintMessageTickCount");
            requiredPointerNames.Add($"{gameAs2Letters}_PrintMessageFlags");
            requiredPointerNames.Add($"{gameAs2Letters}_PrintMessageFlagValues");

            Dictionary<string, object> requiredPointers = GetRequiredPointers(requiredPointerNames);


            byte[] messageData = Encoding.Unicode.GetBytes(message.PadRight(64).Substring(0, 64));
            
            
            
            byte[]? currentTickCount = this.HaloMemoryService.ReadWrite.ReadBytes((ReadWrite.Pointer?)requiredPointers["CurrentTickCount"], 4);

            if (currentTickCount == null)
            {
                throw new Exception("couldn't read Tickcount");
            }

            ReadWrite.Pointer[] printMessageFlagPointers = (ReadWrite.Pointer[])requiredPointers["PrintMessageFlags"];
            byte[] printMessageFlagValues = (byte[])requiredPointers["PrintMessageFlagValues"];

            if (printMessageFlagPointers.Length != printMessageFlagValues.Length)
            {
                throw new Exception("Couldn't print message, FlagPointerCount misaligned with values");
            }



            bool success = this.HaloMemoryService.ReadWrite.WriteBytes((ReadWrite.Pointer)requiredPointers["PrintMessageTickCount"], currentTickCount, false);
            success = success && this.HaloMemoryService.ReadWrite.WriteBytes((ReadWrite.Pointer)requiredPointers["PrintMessageText"], messageData, false);

            for (int i = 0; i < printMessageFlagPointers.Length; i++)
            {

                success = success && this.HaloMemoryService.ReadWrite.WriteByte(printMessageFlagPointers[i], printMessageFlagValues[i], false);
            }



            return success;

        }






    }
}
