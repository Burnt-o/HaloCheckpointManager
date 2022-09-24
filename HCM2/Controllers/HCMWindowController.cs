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

        readonly ICollectionView _checkpointView;
        readonly HCMWindow _hcmWindow;

        #endregion // Data


        #region Constructor
        public HCMWindowController(HCMWindow hcmWindow, CheckpointCollection checkpointCollection)
        {
            if (hcmWindow == null)
                throw new ArgumentNullException("hcmWindow");

            if (checkpointCollection == null)
                throw new ArgumentNullException("checkpointCollection");

            _hcmWindow = hcmWindow;

            _checkpointView = CollectionViewSource.GetDefaultView(checkpointCollection);
        }
        #endregion // Constructor

        #region Command Handlers

        public bool IsACheckpointSelected
        {
            // Return true if there is a selected Checkpoint in the UI.
            get { return _checkpointView.CurrentItem != null; }
        }

        public void RenameSelectedCheckpoint(string newCheckpointName)
        {
                Checkpoint? selectedCheckpoint = _checkpointView.CurrentItem as Checkpoint;

                if (selectedCheckpoint == null)
                    throw new ArgumentNullException("_checkpointView.CurrentItem");

                selectedCheckpoint.CheckpointName = newCheckpointName;
        }

        #endregion // Command Handlers

    }
}
