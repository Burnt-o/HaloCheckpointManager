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
using System.Runtime.CompilerServices;
using System.Data.Common;
using System.Runtime.InteropServices.Marshalling;
using HCMExternal.Properties;
using System.IO;
using System.Diagnostics.Eventing.Reader;

namespace HCMExternal.Services.InterprocServiceNS
{
    public partial class InterprocService
    {

        private CheckpointViewModel CheckpointViewModel;


        private delegate void PFN_ERRORCALLBACK(string str);
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        private static extern void ErrorEventSubscribe(PFN_ERRORCALLBACK callback);
        private PFN_ERRORCALLBACK DEL_ERRORCALLBACK;

        private delegate void PFN_LOGCALLBACK([MarshalAs(UnmanagedType.LPWStr)] string str);
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        private static extern void LogEventSubscribe(PFN_LOGCALLBACK callback);
        private PFN_LOGCALLBACK DEL_LOGCALLBACK;

        [LibraryImport("HCMInterproc.DLL", StringMarshallingCustomType = typeof(Utf8StringMarshaller))]
        private static partial void updateSelectedCheckpoint([MarshalAs(UnmanagedType.Bool)]bool nullData, int game = 0, string name = "", string path = "", string levelcode = "", string gameVersion = "", int difficulty = 0);

        [LibraryImport("HCMInterproc.DLL", StringMarshallingCustomType = typeof(Utf8StringMarshaller))]
        private static partial void updateSelectedFolder(
        string SFnameH1, string SFpathH1,
        string SFnameH2, string SFpathH2,
        string SFnameH3, string SFpathH3,
        string SFnameOD, string SFpathOD,
        string SFnameHR, string SFpathHR,
        string SFnameH4, string SFpathH4);

        [LibraryImport("HCMInterproc.DLL", StringMarshallingCustomType = typeof(Utf8StringMarshaller))]
        private static partial UInt16 initSharedMemory(
            [MarshalAs(UnmanagedType.Bool)] bool CPnullData, 
            int CPgame, string CPname, string CPpath, string CPlevelcode, string CPgameVersion, int CPdifficulty,
            string SFnameH1, string SFpathH1,
            string SFnameH2, string SFpathH2,
            string SFnameH3, string SFpathH3,
            string SFnameOD, string SFpathOD,
            string SFnameHR, string SFpathHR,
            string SFnameH4, string SFpathH4
            );



        [DllImport("HCMInterproc.dll")] private static extern bool SetupInternal();

