using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModel.MVVM;
using HCM3.Model;
using System.Collections.ObjectModel;
using System.Windows.Input;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Diagnostics;
using HCM3.Model.CheckpointModels;

namespace HCM3.ViewModel
{
    internal sealed class MainViewModel : Presenter
    {
        private int _selectedTabIndex; 
        public int SelectedTabIndex 
        {
            get { return _selectedTabIndex; }
            set 
            { 
            _selectedTabIndex = value;
            OnPropertyChanged(nameof(SelectedTabIndex));
            }
        }

        public MainModel MainModel { get; set; }

        public CheckpointViewModel CheckpointViewModel { get; init; }

        public MainViewModel(MainModel mainModel)
        {
            MainModel = mainModel;

            CheckpointViewModel = new(MainModel.CheckpointModel, this, mainModel);

            this.PropertyChanged += Handle_PropertyChanged;

            // Load in selected tab to what it was when HCM closed last
            SelectedTabIndex = Properties.Settings.Default.LastSelectedTab;
        }


        private void Handle_PropertyChanged(object? sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(SelectedTabIndex))
            {
               


                // Tell main model that the tab changed so it can refresh checkpoint details to be of the correct game/folder
                MainModel.HCMTabChanged(SelectedTabIndex);

                // Now let's try to set the selected folder to whatever folder was last selected on this tab
                if (MainModel.CheckpointModel.RootSaveFolder != null)
                {
                    // Get the last selected folder from the app settings string collection
                    string? lastSelectedFolder = Properties.Settings.Default.LastSelectedFolder?[SelectedTabIndex];
                    bool ableToSetLastFolder = false; // a flag that can be set in foreach loop and used for root folder fallback if not set
                    if (lastSelectedFolder != null)
                    {
                        IEnumerable<SaveFolder> flattenedTree = FlattenTree(MainModel.CheckpointModel.RootSaveFolder);
                        foreach (SaveFolder sf in flattenedTree)
                        {
                            // If it matches!
                            if (sf.SaveFolderPath == lastSelectedFolder)
                            {
                                Trace.WriteLine("Resetting last selected folder to " + sf.SaveFolderPath);
                                sf.IsSelected = true;
                                ableToSetLastFolder = true;
                                break;
                            }
                        }
                    }
                    if (!ableToSetLastFolder)
                    {
                        // If we weren't able to reset it then default to root folder
                        MainModel.CheckpointModel.RootSaveFolder.IsSelected = true;
                    }

                }
                

                // Save the selected tab to settings so we can load it in next time HCM starts
                Properties.Settings.Default.LastSelectedTab = SelectedTabIndex;
            }
        }

        private static IEnumerable<SaveFolder> FlattenTree(SaveFolder node)
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
}
