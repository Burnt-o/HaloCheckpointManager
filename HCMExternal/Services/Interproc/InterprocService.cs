using HCMExternal.Helpers.DictionariesNS;
using HCMExternal.Models;
using HCMExternal.Properties;
using HCMExternal.ViewModels;
using HCMExternal.ViewModels.Commands;
using Serilog;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;
using System.Threading.Tasks;


namespace HCMExternal.Services.InterprocServiceNS
{
    public partial class InterprocService
    {

        private CheckpointViewModel CheckpointViewModel;


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
        private static partial void updateSelectedFolder(
        string SFnameH1, string SFpathH1,
        string SFnameH2, string SFpathH2,
        string SFnameH3, string SFpathH3,
        string SFnameOD, string SFpathOD,
        string SFnameHR, string SFpathHR,
        string SFnameH4, string SFpathH4);

        [LibraryImport("HCMInterproc.DLL", StringMarshallingCustomType = typeof(Utf8StringMarshaller))]
        private static partial ushort initialiseInterproc(
            [MarshalAs(UnmanagedType.Bool)] bool CPnullData,
            int CPgame, string CPname, string CPpath, string CPlevelcode, string CPgameVersion, int CPdifficulty,
            string SFnameH1, string SFpathH1,
            string SFnameH2, string SFpathH2,
            string SFnameH3, string SFpathH3,
            string SFnameOD, string SFpathOD,
            string SFnameHR, string SFpathHR,
            string SFnameH4, string SFpathH4
            );

        [LibraryImport("HCMInterproc.DLL")]
        private static partial void queueInjectCommand();

        [LibraryImport("HCMInterproc.DLL")]
        private static partial void resetStateMachine();

        public void resetStateMachineEx() { resetStateMachine(); }

        private delegate void PFN_SMSTATUSCALLBACK(int state);
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        private static extern void HookStateChangedEvent_Subscribe(PFN_SMSTATUSCALLBACK callback);
        private PFN_SMSTATUSCALLBACK DEL_SMSTATUSCALLBACK;


        public InterprocService(CheckpointViewModel checkpointViewModel)
        {
            CheckpointViewModel = checkpointViewModel;

            CheckpointViewModel.PropertyChanged += CheckpointViewModel_PropertyChanged;



            // NOTE: do NOT put temporaries into the subscribes. They will eventually get garbage collected, then Interproc will eventually call them leading to KABOOM. I learned the hard way.
            Log.Verbose("Got this far 0");

            // Subscribe to interproc events
            DEL_ERRORCALLBACK = new PFN_ERRORCALLBACK(ERRORCALLBACK);
            ErrorEvent_Subscribe(DEL_ERRORCALLBACK);


            Log.Verbose("Got this far 1");
            DEL_LOGCALLBACK = new PFN_LOGCALLBACK(LOGCALLBACK);
            LogEvent_Subscribe(DEL_LOGCALLBACK);

            Log.Verbose("Got this far 2");
            DEL_SMSTATUSCALLBACK = new PFN_SMSTATUSCALLBACK(SMSTATUSCALLBACK);
            HookStateChangedEvent_Subscribe(DEL_SMSTATUSCALLBACK);

            //DumpCommand.DumpEvent += () => 
            //{
            //    Log.Information("Sending dump command to internal");
            //    sendDumpCommand();
            //};

            Log.Verbose("Got this far 3");
            InjectCommand.InjectEvent += () =>
            {
                Log.Information("Sending inject command to internal");
                queueInjectCommand();
            };

            Log.Verbose("Got this far 4");

            // Since InterprocService is created after CheckpointViewModel, the deserialised first selected checkpoint & savefolder events will be missed. So we need to manually fire them here.
            //CheckpointViewModel.SelectedCheckpoint = CheckpointViewModel.SelectedCheckpoint;
            //CheckpointViewModel.SelectedSaveFolder = CheckpointViewModel.SelectedSaveFolder;

        }

