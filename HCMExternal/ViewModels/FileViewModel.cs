using GongSolutions.Wpf.DragDrop;
using HCMExternal.Models;
using HCMExternal.Services.CheckpointIO;
using HCMExternal.Services.External;
using HCMExternal.Services.Interproc;
using HCMExternal.ViewModels.Commands;
using HCMExternal.ViewModels.Interfaces;
using Serilog;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows;
using System.Windows.Data;


namespace HCMExternal.ViewModels
{

    public partial class FileViewModel : Presenter, IDropTarget
    {


        public ObservableCollection<Checkpoint> CheckpointCollection { get; private set; }
        public ObservableCollection<SaveFolder> SaveFolderHierarchy { get; private set; }
        public SaveFolder RootSaveFolder { get; private set; }


        private Checkpoint? _selectedCheckpoint = null;
        public Checkpoint? SelectedCheckpoint
        {
            get => _selectedCheckpoint;
            set
            {
                _selectedCheckpoint = value;
                OnPropertyChanged(nameof(SelectedCheckpoint));
                _interprocService.UpdateSharedMemCheckpoint(SelectedGame, SelectedCheckpoint);

                if (value == null)
                {
                    Log.Debug("Selected checkpoint set to null (ie no checkpoint selected)");
                }

                // serialise
                Properties.Settings.Default.LastSelectedCheckpoint = _selectedCheckpoint?.CheckpointName;

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
                _interprocService.UpdateSharedMemSaveFolder(SelectedGame, SelectedSaveFolder);

                // serialise
                if (Properties.Settings.Default.LastSelectedFolder == null || Properties.Settings.Default.LastSelectedFolder.Count < 7)
                {
                    Properties.Settings.Default.LastSelectedFolder = new StringCollection() { "", "", "", "", "", "", "" };
                }
                Properties.Settings.Default.LastSelectedFolder[(int)SelectedGame] = _selectedSaveFolder.SaveFolderPath;


                UpdateCheckpointCollection();
                // set selected checkpoint to top of the list
                if (CheckpointCollection.Count > 0)
                {
                    SelectedCheckpoint = CheckpointCollection.ElementAt(0);
                }
            }
        }

        private HaloGame _selectedGame = HaloGame.Halo1;
        public HaloGame SelectedGame
        {
            get => _selectedGame;
            set
            {
                Log.Verbose("SelectedGame changed from " + SelectedGame.ToString() + " to " + value.ToString());
                _selectedGame = value;
                OnPropertyChanged(nameof(SelectedGame)); // update ui

                // update save folder and checkpoints
                UpdateSaveFolderCollection();
                UpdateCheckpointCollection();

                // serialise
                HCMExternal.Properties.Settings.Default.LastSelectedGameTab = (int)SelectedGame;
            }
        }

        [Obsolete("Only for design data", true)]
        public FileViewModel()
        {
            if (!IsInDesignModeStatic)
            {
                throw new Exception("Use only for design mode");
            }

        }


        public MainVMCommands MainVMCommands { get; init; }

        private readonly FileSystemWatcher? SaveFileWatcher = new FileSystemWatcher(@"Saves\", "*.bin");
        private readonly FileSystemWatcher? SaveDirWatcher = new FileSystemWatcher(@"Saves\");



        private readonly SynchronizationContext _syncContext;
        private ICheckpointIOService _checkpointIOService { get; init; }
        private IExternalService _externalService { get; init; }
        private IInterprocService _interprocService { get; init; }
        public FileViewModel(ICheckpointIOService cpio, IExternalService ex, IInterprocService ip, MainVMCommands mvmCommands)
        {
            MainVMCommands = mvmCommands;
            _syncContext = SynchronizationContext.Current ?? throw new Exception("Null Synchronization Context ?!?!");
            Log.Verbose("FileViewModel constructing");
            _checkpointIOService = cpio;
            _externalService = ex;
            _interprocService = ip;
            CheckpointCollection = new();
            SaveFolderHierarchy = new();
            RootSaveFolder = null;

            // checkpoint collection is always sorted by LastWriteTime. 
            ListCollectionView view = (ListCollectionView)CollectionViewSource
                    .GetDefaultView(CheckpointCollection);

            view.CustomSort = new SortCheckpointsByLastWriteTime();
        }
          



        public void UpdateCheckpointCollection()
        {

            // store old selected checkpoint
            string? oldCP = SelectedCheckpoint?.CheckpointName;

            CheckpointCollection.Clear();
            Log.Debug("Populating checkpoint list with data from folder: " + SelectedSaveFolder.SaveFolderPath);

            ObservableCollection<Checkpoint> newCollection = _checkpointIOService.PopulateCheckpointList(SelectedSaveFolder, SelectedGame);
            foreach (Checkpoint c in newCollection)
            {
                CheckpointCollection.Add(c);
            }
            Log.Debug("refreshed CheckpointCollection, count: " + CheckpointCollection.Count);

            // try to reselect old checkpoint
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

        public void UpdateSaveFolderCollection()
        {

            SaveFolderHierarchy.Clear();
            ObservableCollection<SaveFolder> newHierarchy = _checkpointIOService.PopulateSaveFolderTree(out SaveFolder rootFolder, SelectedGame);
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
            else
            {
                Log.Error("Oh dear god, the root folder is null");
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




        public void TreeFolderChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            Trace.WriteLine("TreeFolderChanged. Sender: " + sender + ", currentgame?: " + SelectedGame);
            SaveFolder? saveFolder = (SaveFolder?)e.NewValue;

            if (saveFolder == null)
            {
                return;
            }

            Trace.WriteLine("Selected Folder Path: " + saveFolder.SaveFolderPath);

            if (Directory.Exists(saveFolder.SaveFolderPath))
                SelectedSaveFolder = saveFolder;
            else
                MessageBox.Show("Error with new selected save folder at path: " + saveFolder.SaveFolderPath);



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



    }




}
