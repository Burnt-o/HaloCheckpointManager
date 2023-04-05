using HCM3.ViewModels.MVVM;
using HCM3.Models;
using System.Collections.ObjectModel;
using System.Windows.Input;

using System.ComponentModel;
using System.Diagnostics;
using System.Windows.Controls;
using System.Windows;
using System;
using Microsoft.Xaml.Behaviors;
using System.Collections;
using System.Collections.Specialized;
using System.Windows.Data;
using GongSolutions.Wpf.DragDrop;
using System.Windows.Controls;
using System.Windows.Shapes;
using System.Collections.Generic;
using System.Linq;
using HCM3.ViewModels.Commands;
using System.IO;
using System.ComponentModel;
using HCM3.Services;
using HCM3.Services.Cheats;
using HCM3.Services.HaloMemory;

namespace HCM3.ViewModels
{

    public partial class CheckpointViewModel : Presenter, IDropTarget
    {

        public ObservableCollection<Checkpoint> CheckpointCollection { get; private set; }
        public ObservableCollection<SaveFolder> SaveFolderHierarchy { get; private set; }

        public SaveFolder? RootSaveFolder { get; private set; }


        public int SelectedGame { get; set; } //CheckpointVM doesn't know about settings tab

        private Checkpoint? _selectedCheckpoint;
        public Checkpoint? SelectedCheckpoint
        {
            get { return _selectedCheckpoint; }
            set
            {
                _selectedCheckpoint = value;
                OnPropertyChanged(nameof(SelectedCheckpoint));
            }
        }

        private SaveFolder? _selectedSaveFolder;
        public SaveFolder? SelectedSaveFolder
        { get { return _selectedSaveFolder; }
            set
            {
                if (_selectedSaveFolder != value)
                {
                    if (value == null)
                    {
                        Trace.WriteLine("DEAR GOD WHY");
                    }
                    else
                    {
                        Trace.WriteLine("JGHG: " + value.SaveFolderPath);
                    }

                }

                _selectedSaveFolder = value;
                OnPropertyChanged(nameof(SelectedSaveFolder));
            }
        }