        public void initInterproc()
        {
            // get info on init cp and sf
            Models.Checkpoint? cp = CheckpointViewModel.SelectedCheckpoint;
            Models.SaveFolder sf = CheckpointViewModel.SelectedSaveFolder;

            (string, string) H1sf = getGameSFData(HaloTabEnum.Halo1);
            (string, string) H2sf = getGameSFData(HaloTabEnum.Halo2);
            (string, string) H3sf = getGameSFData(HaloTabEnum.Halo3);
            (string, string) ODsf = getGameSFData(HaloTabEnum.Halo3ODST);
            (string, string) HRsf = getGameSFData(HaloTabEnum.HaloReach);
            (string, string) H4sf = getGameSFData(HaloTabEnum.Halo4);

            Log.Information("Initialising save folder shared memory with: \n" +
                    H1sf.Item2 + "\n" +
                    H2sf.Item2 + "\n" +
                    H3sf.Item2 + "\n" +
                    ODsf.Item2 + "\n" +
                    HRsf.Item2 + "\n" +
                    H4sf.Item2 + "\n"
                );

            ushort interprocInit;
            // is data good
            if (cp == null || cp.CheckpointName == null || sf == null || sf.SaveFolderName == null || sf.SaveFolderPath == null)
            {
                interprocInit = initialiseInterproc(true, 0, "", "", "", "", 0, // null cp data
                    H1sf.Item1, H1sf.Item2,
                    H2sf.Item1, H2sf.Item2,
                    H3sf.Item1, H3sf.Item2,
                    ODsf.Item1, ODsf.Item2,
                    HRsf.Item1, HRsf.Item2,
                    H4sf.Item1, H4sf.Item2
                    );
            }
            else
            {
                interprocInit = initialiseInterproc(false,
                    (int)CheckpointViewModel.SelectedGame, cp.CheckpointName, sf.SaveFolderPath + "\\" + cp.CheckpointName + ".bin", cp.LevelName ?? "", cp.GameVersion ?? "", cp.Difficulty ?? 0,
                    H1sf.Item1, H1sf.Item2,
                    H2sf.Item1, H2sf.Item2,
                    H3sf.Item1, H3sf.Item2,
                    ODsf.Item1, ODsf.Item2,
                    HRsf.Item1, HRsf.Item2,
                    H4sf.Item1, H4sf.Item2
                    );
            }

            if (interprocInit == 0)
            {
                Log.Error("Failed to init shared memory!");
                throw new System.Exception("Failed to initialise interproc service!");
            }
            else
            {
                Log.Information("Shared memory initialised");
            }
        }

        // first return is directory name, second return is directory path
        private (string, string) getGameSFData(HaloTabEnum game)
        {
            // Not sure which of these methods is the most reliable
            //Log.Verbose("AppContext.BaseDirectory: " + AppContext.BaseDirectory
            //    + "\nSystem.AppDomain.CurrentDomain.BaseDirectory: " + System.AppDomain.CurrentDomain.BaseDirectory
            //    + "\nSystem.Reflection.Assembly.GetExecutingAssembly().Location: " + System.Reflection.Assembly.GetExecutingAssembly().Location
            //    + "\nDirectory.GetCurrentDirectory(): " + Directory.GetCurrentDirectory()
            //    );

            Log.Verbose("Current directory: " + Directory.GetCurrentDirectory());
            string currentBaseDirectory = Directory.GetCurrentDirectory();

            string rootFolder = currentBaseDirectory + @"\Saves\" + Dictionaries.GameToRootFolderPath[game];
            string? dir = Settings.Default.LastSelectedFolder[(int)game]; // need to try catch this and return root folder on failure. 

            Log.Verbose("The last selected folder for game: " + game + " was " + Settings.Default.LastSelectedFolder[(int)game]);

            if (dir == null) // check if dir is real and exists
            {
                Log.Error("getGameSFData: dir was null at Settings.Default.LastSelectedFolder[game] with game == " + game + ", as int: " + (int)game);
                return (Dictionaries.GameToRootFolderPath[game], rootFolder);        // return Root folder if there's a problem.
            }
            else if (!Directory.Exists(dir))
            {
                Log.Error("getGameSFData: Directory didn't exist at " + dir);
                Log.Verbose("so returning " + Dictionaries.GameToRootFolderPath[game] + ", " + rootFolder);
                return (Dictionaries.GameToRootFolderPath[game], rootFolder);        // return Root folder if there's a problem.
            }
            Log.Verbose("getGameSFData: returning " + Path.GetFileName(dir) + ", " + dir + " for game " + game);
            return (Path.GetFileName(dir), dir);
        }




