using HCMExternal.Models;
using Serilog;
using System;
using System.Runtime.InteropServices.Marshalling;
using System.Runtime.InteropServices;



namespace HCMExternal.Services.Interproc.Impl
{

    public partial class InterprocService : IInterprocService
    {


        private delegate void PFN_ERRORCALLBACK([MarshalAs(UnmanagedType.LPWStr)] string str);
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        private static extern void ErrorEvent_Subscribe(PFN_ERRORCALLBACK callback);
        private PFN_ERRORCALLBACK DEL_ERRORCALLBACK;

        private delegate void PFN_LOGCALLBACK([MarshalAs(UnmanagedType.LPWStr)] string str);
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        private static extern void LogEvent_Subscribe(PFN_LOGCALLBACK callback);
        private PFN_LOGCALLBACK DEL_LOGCALLBACK;

        [LibraryImport("HCMInterproc.DLL", StringMarshallingCustomType = typeof(Utf8StringMarshaller))]
        private static partial void updateSelectedCheckpoint([MarshalAs(UnmanagedType.Bool)] bool nullData, int game = 0, string name = "", string path = "", string levelcode = "", string gameVersion = "", int difficulty = 0);

        [LibraryImport("HCMInterproc.DLL", StringMarshallingCustomType = typeof(Utf8StringMarshaller))]
        private static partial void updateSelectedFolder(int SFgame, string SFname, string SFpath);

        [LibraryImport("HCMInterproc.DLL", StringMarshallingCustomType = typeof(Utf8StringMarshaller))]
        private static partial ushort initialiseInterproc(
            [MarshalAs(UnmanagedType.Bool)] bool CPnullData,
            int CPgame, string CPname, string CPpath, string CPlevelcode, string CPgameVersion, int CPdifficulty,
            int SFgame, string SFname, string SFpath
            );

        [LibraryImport("HCMInterproc.DLL")]
        private static partial void queueInjectCommand();

        [LibraryImport("HCMInterproc.DLL")]
        private static partial void resetStateMachine();

        private delegate void PFN_SMSTATUSCALLBACK(int state);
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        private static extern void HookStateChangedEvent_Subscribe(PFN_SMSTATUSCALLBACK callback);
        private PFN_SMSTATUSCALLBACK DEL_SMSTATUSCALLBACK;

        public event EventHandler<IInterprocService.StatusMachineStatusChangedEventArgs> StateMachineStatusChangedHandler = delegate { };

        public event EventHandler<IInterprocService.InterprocErrorEventArgs> InterprocErrorHandler = delegate { };


        public InterprocService()
        {
            // NOTE: do NOT put temporaries into the subscribes. They will eventually get garbage collected, then Interproc will eventually call them leading to KABOOM. I learned the hard way.
            // Subscribe to interproc events
            DEL_ERRORCALLBACK = new PFN_ERRORCALLBACK(ERRORCALLBACK);
            ErrorEvent_Subscribe(DEL_ERRORCALLBACK);


            Log.Verbose("Got this far 1");
            DEL_LOGCALLBACK = new PFN_LOGCALLBACK(LOGCALLBACK);
            LogEvent_Subscribe(DEL_LOGCALLBACK);

            Log.Verbose("Got this far 2");
            DEL_SMSTATUSCALLBACK = new PFN_SMSTATUSCALLBACK(SMSTATUSCALLBACK);
            HookStateChangedEvent_Subscribe(DEL_SMSTATUSCALLBACK);
        }


        public void UpdateSharedMemCheckpoint(HaloGame game, Checkpoint? cp)
        {
            if (cp == null)
                updateSelectedCheckpoint(true);
            else
                updateSelectedCheckpoint(false, game.ToInternalIndex(), cp.CheckpointName, cp.CheckpointPath, cp.LevelName ?? "", cp.GameVersion ?? "", cp.Difficulty ?? 0);
        }
        public void UpdateSharedMemSaveFolder(HaloGame game, SaveFolder sf)
        {
            updateSelectedFolder(game.ToInternalIndex(), sf.SaveFolderName, sf.SaveFolderPath);
        }


        public void UpdateSharedMemQueueInjectCommand()
        {
            queueInjectCommand();
        }

        public void initializeSharedMemory(Checkpoint? cp, SaveFolder sf, HaloGame game)
        {
            if (cp != null)
                initialiseInterproc(false, game.ToInternalIndex(), cp.CheckpointName, cp.CheckpointPath, cp.LevelName ?? "", cp.GameVersion ?? "", cp.Difficulty ?? 0, game.ToInternalIndex(), sf.SaveFolderName, sf.SaveFolderPath);
            else
                initialiseInterproc(true, game.ToInternalIndex(), "", "", "", "", 0, game.ToInternalIndex(), sf.SaveFolderName, sf.SaveFolderPath);
        }


        public void resetStateMachineEx()
        {
            resetStateMachine();
        }


        private void ERRORCALLBACK(string errorMessage)
        {
            Log.Verbose("Handling interproc error event with message: " + errorMessage);
            EventHandler<IInterprocService.InterprocErrorEventArgs> handler = InterprocErrorHandler;
            InterprocErrorHandler(this, new IInterprocService.InterprocErrorEventArgs() { errorMessage = errorMessage });
        }

        private void LOGCALLBACK(string message)
        {
            Log.Verbose("HCMInterproc: " + message);
        }

        private void SMSTATUSCALLBACK(int statusCode)
        {
            Log.Verbose("Handling interproc state machine status changed event with new status: " + statusCode);
            EventHandler<IInterprocService.StatusMachineStatusChangedEventArgs> handler = StateMachineStatusChangedHandler;
            if (Enum.IsDefined(typeof(MCCHookStateEnum), statusCode))
            {
                StateMachineStatusChangedHandler(this, new IInterprocService.StatusMachineStatusChangedEventArgs() { hookState = (MCCHookStateEnum)statusCode });
            }
        }

    }
}
