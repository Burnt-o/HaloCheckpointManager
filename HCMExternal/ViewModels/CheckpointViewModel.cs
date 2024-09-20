using GongSolutions.Wpf.DragDrop;
using HCMExternal.Helpers.DictionariesNS;
using HCMExternal.Models;
using HCMExternal.Services.CheckpointServiceNS;
using HCMExternal.ViewModels.Commands;
using HCMExternal.ViewModels.Interfaces;
using Serilog;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows;
using System.Windows.Data;
using System.Windows.Input;

namespace HCMExternal.ViewModels
{

    public partial class CheckpointViewModel : Presenter, IDropTarget
    {

        public ObservableCollection<Checkpoint> CheckpointCollection { get; private set; }
        public ObservableCollection<SaveFolder> SaveFolderHierarchy { get; private set; }

        public SaveFolder RootSaveFolder { get; private set; }

        private HaloTabEnum _selectedGame = HaloTabEnum.Halo1;
        public HaloTabEnum SelectedGame
        {
            get => _selectedGame;
            set
            {
                Log.Verbose("SelectedGame changed: " + SelectedGame.ToString());
                _selectedGame = value;
                OnPropertyChanged(nameof(SelectedGame));
            }
        }



        private Checkpoint? _selectedCheckpoint = null;
        public Checkpoint? SelectedCheckpoint
        {
            get => _selectedCheckpoint;
            set
            {
                _selectedCheckpoint = value;
                OnPropertyChanged(nameof(SelectedCheckpoint));

                if (value == null)
                {
                    Log.Debug("Selected checkpoint set to null");
                    // Log.Verbose("SelectedCheckpoint set to null! Stacktrace:\n" + Environment.StackTrace);
                }

            }
        }

        private SaveFolder _selectedSaveFolder;
        public SaveFolder SelectedSaveFolder
        {
            get => _selectedSaveFolder;
            set
            {
                if (_selectedSaveFolder != value)
                {
                    Log.Verbose(string.Format("Setting saveFolderPath to {0}", value.SaveFolderPath));
                }

                _selectedSaveFolder = value;
                OnPropertyChanged(nameof(SelectedSaveFolder));
            }
        }


        [Obsolete("Only for design data", true)]
        public CheckpointViewModel()
        {
            if (!IsInDesignModeStatic)
            {
                throw new Exception("Use only for design mode");
            }

        }

        //public event EventHandler<int> RequestTabChange;

        //public void InvokeRequestTabChange(int requestedTab)
        //{ 
        //RequestTabChange?.Invoke(this, requestedTab);
        //}


