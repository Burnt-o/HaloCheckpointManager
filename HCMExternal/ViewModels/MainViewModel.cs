
using GongSolutions.Wpf.DragDrop;
using HCMExternal.Models;
using HCMExternal.Services.CheckpointIO.Impl;
using HCMExternal.Services.External;
using HCMExternal.Services.Interproc;
using HCMExternal.ViewModels.Interfaces;
using Serilog;
using System;
using System.Collections.Specialized;
using System.IO;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using HCMExternal.ViewModels.Commands;
using HCMExternal.Services.Hotkeys;
using HCMExternal.Services.CheckpointIO;
using HCMExternal.Services.Hotkeys.Impl;
using System.Windows;
namespace HCMExternal.ViewModels
{
    public partial class MainViewModel : Presenter
    {




        // created
        public FileViewModel FileViewModel { get; init; }
        public MCCHookStateViewModel MCCHookStateViewModel { get; init; }
        public TrainerViewModel TrainerViewModel { get; init; }

        // injected
        private ICheckpointIOService _checkpointIOService;
        private IExternalService _externalService;
        private IInterprocService _interprocService;


        private readonly FileSystemWatcher? SaveFileWatcher = new FileSystemWatcher(@"Saves\", "*.bin");
        private readonly FileSystemWatcher? SaveDirWatcher = new FileSystemWatcher(@"Saves\");

        private bool _shouldShowTrainerView;
        public bool ShouldShowTrainerView
        {
            get => _shouldShowTrainerView;
            set
            {
                 
                if (_shouldShowTrainerView != value)
                {
                    _shouldShowTrainerView = value;
                    const int trainerViewWidth = 400;
                    Log.Verbose("Expand/contracting main window");
                    if (value)
                    {

                        Application.Current.MainWindow.Width += trainerViewWidth;
                        Application.Current.MainWindow.MinWidth += trainerViewWidth;
                    }
                    else
                    {
                        Application.Current.MainWindow.MinWidth -= trainerViewWidth;
                        Application.Current.MainWindow.Width -= trainerViewWidth;
                    }
                       

                }

                OnPropertyChanged(nameof(ShouldShowTrainerView));
            }
        }

        public MainViewModel(ICheckpointIOService cpio, IExternalService exs, IInterprocService ips, IHotkeyManager hotkey)
        {
            _checkpointIOService = cpio;
            _externalService = exs;
            _interprocService = ips;

           

            Log.Verbose("MainViewModel constructing");

            // create command collection, binding commands to actions
            MainVMCommands mvmCommands = new()
            {
                DeleteCheckpoint = new((arg) => { onDeleteCheckpoint(); }),
                RenameCheckpoint = new((arg) => { onRenameCheckpoint(); }),
                ReVersionCheckpoint = new((arg) => { onReVersionCheckpoint(); }),
                SortCheckpoint = new((arg) => { onSortCheckpoint(); }),
                OpenInExplorer = new((arg) => { onOpenInExplorer(); }),
                RenameFolder = new((arg) => { onRenameFolder(); }),
                DeleteFolder = new((arg) => { onDeleteFolder(); }),
                NewFolder = new((arg) => { onNewFolder(); }),
                ForceCheckpoint = new((arg) => { onForceCheckpoint(); }),
                ForceRevert = new((arg) => { onForceRevert(); }),
                ForceDoubleRevert = new((arg) => { onForceDoubleRevert(); }),
                DumpCheckpoint = new((arg) => { onDumpCheckpoint(); }),
                InjectCheckpoint = new((arg) => { onInjectCheckpoint(); }),
                DisableCheckpoint = new((arg) => {
                    if (arg == null || arg.GetType() != typeof(bool))
                        Log.Error("Bad arg to DisableCheckpoint Command");
                    else
                        onDisableCheckpoints((bool)arg); }),
            };


            // need to load before it might get changed by FileViewModel. used for deserialisation 
            string? lastSelectedCheckpoint = Properties.Settings.Default.LastSelectedCheckpoint;

            FileViewModel = new(_checkpointIOService, exs, ips, mvmCommands); ;
            MCCHookStateViewModel = new(new ErrorDialogViewModel(), ips);
            TrainerViewModel = new(mvmCommands, hotkey);


            // (dis)able hotkeys depending whether project cartrographer tab selected
            FileViewModel.PropertyChanged += (o, e) =>
            {
                if (e.PropertyName == "SelectedGame")
                {
                    Log.Verbose("SelectedGame changed t mvm");
                    ShouldShowTrainerView = FileViewModel.SelectedGame == HaloGame.ProjectCartographer;
                    hotkey.HotkeysEnabled = ShouldShowTrainerView;
                }
                        
            };

            // Deserialise selected tab
            if (!Enum.IsDefined(typeof(HaloGame), HCMExternal.Properties.Settings.Default.LastSelectedGameTab))
            {
                Log.Error("Failed to deserialise valid LastSelectedGameTab value: " + HCMExternal.Properties.Settings.Default.LastSelectedGameTab);
                HCMExternal.Properties.Settings.Default.LastSelectedGameTab = (int)HaloGame.Halo1;
                Properties.Settings.Default.Save();
            }
            Log.Verbose("Deserialised LastSelectedGameTab: " + (HaloGame)HCMExternal.Properties.Settings.Default.LastSelectedGameTab);
            FileViewModel.SelectedGame = (HaloGame)HCMExternal.Properties.Settings.Default.LastSelectedGameTab;



            // deserialise last selected checkpoint
            Log.Verbose("Deserialising last selected checkpoint: lastSelectedCheckpoint != null = " + (lastSelectedCheckpoint != null) + ", this.SelectedSaveFolder != null = " + (FileViewModel.SelectedSaveFolder != null));
            if (lastSelectedCheckpoint != null && FileViewModel.SelectedSaveFolder != null)
            {
                Log.Verbose("lastSelectedCheckpoint: " + lastSelectedCheckpoint);
                string lastSelectedCheckpointPath = (FileViewModel.SelectedSaveFolder.SaveFolderPath + "\\" + lastSelectedCheckpoint + ".bin");
                Log.Verbose("lastSelectedCheckpointPath: " + lastSelectedCheckpointPath);
                Log.Verbose("exists? " + File.Exists(lastSelectedCheckpointPath));
                if (File.Exists(lastSelectedCheckpointPath))
                {
                    // iterate thru checkpoints in current savefolder and select the one that matches the name
                    foreach (Checkpoint cp in FileViewModel.CheckpointCollection)
                    {
                        if (cp.CheckpointName == lastSelectedCheckpoint)
                        {
                            Log.Verbose("Found lastSelectedCheckpoint in current collection, selecting.");
                            FileViewModel.SelectedCheckpoint = cp;
                            break;
                        }
                    }
                }
            }


            // Setup FileSystemWatcher to monitor save folders for changes
            // set filters
            SaveFileWatcher.NotifyFilter = NotifyFilters.CreationTime
                                 | NotifyFilters.FileName
                                 | NotifyFilters.LastWrite
                                 | NotifyFilters.Size;

            SaveDirWatcher.NotifyFilter = NotifyFilters.DirectoryName;

            // sub to events
            //SaveFileWatcher.Changed += OnFileChanged;
            SaveFileWatcher.Created += OnFileChanged;
            SaveFileWatcher.Deleted += OnFileChanged;
            SaveFileWatcher.Renamed += OnFileChanged;

            //SaveDirWatcher.Changed += OnDirChanged;
            SaveDirWatcher.Created += OnDirChanged;
            SaveDirWatcher.Deleted += OnDirChanged;
            SaveDirWatcher.Renamed += OnDirChanged;

            SaveFileWatcher.IncludeSubdirectories = true;
            SaveDirWatcher.IncludeSubdirectories = true;

            SaveFileWatcher.EnableRaisingEvents = true;
            SaveDirWatcher.EnableRaisingEvents = true;

        }

 


        private void OnFileChanged(object sender, FileSystemEventArgs e)
        {
#if HCM_DEBUG
            Log.Debug($"OnFileChanged: Stacktrace: {Environment.StackTrace}");
            Log.Debug($"args: {e.ToString()}, affected file path: {e.FullPath}, sender: {sender.ToString()}");
#endif

            HCMExternal.App.Current.Dispatcher.Invoke(delegate // Need to make sure it's run on the UI thread
            {
                FileViewModel.UpdateCheckpointCollection();
            });
        }

        private void OnDirChanged(object sender, FileSystemEventArgs e)
        {

#if HCM_DEBUG
            Log.Debug($"OnDirChanged: Stacktrace: {Environment.StackTrace}");
            Log.Debug($"args: {e.ToString()}, affected file path: {e.FullPath}, sender: {sender.ToString()}");
#endif
            HCMExternal.App.Current.Dispatcher.Invoke(delegate // Need to make sure it's run on the UI thread
            {
                FileViewModel.UpdateSaveFolderCollection();
                FileViewModel.UpdateCheckpointCollection();
            });
        }


       

    }
}
