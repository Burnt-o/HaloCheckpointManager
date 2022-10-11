using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Model;
using BurntMemory;
using System.Collections.ObjectModel;
using HCM3.Model.CheckpointModels;
using System.Diagnostics;
using HCM3.Startup;


namespace HCM3.Model
{
    internal class MainModel
    {
        public DataPointers DataPointers { get; init; }
        public HaloMemory HaloMemory { get; init; }

        public CheckpointModels.CheckpointModel CheckpointModel { get; init; }

        public string? CurrentAttachedMCCVersion { get; set; }
        public string? HighestSupportMCCVersion { get; set; }
        public int SelectedTabIndex { get; set; }

        // Constructor
        public MainModel()
        {
            HCMSetup setup = new();
            // Run some checks; have admin priviledges, have file access, have required folders & files.
            if (!setup.HCMSetupChecks(out string errorMessage))
            {
                // If a check fails, tell the user why, then shutdown the application.
                System.Windows.MessageBox.Show(errorMessage, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
                System.Windows.Application.Current.Shutdown();
            }

            // Create collection of all our ReadWrite.Pointers (and other data) and load them from the online repository
            DataPointers = new();
            if (!DataPointers.LoadPointerDataFromGit(out string error, out string? highestSupportMCCVersion))
            {
                System.Windows.MessageBox.Show(error, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
                System.Windows.Application.Current.Shutdown();
            }


            HighestSupportMCCVersion = highestSupportMCCVersion;

            // Halo Memory, our extension of BurntMemory for read/write and other interactions with MCCs memory
            HaloMemory = new(this);

            // CheckpointModel handles the data used by the Checkpoint Library
            CheckpointModel = new(this);

            // Subscribe to attach/detach of MCC process
            BurntMemory.Events.ATTACH_EVENT += Events_ATTACH_EVENT;
            BurntMemory.Events.DEATTACH_EVENT += Events_DEATTACH_EVENT;

            // Tell HaloMemory to try to attach to MCC, both steam and winstore versions
            HaloMemory.HaloState.ProcessesToAttach = new string[] { "MCC-Win64-Shipping", "MCCWinStore-Win64-Shipping" };
            HaloMemory.HaloState.TryToAttachTimer.Enabled = true;
            

        }

        // Popped by BurntMemory.HaloState(aka AttachState) when it detaches from the MCC process
        private void Events_DEATTACH_EVENT(object? sender, EventArgs e)
        {
            CurrentAttachedMCCVersion = null;
            HaloMemory.HaloState.TryToAttachTimer.Enabled = true;
            Trace.WriteLine("MainModel detected BurntMemory DEtach; Set current MCC version to null");
        }

        // Popped by BurntMemory.HaloState(aka AttachState) when it attaches to the MCC process
        private void Events_ATTACH_EVENT(object? sender, Events.AttachedEventArgs e)
        {
            string? potentialVersion = HaloMemory.HaloState.ProcessVersion;
            if (potentialVersion != null && potentialVersion.StartsWith("1."))
            {
                CurrentAttachedMCCVersion = potentialVersion;
            }
            else
            {
                CurrentAttachedMCCVersion = null;
            }

            Trace.WriteLine("MainModel detected BurntMemory attach; Set current MCC version to " + CurrentAttachedMCCVersion);

            // Also we want to refresh the checkpoint list in case the CurrentAttachedMCCVersion changed (it's used to read data from checkpoints)


        }

        // MainViewModel triggers an event when the Tab Control has it's tab changed, which calls this. 
        public void OnHCMTabChanged(int selectedTabIndex, SaveFolder? SelectedSaveFolder)
        {
            SelectedTabIndex = selectedTabIndex;

            App.Current.Dispatcher.Invoke((Action)delegate // Need to make sure it's run on the UI thread
            {
                this.CheckpointModel.RefreshSaveFolderTree();
                this.CheckpointModel.RefreshCheckpointList(SelectedSaveFolder);
            });
        }



    }
}