        public InterprocService(CheckpointViewModel checkpointViewModel)
        {
            CheckpointViewModel = checkpointViewModel;

            CheckpointViewModel.PropertyChanged += CheckpointViewModel_PropertyChanged;



            // NOTE: do NOT put temporaries into the subscribes. They will eventually get garbage collected, then Interproc will eventually call them leading to KABOOM. I learned the hard way.

            // Subscribe to interproc events
            DEL_ERRORCALLBACK = new PFN_ERRORCALLBACK(ERRORCALLBACK);
            ErrorEventSubscribe(DEL_ERRORCALLBACK);

            DEL_LOGCALLBACK = new PFN_LOGCALLBACK(LOGCALLBACK);
            LogEventSubscribe(DEL_LOGCALLBACK);

            //DumpCommand.DumpEvent += () => 
            //{
            //    Log.Information("Sending dump command to internal");
            //    sendDumpCommand();
            //};
            //InjectCommand.InjectEvent += () =>
            //{
            //    Log.Information("Sending inject command to internal");
            //    sendInjectCommand();
            //};

            // Since InterprocService is created after CheckpointViewModel, the deserialised first selected checkpoint & savefolder events will be missed. So we need to manually fire them here.
            CheckpointViewModel.SelectedCheckpoint = CheckpointViewModel.SelectedCheckpoint;
            CheckpointViewModel.SelectedSaveFolder = CheckpointViewModel.SelectedSaveFolder;

            // get info on init cp and sf
            var cp = CheckpointViewModel.SelectedCheckpoint;
            var sf = CheckpointViewModel.SelectedSaveFolder;

            var H1sf = getGameSFData(HaloTabEnum.Halo1);
            var H2sf = getGameSFData(HaloTabEnum.Halo2);
            var H3sf = getGameSFData(HaloTabEnum.Halo3);
            var ODsf = getGameSFData(HaloTabEnum.Halo3ODST);
            var HRsf = getGameSFData(HaloTabEnum.HaloReach);
            var H4sf = getGameSFData(HaloTabEnum.Halo4);

            Log.Information("Initialising save folder shared memory with: \n" +
                    H1sf.Item2 + "\n" +
                    H2sf.Item2 + "\n" +
                    H3sf.Item2 + "\n" +
                    ODsf.Item2 + "\n" +
                    HRsf.Item2 + "\n" +
                    H4sf.Item2 + "\n"
                );

            UInt16 sharedMemoryInit;
            // is data good
            if (cp == null || cp.CheckpointName == null || sf == null || sf.SaveFolderName == null || sf.SaveFolderPath == null) 
            {
                sharedMemoryInit = initSharedMemory(true, 0, "", "", "", "", 0, // null cp data
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
                sharedMemoryInit = initSharedMemory(false, 
                    (int)CheckpointViewModel.SelectedGame, cp.CheckpointName, sf.SaveFolderPath + "\\" + cp.CheckpointName + ".bin", cp.LevelName ?? "", cp.GameVersion ?? "", cp.Difficulty ?? 0,
                    H1sf.Item1, H1sf.Item2,
                    H2sf.Item1, H2sf.Item2,
                    H3sf.Item1, H3sf.Item2,
                    ODsf.Item1, ODsf.Item2,
                    HRsf.Item1, HRsf.Item2,
                    H4sf.Item1, H4sf.Item2
                    );
            }

            if (sharedMemoryInit == 0)
            {
                Log.Error("Failed to init shared memory!");
            }
            else 
            {
                Log.Information("Shared memory initialised");
            }


        }

        // first return is directory name, second return is directory path
        private (string, string) getGameSFData(HaloTabEnum game)
        {
            var rootFolder = AppContext.BaseDirectory + @"Saves\" + Dictionaries.GameToRootFolderPath[game];
            var dir = Settings.Default.LastSelectedFolder[(int)game]; // need to try catch this and return root folder on failure. 
            
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
            Log.Verbose("getGameSFData: returning " + Path.GetDirectoryName(dir) + ", " + dir + " for game " + game);
            return (Path.GetFileName(dir), dir);
        }

      


        private void CheckpointViewModel_PropertyChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            Task.Run(() => {
                if (e.PropertyName == nameof(CheckpointViewModel.SelectedCheckpoint))
                {
                    Log.Verbose("sending new cp info");
                    var cp = CheckpointViewModel.SelectedCheckpoint;
                    var sf = CheckpointViewModel.SelectedSaveFolder;
                    Log.Verbose(string.Format("Sending new checkpoint info to internal, null cp: {0}, null sf: {1}", cp == null ? "true" : "false", ", null sf:" + sf == null ? "true" : "false"));
                    if (cp == null || cp.CheckpointName == null || sf == null || sf.SaveFolderName == null || sf.SaveFolderPath == null)
                        updateSelectedCheckpoint(true);
                    else
                        updateSelectedCheckpoint(false, (int)CheckpointViewModel.SelectedGame, cp.CheckpointName, sf.SaveFolderPath + "\\" + cp.CheckpointName + ".bin", cp.LevelName ?? "", cp.GameVersion ?? "", cp.Difficulty ?? 0);
                }
                else if (e.PropertyName == nameof(CheckpointViewModel.SelectedSaveFolder))
                {

                    var H1sf = getGameSFData(HaloTabEnum.Halo1);
                    var H2sf = getGameSFData(HaloTabEnum.Halo2);
                    var H3sf = getGameSFData(HaloTabEnum.Halo3);
                    var ODsf = getGameSFData(HaloTabEnum.Halo3ODST);
                    var HRsf = getGameSFData(HaloTabEnum.HaloReach);
                    var H4sf = getGameSFData(HaloTabEnum.Halo4);

                    Log.Verbose("sending new sf info");
                    var sf = CheckpointViewModel.SelectedSaveFolder;
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


        public bool Setup()
        {
            return SetupInternal();
        }



        private void ERRORCALLBACK(string message)
        {
            Log.Error("A HCMInterproc error occured: " + message);
            MessageBox.Show("An internal error occured: \n\n" + message + "\nLet Burnt know on discord. If the error keeps occuring, try disabling advanced cheats in settings.");
        }

        private void LOGCALLBACK(string message)
        {
            Log.Verbose("HCMInterproc: " + message);
        }

    



    }
}