        private void CheckpointViewModel_PropertyChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            Task.Run(() =>
            {
                if (e.PropertyName == nameof(CheckpointViewModel.SelectedCheckpoint))
                {
                    Log.Verbose("sending new cp info");
                    Models.Checkpoint? cp = CheckpointViewModel.SelectedCheckpoint;
                    Models.SaveFolder sf = CheckpointViewModel.SelectedSaveFolder;
                    Log.Verbose(string.Format("Sending new checkpoint info to internal, null cp: {0}, null sf: {1}", cp == null ? "true" : "false", ", null sf:" + sf == null ? "true" : "false"));
                    if (cp == null || cp.CheckpointName == null || sf == null || sf.SaveFolderName == null || sf.SaveFolderPath == null)
                    {
                        updateSelectedCheckpoint(true);
                    }
                    else
                    {
                        updateSelectedCheckpoint(false, Dictionaries.HaloTabEnumToInternalIndex(CheckpointViewModel.SelectedGame), cp.CheckpointName, sf.SaveFolderPath + "\\" + cp.CheckpointName + ".bin", cp.LevelName ?? "", cp.GameVersion ?? "", cp.Difficulty ?? 0);
                    }
                }
                else if (e.PropertyName == nameof(CheckpointViewModel.SelectedSaveFolder))
                {

                    (string, string) H1sf = getGameSFData(HaloTabEnum.Halo1);
                    (string, string) H2sf = getGameSFData(HaloTabEnum.Halo2);
                    (string, string) H3sf = getGameSFData(HaloTabEnum.Halo3);
                    (string, string) ODsf = getGameSFData(HaloTabEnum.Halo3ODST);
                    (string, string) HRsf = getGameSFData(HaloTabEnum.HaloReach);
                    (string, string) H4sf = getGameSFData(HaloTabEnum.Halo4);

                    Log.Information("Updating save folder shared memory with: \n" +
                    H1sf.Item2 + "\n" +
                    H2sf.Item2 + "\n" +
                    H3sf.Item2 + "\n" +
                    ODsf.Item2 + "\n" +
                    HRsf.Item2 + "\n" +
                    H4sf.Item2 + "\n"
                );

                    Log.Verbose("sending new sf info");
                    Models.SaveFolder sf = CheckpointViewModel.SelectedSaveFolder;
                    Log.Verbose(string.Format("Sending new saveFolder info to internal: {0}", sf == null ? "null!" : sf.SaveFolderPath));
                    updateSelectedFolder(
                    H1sf.Item1, H1sf.Item2,
                    H2sf.Item1, H2sf.Item2,
                    H3sf.Item1, H3sf.Item2,
                    ODsf.Item1, ODsf.Item2,
                    HRsf.Item1, HRsf.Item2,
                    H4sf.Item1, H4sf.Item2);
                }
            });


        }


        // basically we're just converting from the callback to a c# style event

        public class StatusMachineStatusChangedEventArgs : EventArgs
        {
           public MCCHookStateEnum state { get; set; }
        }

        public class InterprocErrorEventArgs : EventArgs
        {
            public string message { get; set; }
        }


        public event EventHandler<StatusMachineStatusChangedEventArgs> StateMachineStatusChanged;
        public event EventHandler<InterprocErrorEventArgs> InterprocError;




        private void ERRORCALLBACK(string message)
        {
            Log.Verbose("Handling interproc error event with message: " + message);
            EventHandler<InterprocErrorEventArgs> handler = InterprocError;
            if (handler != null)
            {
                InterprocErrorEventArgs args = new InterprocErrorEventArgs();
                args.message = message;
                handler(this, args);
            }
        }

        private void LOGCALLBACK(string message)
        {
            Log.Verbose("HCMInterproc: " + message);
        }

        private void SMSTATUSCALLBACK(int status)
        {
            Log.Verbose("Handling interproc state machine status changed event with new status: " + status);
            EventHandler<StatusMachineStatusChangedEventArgs> handler = StateMachineStatusChanged;
            if (handler != null && Enum.IsDefined(typeof(MCCHookStateEnum), status))
            {
                StatusMachineStatusChangedEventArgs args = new StatusMachineStatusChangedEventArgs();
                args.state = (MCCHookStateEnum)status;
                handler(this, args);
            }
        }




    }
}
