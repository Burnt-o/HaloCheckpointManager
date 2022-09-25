using System;
using System.ComponentModel;
using System.IO;
using System.Reflection;
using System.Windows.Data;
using HCM.Model;
using HCM.Views;

namespace HCM.Controllers
{
    internal class HCMWindowController
    {
        #region Data

        readonly ICollectionView _gameSaveTreeView;
        readonly HCMWindow _hcmWindow;

        #endregion // Data


        #region Constructor
        public HCMWindowController(HCMWindow hcmWindow, GameSaveCollection gameSaveCollection)
        {
            if (hcmWindow == null)
                throw new ArgumentNullException("hcmWindow");

            if (gameSaveCollection == null)
                throw new ArgumentNullException("gameSaveCollection");

            _hcmWindow = hcmWindow;

            _gameSaveTreeView = CollectionViewSource.GetDefaultView(gameSaveCollection);
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
                GameSave? selectedItem = _gameSaveTreeView.CurrentItem as GameSave;

                if (selectedItem == null)
                    throw new ArgumentNullException(nameof(selectedItem));

                selectedItem.Name = newItemName;
        }

        #endregion // Command Handlers

    }
}
