using HCMExternal.Services.CheckpointServiceNS;
using HCMExternal.ViewModels;
using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using HCMExternal.ViewModels.Commands;
using HCMExternal.Helpers.DictionariesNS;
using HCMExternal.Services.MCCStateServiceNS;
using System.Windows;

namespace HCMExternal.Services.InterprocServiceNS
{
    public class InterprocService
    {

        private CheckpointViewModel CheckpointViewModel;

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        private struct checkpointInjectInfoExternal
        {
            public checkpointInjectInfoExternal() { }
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)] public string checkpointFilePath = "";
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)] public string levelCode = "";
            [MarshalAs(UnmanagedType.I8)] public Int64 difficulty = 0;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)] public string version = "";
            [MarshalAs(UnmanagedType.U1)] public  bool requestFailed = true;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct checkpointDumpInfoExternal
        {
            public checkpointDumpInfoExternal() { }
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)] public string dumpFolderPath = "";
            [MarshalAs(UnmanagedType.U1)] public bool requestFailed = true;
        }

        private delegate Int64 PFN_HEARTBEATCALLBACK();
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        private static extern void HeartbeatEventSubscribe(PFN_HEARTBEATCALLBACK callback);
        private PFN_HEARTBEATCALLBACK DEL_HEARTBEATCALLBACK;

        private delegate void PFN_ERRORCALLBACK(string str);
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        private static extern void ErrorEventSubscribe(PFN_ERRORCALLBACK callback);
        private PFN_ERRORCALLBACK DEL_ERRORCALLBACK;

        private delegate void PFN_LOGCALLBACK([MarshalAs(UnmanagedType.LPWStr)] string str);
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        private static extern void LogEventSubscribe(PFN_LOGCALLBACK callback);
        private PFN_LOGCALLBACK DEL_LOGCALLBACK;

        private delegate checkpointInjectInfoExternal PFN_InternalRequestsInjectInfoCALLBACK(int game);
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        private static extern void InternalRequestsInjectInfoEventSubscribe(PFN_InternalRequestsInjectInfoCALLBACK callback);
        private PFN_InternalRequestsInjectInfoCALLBACK DEL_InternalRequestsInjectInfoCALLBACK;

        private delegate checkpointDumpInfoExternal PFN_InternalRequestsDumpInfoCALLBACK(int game);
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        private static extern void InternalRequestsDumpInfoEventSubscribe(PFN_InternalRequestsDumpInfoCALLBACK callback);
        private PFN_InternalRequestsDumpInfoCALLBACK DEL_InternalRequestsDumpInfoCALLBACK;

        //todo: sendinject/senddump stuff;


        [DllImport("HCMInterproc.dll")] private static extern bool SetupInternal();

        public InterprocService(CheckpointViewModel checkpointViewModel)
        {
            CheckpointViewModel = checkpointViewModel;

            // NOTE: do NOT put temporaries into the subscribes. They will eventually get garbage collected, then Interproc will eventually call them leading to KABOOM. I learned the hard way.

            // Subscribe to interproc events
            DEL_ERRORCALLBACK = new PFN_ERRORCALLBACK(ERRORCALLBACK);
            ErrorEventSubscribe(DEL_ERRORCALLBACK);

            DEL_LOGCALLBACK = new PFN_LOGCALLBACK(LOGCALLBACK);
            LogEventSubscribe(DEL_LOGCALLBACK);

            DEL_HEARTBEATCALLBACK = new PFN_HEARTBEATCALLBACK(HEARTBEATCALLBACK);
            HeartbeatEventSubscribe(DEL_HEARTBEATCALLBACK);

            // internal will fire these when it needs info for inject/dump
            DEL_InternalRequestsInjectInfoCALLBACK = new PFN_InternalRequestsInjectInfoCALLBACK(getInjectInfo);
            InternalRequestsInjectInfoEventSubscribe(DEL_InternalRequestsInjectInfoCALLBACK);

            DEL_InternalRequestsDumpInfoCALLBACK = new PFN_InternalRequestsDumpInfoCALLBACK(getDumpInfo);
            InternalRequestsDumpInfoEventSubscribe(DEL_InternalRequestsDumpInfoCALLBACK);

            DumpCommand.DumpEvent += SendDumpCommand;
            InjectCommand.InjectEvent += SendInjectCommand;


        }


        public bool Setup()
        {
            return SetupInternal();
        }

        private enum HCMCommand
        {
            None = 0,
            Inject = 1,
            Dump = 2
        }

        private static HCMCommand currentCommand = HCMCommand.None;

        private void ERRORCALLBACK(string message)
        {
            Log.Error("A HCMInterproc error occured: " + message);
        }

        private void LOGCALLBACK(string message)
        {
            Log.Verbose("Marco");
            Log.Verbose("HCMInterproc: " + message);
            Log.Verbose("Polo");
        }

        private Int64 HEARTBEATCALLBACK()
        {
            Log.Verbose("Recieved heartbeat command from internal! Gonna send it: " + currentCommand.ToString());
            if (currentCommand == HCMCommand.Dump)
            {
                currentCommand = HCMCommand.None;
                return (Int64)HCMCommand.Dump;
            }
            if (currentCommand == HCMCommand.Inject)
            {
                currentCommand = HCMCommand.None;
                return (Int64)HCMCommand.Inject;
            }

            return (Int64)currentCommand; // none
        }


        private void SendInjectCommand()
        {
            Log.Information("Sending inject command to internal");
            currentCommand = HCMCommand.Inject;
        }

        private void SendDumpCommand()
        {
            Log.Information("Sending dump command to internal");
            currentCommand = HCMCommand.Dump;
        }

        private checkpointInjectInfoExternal getInjectInfo(int game)
        {
            Log.Information("getInjectInfo");
            checkpointInjectInfoExternal working = new(); // requestFailed is initialized to true for if we early return on error

            // Set checkpointviewmodel tab to match game
            if (!Enum.IsDefined(typeof(HaloTabEnum), game))
            {
                Log.Error("HaloTabEnum was not defined for game: " + game);
                return working;
            }
            CheckpointViewModel.RequestTabChange((HaloTabEnum)game);

            if (CheckpointViewModel.SelectedSaveFolder?.SaveFolderPath == null || CheckpointViewModel.SelectedCheckpoint == null)
            {
                Log.Error("SaveFolderPath or SelectedCheckpoint was null. Game: " + game + ", SelectedSaveFolder == null: " + (CheckpointViewModel.SelectedSaveFolder == null) + ", SelectedCheckpoint == null" + (CheckpointViewModel.SelectedCheckpoint == null));
                return working;
            }
            working.checkpointFilePath = CheckpointViewModel.SelectedSaveFolder.SaveFolderPath + CheckpointViewModel.SelectedCheckpoint.CheckpointName;
            Log.Debug("Set checkpointFilePath to " + working.checkpointFilePath);

            if (CheckpointViewModel.SelectedCheckpoint.LevelName == null)
            {
                Log.Error("LevelName was null!");
                return working;
            }
            working.levelCode = CheckpointViewModel.SelectedCheckpoint.LevelName;

            if (CheckpointViewModel.SelectedCheckpoint.Difficulty == null)
            {
                Log.Error("Difficulty was null!");
                return working;
            }
            working.difficulty = (int)CheckpointViewModel.SelectedCheckpoint.Difficulty;

            if (CheckpointViewModel.SelectedCheckpoint.GameVersion == null)
            {
                // This is fine
                Log.Debug("Game version was null but that can happen");
                working.version = "null";
            }
            else
            {
                working.version = CheckpointViewModel.SelectedCheckpoint.GameVersion;
            }

            // Finally, set requestFailed to false and return.
            working.requestFailed = false;
            return working;


        }


        private checkpointDumpInfoExternal getDumpInfo(int game)
        {
            Log.Information("getDumpInfo");
            checkpointDumpInfoExternal working = new(); // requestFailed is initialized to true for if we early return on error

            // Set checkpointviewmodel tab to match game
            if (!Enum.IsDefined(typeof(HaloTabEnum), game))
            {
                Log.Error("HaloTabEnum was not defined for game: " + game);
                return working;
            }
            CheckpointViewModel.RequestTabChange((HaloTabEnum)game);

            if (CheckpointViewModel.SelectedSaveFolder == null || CheckpointViewModel.SelectedSaveFolder.SaveFolderPath == null)
            {
                Log.Error("SelectedSaveFolder was null!");
                return working;
            }
            working.dumpFolderPath = CheckpointViewModel.SelectedSaveFolder.SaveFolderPath;
            Log.Debug("Setting dumpInfo savefolderpath to : " + CheckpointViewModel.SelectedSaveFolder.SaveFolderPath);

            // Finally, set requestFailed to false and return.
            working.requestFailed = false;
            return working;
        }

    }
}
