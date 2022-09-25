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

        public void RenameSelectedGameSave(string newGameSaveName)
        {
                GameSave? selectedCheckpoint = _gameSaveTreeView.CurrentItem as GameSave;

                if (selectedCheckpoint == null)
                    throw new ArgumentNullException("_checkpointView.CurrentItem");

                selectedCheckpoint.GameSaveName = newGameSaveName;
        }

        #endregion // Command Handlers

    }
}