        // Evalutes whether the HCM tab is aligned with the current MCC in-game game. So buttons like dump can only work if you're actually in that game.
        private bool _selectedGameSameAsActualGame;
        public bool SelectedGameSameAsActualGame
        {
            get { return _selectedGameSameAsActualGame; }
            set
            { if (_selectedGameSameAsActualGame != value)
                {
                    _selectedGameSameAsActualGame = value;
                    OnPropertyChanged(nameof(SelectedGameSameAsActualGame));

                }
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

        public event EventHandler<int> RequestTabChange;

        public void InvokeRequestTabChange(int requestedTab)
        { 
        RequestTabChange?.Invoke(this, requestedTab);
        }


        private CheckpointServices CheckpointServices { get; init; }
        private CheatManagerService CheatManagerService { get; init; }
        public CheckpointViewModel(CheckpointServices checkpointServices, CheatManagerService cheatManagerService)
        {
            this.CheckpointServices = checkpointServices;
            this.CheatManagerService = cheatManagerService;
            this.CheckpointCollection = new();
            this.SaveFolderHierarchy = new();
            this.RootSaveFolder = null;


            //need to provide a reference to self to cheat_dump and cheat_inject

            ((InjectCheckpoint)this.CheatManagerService.ListOfCheats["InjectCheckpoint"]).CheckpointViewModel = this;
            ((DumpCheckpoint)this.CheatManagerService.ListOfCheats["DumpCheckpoint"]).CheckpointViewModel = this;

            ListCollectionView view = (ListCollectionView)CollectionViewSource
                    .GetDefaultView(this.CheckpointCollection);



            view.CustomSort = new SortCheckpointsByLastWriteTime();


            //Subscribe to AttachEvent so we can tell CheckpointModel to refreshList
            BurntMemory.Events.ATTACH_EVENT += (obj, args) =>
            {
                App.Current.Dispatcher.Invoke((Action)delegate // Need to make sure it's run on the UI thread
                {
                    RefreshCheckpointList();
                });
            };

            RefreshSaveFolderTree();
            RefreshCheckpointList();

        }

        // Move to services??
        public void RefreshCheckpointList()
        {
            this.CheckpointCollection.Clear();
            ObservableCollection<Checkpoint> newCollection = this.CheckpointServices.PopulateCheckpointList(this.SelectedSaveFolder, this.SelectedGame);
            foreach (Checkpoint c in newCollection)
            {
                this.CheckpointCollection.Add(c);
            }
            Trace.WriteLine("refreshed ccollection count: " + CheckpointCollection.Count);
        }

        public void RefreshSaveFolderTree()
        {
            this.SaveFolderHierarchy.Clear();
            ObservableCollection<SaveFolder> newHierarchy = this.CheckpointServices.PopulateSaveFolderTree(out SaveFolder? rootFolder, this.SelectedGame);
            this.RootSaveFolder = rootFolder;
            foreach (SaveFolder s in newHierarchy)
            {
                this.SaveFolderHierarchy.Add(s);
            }


                // Now let's try to set the selected folder to whatever folder was last selected on this tab
                if (this.RootSaveFolder != null)
                {
                    bool ableToSetLastFolder = false; // a flag that can be set in foreach loop and used for root folder fallback if not set

                string? lastSelectedFolder = null;
                if (Properties.Settings.Default.LastSelectedFolder != null && Properties.Settings.Default.LastSelectedFolder.Count > SelectedGame && SelectedGame >= 0)
                {
                    lastSelectedFolder = Properties.Settings.Default.LastSelectedFolder?[SelectedGame];
                }
                    
                if (lastSelectedFolder != null)
                    {
                        IEnumerable<SaveFolder> flattenedTree = FlattenTree(this.RootSaveFolder);
                        foreach (SaveFolder sf in flattenedTree)
                        {
                            // If it matches!
                            if (sf.SaveFolderPath == lastSelectedFolder && Directory.Exists(lastSelectedFolder))
                            {
                                Trace.WriteLine("Resetting last selected folder to " + sf.SaveFolderPath);
                                sf.IsSelected = true;
                                ableToSetLastFolder = true;
                            this.SelectedSaveFolder = sf;
                                break;
                            }
                        }
                    }
                    if (!ableToSetLastFolder)
                    {
                        // If we weren't able to reset it then default to root folder
                        this.RootSaveFolder.IsSelected = true;
                    this.SelectedSaveFolder = this.RootSaveFolder;
                    }

                }




            IEnumerable<SaveFolder> FlattenTree(SaveFolder node)
            {
                if (node == null)
                {
                    yield break;
                }
                yield return node;
                foreach (var n in node.Children)
                {
                    foreach (var innerN in FlattenTree(n))
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

                int? diff =  (int?)(cx.ModifiedOn - cy.ModifiedOn)?.TotalSeconds;
                return diff == null ? 0 : (int)diff;
            }
        }

        public void FolderChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            Trace.WriteLine("FolderChanged. Sender: " + sender + ", currentgame?: " + this.SelectedGame);
            SaveFolder? saveFolder = (SaveFolder?)e.NewValue;
            Trace.WriteLine("Selected Folder Path: " + saveFolder?.SaveFolderPath);
            //System.Diagnostics.StackTrace t = new System.Diagnostics.StackTrace();
            //Trace.WriteLine("stacktrace: " + t.ToString());
            if (saveFolder != null)
            {
                Trace.WriteLine("not null, so changing.");
                Properties.Settings.Default.LastSelectedFolder[this.SelectedGame] = saveFolder.SaveFolderPath;
                this.SelectedSaveFolder = saveFolder;
                this.RefreshCheckpointList();

            }
            else 
            {
                //this.RefreshSaveFolderTree();
            }



        }



        private ICommand _dump;
        public ICommand Dump
        {
            get { return _dump ?? (_dump = new DumpCommand(this, CheckpointServices, CheatManagerService)); }
            set { _dump = value; }
        }
        private ICommand _inject;
        public ICommand Inject
        {
            get { return _inject ?? (_inject = new InjectCommand(this, CheckpointServices, CheatManagerService)); }
            set { _inject = value; }
        }

        private ICommand _deleteCheckpoint;
        public ICommand DeleteCheckpoint
        {
            get { return _deleteCheckpoint ?? (_deleteCheckpoint = new DeleteCheckpointCommand(this, CheckpointServices)); }
            set { _deleteCheckpoint = value; }
        }

        private ICommand _renameCheckpoint;
        public ICommand RenameCheckpoint
        {
            get { return _renameCheckpoint ?? (_renameCheckpoint = new RenameCheckpointCommand(this, CheckpointServices)); }
            set { _renameCheckpoint = value; }
        }

        private ICommand _reVersionCheckpoint;
        public ICommand ReVersionCheckpoint
        {
            get { return _reVersionCheckpoint ?? (_reVersionCheckpoint = new ReVersionCheckpointCommand(this, CheckpointServices)); }
            set { _reVersionCheckpoint = value; }
        }

        private ICommand _sortCheckpoint;
        public ICommand SortCheckpoint
        {
            get { return _sortCheckpoint ?? (_sortCheckpoint = new SortCheckpointCommand(this, CheckpointServices)); }
            set { _sortCheckpoint = value; }
        }

        private ICommand _openInExplorer;
        public ICommand OpenInExplorer
        {
            get { return _openInExplorer ?? (_openInExplorer = new OpenInExplorerCommand(this, CheckpointServices)); }
            set { _openInExplorer = value; }
        }

        private ICommand _renameFolder;
        public ICommand RenameFolder
        {
            get { return _renameFolder ?? (_renameFolder = new RenameFolderCommand(this, CheckpointServices)); }
            set { _renameFolder = value; }
        }

        private ICommand _deleteFolder;
        public ICommand DeleteFolder
        {
            get { return _deleteFolder ?? (_deleteFolder = new DeleteFolderCommand(this, CheckpointServices)); }
            set { _deleteFolder = value; }
        }

        private ICommand _newFolder;
        public ICommand NewFolder
        {
            get { return _newFolder ?? (_newFolder = new NewFolderCommand(this, CheckpointServices)); }
            set { _newFolder = value; }
        }




    }




}