        private readonly FileSystemWatcher? SaveFileWatcher = new FileSystemWatcher(@"Saves\", "*.bin");
        private readonly FileSystemWatcher? SaveDirWatcher = new FileSystemWatcher(@"Saves\");


        private readonly SynchronizationContext _syncContext;
        private CheckpointService CheckpointServices { get; init; }
        public CheckpointViewModel(CheckpointService cs, MCCHookStateViewModel hookStateVM)
        {
            _syncContext = SynchronizationContext.Current;
            Log.Verbose("CheckpointViewModel constructing");
            CheckpointServices = cs;
            CheckpointCollection = new();
            SaveFolderHierarchy = new();
            RootSaveFolder = null;

            // Deserialise selected tab
            if (!Enum.IsDefined(typeof(HaloTabEnum), HCMExternal.Properties.Settings.Default.LastSelectedGameTab))
            {
                Log.Error("Failed to deserialise valid LastSelectedGameTab value: " + HCMExternal.Properties.Settings.Default.LastSelectedGameTab);
                HCMExternal.Properties.Settings.Default.LastSelectedGameTab = (int)HaloTabEnum.Halo1;
            }
            SelectedGame = (HaloTabEnum)HCMExternal.Properties.Settings.Default.LastSelectedGameTab;
            ListCollectionView view = (ListCollectionView)CollectionViewSource
                    .GetDefaultView(CheckpointCollection);

            view.CustomSort = new SortCheckpointsByLastWriteTime();


            // Subscribe to changes in hookstate so we can tell CheckpointModel to refreshList w/ new version guess on checkpoint files
            hookStateVM.State.PropertyChanged += (o, i) =>
            {
                if (i.PropertyName == nameof(MCCHookState.MCCVersion)) // we only care about changes in currently attached mcc version
                {
                    HCMExternal.App.Current.Dispatcher.Invoke(delegate // Need to make sure it's run on the UI thread
                    {
                        RefreshCheckpointList();
                    });
                }

            };


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


            RefreshSaveFolderTree();
            RefreshCheckpointList();

            PropertyChanged += CheckpointViewModel_PropertyChanged;

            // deserialise last selected checkpoint
            Log.Verbose("Deserialising last selected checkpoint: Properties.Settings.Default.LastSelectedCheckpoint != null = " + (Properties.Settings.Default.LastSelectedCheckpoint != null) + ", this.SelectedSaveFolder != null = " + (SelectedSaveFolder != null));
            if (Properties.Settings.Default.LastSelectedCheckpoint != null && SelectedSaveFolder != null)
            {
                Log.Verbose("Properties.Settings.Default.LastSelectedCheckpoint: " + Properties.Settings.Default.LastSelectedCheckpoint);
                string lastSelectedCheckpointPath = (SelectedSaveFolder.SaveFolderPath + Properties.Settings.Default.LastSelectedCheckpoint);
                Log.Verbose("lastSelectedCheckpointPath: " + lastSelectedCheckpointPath);
                Log.Verbose("exists? " + File.Exists(lastSelectedCheckpointPath));
                if (File.Exists(lastSelectedCheckpointPath))
                {
                    // iterate thru checkpoints in current savefolder and select the one that matches the name
                    foreach (Checkpoint cp in CheckpointCollection)
                    {
                        if (cp.CheckpointName == Properties.Settings.Default.LastSelectedCheckpoint)
                        {
                            Log.Verbose("Found lastSelectedCheckpoint in current collection, selecting.");
                            SelectedCheckpoint = cp;
                            break;
                        }
                    }
                }
            }



        }

        private void OnFileChanged(object sender, FileSystemEventArgs e)
        {
#if HCM_DEBUG
            Log.Debug($"OnFileChanged: Stacktrace: {Environment.StackTrace}");
            Log.Debug($"args: {e.ToString()}, affected file path: {e.FullPath}, sender: {sender.ToString()}");
#endif

            HCMExternal.App.Current.Dispatcher.Invoke(delegate // Need to make sure it's run on the UI thread
            {
                RefreshCheckpointList();
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
                RefreshSaveFolderTree();
                RefreshCheckpointList();
            });
        }

        private void CheckpointViewModel_PropertyChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            switch (e.PropertyName)
            {
                case nameof(SelectedGame):
                    HCMExternal.Properties.Settings.Default.LastSelectedGameTab = (int)SelectedGame;
                    RefreshSaveFolderTree();
                    RefreshCheckpointList();

                    break;

                case nameof(SelectedCheckpoint):
                    // serialise 
                    if (SelectedCheckpoint != null)
                    {
                        Properties.Settings.Default.LastSelectedCheckpoint = SelectedCheckpoint.CheckpointName;
                        Log.Verbose("Properties.Settings.Default.LastSelectedCheckpoint set to " + SelectedCheckpoint.CheckpointName);
                    }
                    else
                    {
                        Properties.Settings.Default.LastSelectedCheckpoint = null;
                        Log.Verbose("Properties.Settings.Default.LastSelectedCheckpoint set to null");
                    }

                    break;


                default:
                    break;
            }
        }


        public void RefreshCheckpointList()
        {

            // store old selected checkpoint
            string? oldCP = SelectedCheckpoint?.CheckpointName;

            CheckpointCollection.Clear();
            Log.Debug("Populating checkpoint list with data from folder: " + SelectedSaveFolder.SaveFolderPath);



            ObservableCollection<Checkpoint> newCollection = CheckpointServices.PopulateCheckpointList(SelectedSaveFolder, SelectedGame);
            foreach (Checkpoint c in newCollection)
            {
                CheckpointCollection.Add(c);
            }
            Log.Debug("refreshed CheckpointCollection, count: " + CheckpointCollection.Count);

            // try to reselect checkpoint
            if (oldCP != null)
            {
                foreach (Checkpoint cp in CheckpointCollection)
                {
                    if (cp.CheckpointName == oldCP)
                    {
                        Log.Verbose("Succesfully reselected old checkpoint, name: " + oldCP);
                        SelectedCheckpoint = cp;
                        break;
                    }
                }
            }


        }

        public void RefreshSaveFolderTree()
        {


            SaveFolderHierarchy.Clear();
            ObservableCollection<SaveFolder> newHierarchy = CheckpointServices.PopulateSaveFolderTree(out SaveFolder rootFolder, SelectedGame);
            RootSaveFolder = rootFolder;
            foreach (SaveFolder s in newHierarchy)
            {
                SaveFolderHierarchy.Add(s);
            }


            // Now let's try to set the selected folder to whatever folder was last selected on this tab
            if (RootSaveFolder != null)
            {
                bool ableToSetLastFolder = false; // a flag that can be set in foreach loop and used for root folder fallback if not set

                string? lastSelectedFolder = null;
                if (Properties.Settings.Default.LastSelectedFolder != null && Properties.Settings.Default.LastSelectedFolder.Count > (int)SelectedGame)
                {
                    lastSelectedFolder = Properties.Settings.Default.LastSelectedFolder?[(int)SelectedGame];
                }

                if (lastSelectedFolder != null)
                {
                    IEnumerable<SaveFolder> flattenedTree = FlattenTree(RootSaveFolder);
                    foreach (SaveFolder sf in flattenedTree)
                    {
                        // If it matches!
                        if (sf.SaveFolderPath == lastSelectedFolder && Directory.Exists(lastSelectedFolder))
                        {
                            Trace.WriteLine("Resetting last selected folder to " + sf.SaveFolderPath);
                            sf.IsSelected = true;
                            ableToSetLastFolder = true;
                            SelectedSaveFolder = sf;
                            break;
                        }
                    }
                }
                if (!ableToSetLastFolder)
                {
                    // If we weren't able to reset it then default to root folder
                    RootSaveFolder.IsSelected = true;
                    SelectedSaveFolder = RootSaveFolder;
                }

            }




            IEnumerable<SaveFolder> FlattenTree(SaveFolder node)
            {
                if (node == null)
                {
                    yield break;
                }
                yield return node;
                foreach (SaveFolder n in node.Children)
                {
                    foreach (SaveFolder innerN in FlattenTree(n))
                    {
                        yield return innerN;
                    }
                }
            }
        }



        public class SortCheckpointsByLastWriteTime : IComparer
        {
            public int Compare(object x, object y)
            {
                Checkpoint cx = (Checkpoint)x;
                Checkpoint cy = (Checkpoint)y;

                if (cx.ModifiedOn == null || cy.ModifiedOn == null)
                { return 0; }

                int? diff = (int?)(cx.ModifiedOn - cy.ModifiedOn)?.TotalSeconds;
                return diff == null ? 0 : (int)diff;
            }
        }

        public void FolderChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            Trace.WriteLine("FolderChanged. Sender: " + sender + ", currentgame?: " + SelectedGame);
            SaveFolder? saveFolder = (SaveFolder?)e.NewValue;

            if (saveFolder == null)
            {
                return;
            }

            Trace.WriteLine("Selected Folder Path: " + saveFolder.SaveFolderPath);

            if (Directory.Exists(saveFolder.SaveFolderPath))
            {
                Trace.WriteLine("valid new saveFolder, so changing internal selection and inserting into lastSelectedFolder list.");
                Log.Verbose("New lastSelectedFolder for game " + SelectedGame + " is " + saveFolder.SaveFolderPath);

                Properties.Settings.Default.LastSelectedFolder[(int)SelectedGame] = saveFolder.SaveFolderPath;
                // Properties.Settings.Default.LastSelectedFolder.Insert(, saveFolder.SaveFolderPath);

                Log.Verbose("This should be the same value as:" + Properties.Settings.Default.LastSelectedFolder[(int)SelectedGame]);

                SelectedSaveFolder = saveFolder;
                RefreshCheckpointList();

                // set selected checkpoint to top of the list
                if (CheckpointCollection.Count > 0)
                {
                    SelectedCheckpoint = CheckpointCollection.ElementAt(0);
                }

            }
            else
            {
                MessageBox.Show("Error with new selected save folder at path: " + saveFolder.SaveFolderPath);
                //this.RefreshSaveFolderTree();
            }



        }


