using System;
using System.ComponentModel;
using System.IO;
using System.Reflection;
using System.Windows.Data;
using HCM.Model;
using HCM.Views;
using System.Collections.Generic;
using System.Collections.ObjectModel;



namespace HCM.Controllers
{
    internal class HCMWindowController
    {
        #region Data

        readonly ICollectionView _gameSaveTreeView;
        readonly HCMWindow _hcmWindow;

        #endregion // Data


        #region Constructor
        public HCMWindowController(HCMWindow hcmWindow, ObservableCollection<TreeItem> treeItems)
        {
            if (hcmWindow == null)
                throw new ArgumentNullException("hcmWindow");

            if (treeItems == null)
                throw new ArgumentNullException("gameSaveCollection");

            _hcmWindow = hcmWindow;

            _gameSaveTreeView = CollectionViewSource.GetDefaultView(treeItems);
        }
        #endregion // Constructor

        #region Command Handlers

        public bool IsACheckpointSelected
        {
            // Return true if there is a selected Checkpoint in the UI.
            get { return _gameSaveTreeView.CurrentItem != null; }
        }



        public void RenameSelectedItem(string newItemName)
        {

                TreeItem? selectedItem = _gameSaveTreeView.CurrentItem as TreeItem;
                TreeItem? temp =  

                if (selectedItem == null)
                    throw new ArgumentNullException("selectedItem");

                selectedItem.Name = "oh hi mark";
        }

        #endregion // Command Handlers

    }
}
