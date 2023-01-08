using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Models;
using HCM3.Helpers;
using BurntMemory;
using System.Diagnostics;
using System.Threading;

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
                    "Expected: " + Dictionaries.GameToDLLname[selectedGame] + "\n" +
                    "Actual: " + Dictionaries.GameToDLLname[(int)game] + "\n" +
                    "Are you sure you have the right tab selected in HCM?"
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


        public bool PrintMessage(string message, bool needToDisableCheckpointText = false)
        {
            int loadedGame = GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_CurrentTickCount");
            requiredPointerNames.Add($"{gameAs2Letters}_PrintMessageText");
            requiredPointerNames.Add($"{gameAs2Letters}_PrintMessageTickCount");
            requiredPointerNames.Add($"{gameAs2Letters}_PrintMessageFlags");
            requiredPointerNames.Add($"{gameAs2Letters}_PrintMessageFlagValues");

            Dictionary<string, object> requiredPointers = GetRequiredPointers(requiredPointerNames);


            byte[] messageData = Encoding.Unicode.GetBytes(message.PadRight(64).Substring(0, 64));
            
            
            
            byte[]? currentTickCount = this.HaloMemoryService.ReadWrite.ReadBytes((ReadWrite.Pointer?)requiredPointers["CurrentTickCount"], 4);

            lock (this.HaloMemoryService.HaloState.modules)
            {
                if ((ReadWrite.Pointer?)requiredPointers["CurrentTickCount"] == null)
                {
                    Trace.WriteLine("PTR WAS NULL");
                    ReadWrite.Pointer? mainModulePtr = this.HaloMemoryService.HaloState.modules["main"];
                    string mainModuleInfo = mainModulePtr != null ? (mainModulePtr.Address != null ? mainModulePtr.Address.Value.ToString("X") : "mainModule  address is null!!") : "mainModulePtr was null!!";

                    string gameDLL = Dictionaries.GameToDLLname[(int)loadedGame];
                    Trace.WriteLine("gameDLL: " + gameDLL);
                    ReadWrite.Pointer? gameDLLPtr = this.HaloMemoryService.HaloState.modules[gameDLL];
                    IntPtr? gameDLLresolved = this.HaloMemoryService.ReadWrite.ResolvePointer(gameDLLPtr);
                    string gameDLLInfo = gameDLLPtr != null ? (gameDLLresolved != null ? gameDLLresolved.Value.ToString("X") : "gameDLLresolved is null!!") : "gameDLLPtr was null!!";
                    throw new Exception("CurrentTickCount pointer was null. Why tho? Game version: " + this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion + ", game type: " + this.HaloMemoryService.HaloState.MCCType + ", mainModule info: " + mainModuleInfo + ", gameDLL info: " + gameDLLInfo);

                }

                if (currentTickCount == null)
                {
                    Trace.WriteLine("BYTE WAS NULL");
                    ReadWrite.Pointer? mainModulePtr = this.HaloMemoryService.HaloState.modules["main"];
                    string mainModuleInfo = mainModulePtr != null ? (mainModulePtr.Address != null ? mainModulePtr.Address.Value.ToString("X") : "mainModule address is null!!") : "mainModulePtr was null!!";
                    mainModuleInfo = mainModuleInfo + ", AHH: " + this.HaloMemoryService.HaloState.MainModuleBaseAddress;

                    string gameDLL = Dictionaries.GameToDLLname[(int)loadedGame];
                    Trace.WriteLine("gameDLL: " + gameDLL);
                    ReadWrite.Pointer? gameDLLPtr = this.HaloMemoryService.HaloState.modules[gameDLL];
                    IntPtr? gameDLLresolved = this.HaloMemoryService.ReadWrite.ResolvePointer(gameDLLPtr);
                    string gameDLLInfo = gameDLLPtr != null ? (gameDLLresolved != null ? gameDLLresolved.Value.ToString("X") : "gameDLLresolved is null!!") : "gameDLLPtr was null!!";

                    string errmessage = ("couldn't read tickcount.Why tho ? Game version: " + this.HaloMemoryService.HaloState.CurrentAttachedMCCVersion + ", game type: " + this.HaloMemoryService.HaloState.MCCType + ", mainModule info: " + mainModuleInfo + ", gameDLL info: " + gameDLLInfo);

                    ReadWrite.Pointer? currentTickCountptr = (ReadWrite.Pointer?)requiredPointers["CurrentTickCount"];
                    string ah = "ctcptr modulename: " + currentTickCountptr.Modulename;
                    ah = ah + ", ctcptr offset 0: " + currentTickCountptr.Offsets[0];
                    ah = ah + ", resolved ctc location: " + this.HaloMemoryService.ReadWrite.ResolvePointer(currentTickCountptr).Value.ToString("X");

                    errmessage = ah + ", " + errmessage;

                    Trace.WriteLine(errmessage);
                    throw new Exception(errmessage);


                }
            }

            ReadWrite.Pointer[] printMessageFlagPointers = (ReadWrite.Pointer[])requiredPointers["PrintMessageFlags"];
            byte[] printMessageFlagValues = (byte[])requiredPointers["PrintMessageFlagValues"];

            if (printMessageFlagPointers.Length != printMessageFlagValues.Length)
            {
                throw new Exception("Couldn't print message, FlagPointerCount misaligned with values");
            }


            bool success = false;
            if (needToDisableCheckpointText)
            {
                ReadWrite.Pointer CPMessageCall = (ReadWrite.Pointer)GetRequiredPointers($"{gameAs2Letters}_CPMessageCall");
                int CPMessageCallLength = (int)GetRequiredPointers($"{gameAs2Letters}_CPMessageCallLength");

                byte[] ogBytes = this.HaloMemoryService.ReadWrite.ReadBytes(CPMessageCall, CPMessageCallLength);

                if (ogBytes == null) throw new Exception("ogBytes was null");

                if (ogBytes[0] != 0x90)
                {
                    byte[] nopBytes = new byte[CPMessageCallLength];
                    Array.Fill(nopBytes, (byte)0x90);
                    this.HaloMemoryService.ReadWrite.WriteBytes(CPMessageCall, nopBytes, true);
                }


                success = this.HaloMemoryService.ReadWrite.WriteBytes((ReadWrite.Pointer)requiredPointers["PrintMessageTickCount"], currentTickCount, false);
                success = success && this.HaloMemoryService.ReadWrite.WriteBytes((ReadWrite.Pointer)requiredPointers["PrintMessageText"], messageData, false);

                for (int i = 0; i < printMessageFlagPointers.Length; i++)
                {
                    success = success && this.HaloMemoryService.ReadWrite.WriteByte(printMessageFlagPointers[i], printMessageFlagValues[i], false);
                }


                new Thread(() =>
                {
                    Thread.CurrentThread.IsBackground = true;
                    System.Threading.Thread.Sleep(50);
                    byte? test = this.HaloMemoryService.ReadWrite.ReadByte(CPMessageCall);
                    if (test == 0x90)
                    {
                        this.HaloMemoryService.ReadWrite.WriteBytes(CPMessageCall, ogBytes, true);
                    }
                }).Start();
               



            }
            else
            {

                success = this.HaloMemoryService.ReadWrite.WriteBytes((ReadWrite.Pointer)requiredPointers["PrintMessageTickCount"], currentTickCount, false);
                success = success && this.HaloMemoryService.ReadWrite.WriteBytes((ReadWrite.Pointer)requiredPointers["PrintMessageText"], messageData, false);

                for (int i = 0; i < printMessageFlagPointers.Length; i++)
                {

                    success = success && this.HaloMemoryService.ReadWrite.WriteByte(printMessageFlagPointers[i], printMessageFlagValues[i], false);
                }
            }




            return success;

        }






    }
}