        public void RequestTabChange(HaloTabEnum game)
        {
            // send is syncronhys so this will block
            _syncContext.Send(o => { SelectedGame = game; }, null);
            //Application.Current.Dispatcher.Invoke(new Action(() => { SelectedGame = game; }));
        }


        private ICommand _dump;
        public ICommand Dump
        {
            get => _dump ?? (_dump = new DumpCommand());
            set => _dump = value;
        }
        private ICommand _inject;
        public ICommand Inject
        {
            get => _inject ?? (_inject = new InjectCommand());
            set => _inject = value;
        }

        private ICommand _deleteCheckpoint;
        public ICommand DeleteCheckpoint
        {
            get => _deleteCheckpoint ?? (_deleteCheckpoint = new DeleteCheckpointCommand(this, CheckpointServices));
            set => _deleteCheckpoint = value;
        }

        private ICommand _renameCheckpoint;
        public ICommand RenameCheckpoint
        {
            get => _renameCheckpoint ?? (_renameCheckpoint = new RenameCheckpointCommand(this, CheckpointServices));
            set => _renameCheckpoint = value;
        }

        private ICommand _reVersionCheckpoint;
        public ICommand ReVersionCheckpoint
        {
            get => _reVersionCheckpoint ?? (_reVersionCheckpoint = new ReVersionCheckpointCommand(this, CheckpointServices));
            set => _reVersionCheckpoint = value;
        }

        private ICommand _sortCheckpoint;
        public ICommand SortCheckpoint
        {
            get => _sortCheckpoint ?? (_sortCheckpoint = new SortCheckpointCommand(this, CheckpointServices));
            set => _sortCheckpoint = value;
        }

        private ICommand _openInExplorer;
        public ICommand OpenInExplorer
        {
            get => _openInExplorer ?? (_openInExplorer = new OpenInExplorerCommand(this, CheckpointServices));
            set => _openInExplorer = value;
        }

        private ICommand _renameFolder;
        public ICommand RenameFolder
        {
            get => _renameFolder ?? (_renameFolder = new RenameFolderCommand(this, CheckpointServices));
            set => _renameFolder = value;
        }

        private ICommand _deleteFolder;
        public ICommand DeleteFolder
        {
            get => _deleteFolder ?? (_deleteFolder = new DeleteFolderCommand(this, CheckpointServices));
            set => _deleteFolder = value;
        }

        private ICommand _newFolder;
        public ICommand NewFolder
        {
            get => _newFolder ?? (_newFolder = new NewFolderCommand(this, CheckpointServices));
            set => _newFolder = value;
        }




    }




}
