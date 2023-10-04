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
        private static partial void updateSelectedFolder([MarshalAs(UnmanagedType.Bool)] bool nullData, int game = 0, string name = "", string path = "");

        [LibraryImport("HCMInterproc.DLL", StringMarshallingCustomType = typeof(Utf8StringMarshaller))]
        private static partial UInt16 initSharedMemory(
            [MarshalAs(UnmanagedType.Bool)] bool CPnullData, 
            [MarshalAs(UnmanagedType.Bool)] bool SFnullData, 
            int CPgame = 0, string CPname = "", string CPpath = "", string CPlevelcode = "", string CPgameVersion = "",
            int SFgame = 0, string SFname = "", string SFpath = ""
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

            UInt16 sharedMemoryInit;
            // is data good
            if (cp == null || cp.CheckpointName == null || sf == null || sf.SaveFolderName == null || sf.SaveFolderPath == null || sf == null || sf.SaveFolderName == null || sf.SaveFolderPath == null) 
            {
                sharedMemoryInit = initSharedMemory(true, true); // null data
            }
            else 
            {
                sharedMemoryInit = initSharedMemory(false, false, 
                    (int)CheckpointViewModel.SelectedGame, cp.CheckpointName, sf.SaveFolderPath + "\\" + cp.CheckpointName + ".bin", cp.LevelName ?? "", cp.GameVersion ?? "",
                    (int)CheckpointViewModel.SelectedGame, sf.SaveFolderName, sf.SaveFolderPath
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
                        updateSelectedCheckpoint(false, (int)CheckpointViewModel.SelectedGame, cp.CheckpointName, sf.SaveFolderPath + "\\" + cp.CheckpointName + ".bin", cp.LevelName ?? "", cp.GameVersion ?? "");
                }
                else if (e.PropertyName == nameof(CheckpointViewModel.SelectedSaveFolder))
                {
                    Log.Verbose("sending new sf info");
                    var sf = CheckpointViewModel.SelectedSaveFolder;
                    Log.Verbose(string.Format("Sending new saveFolder info to internal: {0}", sf == null ? "null!" : sf.SaveFolderPath));
                    if (sf == null || sf.SaveFolderName == null || sf.SaveFolderPath == null)
                        updateSelectedFolder(true);
                    else
                        updateSelectedFolder(false, (int)CheckpointViewModel.SelectedGame, sf.SaveFolderName, sf.SaveFolderPath);
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
