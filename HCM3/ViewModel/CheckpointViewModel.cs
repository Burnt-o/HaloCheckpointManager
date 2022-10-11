using HCM3.ViewModel.MVVM;
using HCM3.Model;
using System.Collections.ObjectModel;
using System.Windows.Input;
using HCM3.Model.CheckpointModels;
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
using HCM3.ViewModel.Commands;

namespace HCM3.ViewModel
{

    internal partial class CheckpointViewModel : Presenter, IDropTarget
    {
        internal readonly CheckpointModel CheckpointModel;
        public ObservableCollection<Checkpoint> CheckpointCollection { get; private set; }
        public ObservableCollection<SaveFolder> SaveFolderHierarchy { get; private set; }

        public SaveFolder? RootSaveFolder { get; private set; }


        public MainViewModel MainViewModel { get; private set; }

        public MainModel MainModel { get; private set; }

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
                _selectedSaveFolder = value;
                OnPropertyChanged(nameof(SelectedSaveFolder));
            }
        }


        // Evalutes whether the HCM tab is aligned with the current MCC in-game game. So buttons like dump can only work if you're actually in that game.
        private bool _tabAlignedWithHaloState;
        public bool TabAlignedWithHaloState
        {
            get { return _tabAlignedWithHaloState;}
            set 
            { if (_tabAlignedWithHaloState != value)
                {
                    _tabAlignedWithHaloState = value;
                    OnPropertyChanged(nameof(TabAlignedWithHaloState));
                        
                   } 
            }
        }
        

        public CheckpointViewModel(CheckpointModel checkpointModel, MainViewModel mainViewModel, MainModel mainModel)
        {
            this.CheckpointModel = checkpointModel;
            this.CheckpointCollection = CheckpointModel.CheckpointCollection;
            this.SaveFolderHierarchy = CheckpointModel.SaveFolderHierarchy;
            this.RootSaveFolder = CheckpointModel.RootSaveFolder;
            this.MainViewModel = mainViewModel;
            this.MainModel = mainModel;

            ListCollectionView view = (ListCollectionView)CollectionViewSource
                    .GetDefaultView(this.CheckpointCollection);



            view.CustomSort = new SortCheckpointsByLastWriteTime();

            // Need to subscribe to HaloStateChanged of mainmodel, and tab changed of mainviewmodel,
            // So we can update TabAlignedWithInGame
            MainViewModel.PropertyChanged += (obj, args) => { UpdateTabAlignedWithHaloState(); };
            HaloStateEvents.HALOSTATECHANGED_EVENT += (obj, args) => { UpdateTabAlignedWithHaloState(); };

            //Subscribe to AttachEvent so we can tell CheckpointModel to refreshList
            HaloStateEvents.ATTACH_EVENT += (obj, args) =>
            {
                App.Current.Dispatcher.Invoke((Action)delegate // Need to make sure it's run on the UI thread
                {
                    this.CheckpointModel.RefreshCheckpointList(SelectedSaveFolder);
                });
            };

            

        }


        private void UpdateTabAlignedWithHaloState()
        {
            TabAlignedWithHaloState = (MainViewModel.SelectedTabIndex == MainModel.HaloMemory.HaloState.CurrentHaloState);
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
            SaveFolder? saveFolder = (SaveFolder?)e.NewValue;
            Trace.WriteLine("Selected Folder Path: " + saveFolder?.SaveFolderPath);

            if (saveFolder != null)
            {
                Properties.Settings.Default.LastSelectedFolder[MainViewModel.SelectedTabIndex] = saveFolder.SaveFolderPath;
            }



            CheckpointModel.RefreshCheckpointList(this.SelectedSaveFolder);


        }

        private ICommand _dump;
        public ICommand Dump
        {
            get { return _dump ?? (_dump = new DumpCommand(this)); }
            set { _dump = value; }
        }
        private ICommand _inject;
        public ICommand Inject
        {
            get { return _inject ?? (_inject = new InjectCommand(this)); }
            set { _inject = value; }
        }



        

        

    }




}
