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

namespace HCM3.Model
{
    internal class MainModel
    {
        public PointerCollection PointerCollection { get; init; }
        public HaloMemory HaloMemory { get; init; }

        public CheckpointModels.CheckpointModel CheckpointModel { get; init; }

        public string? CurrentAttachedVersion { get; set; }
        public int SelectedTabIndex { get; set; }

        public MainModel(PointerCollection pcollection)
        { 
        PointerCollection = pcollection;
        HaloMemory = new(this);
        CheckpointModel = new(this);

            BurntMemory.Events.ATTACH_EVENT += Events_ATTACH_EVENT;
            BurntMemory.Events.DEATTACH_EVENT += Events_DEATTACH_EVENT;
            HaloMemory.HaloState.ProcessesToAttach = new string[] { "MCC-Win64-Shipping", "MCCWinStore-Win64-Shipping" };
            HaloMemory.HaloState.TryToAttachTimer.Enabled = true;
            

        }

        private void Events_DEATTACH_EVENT(object? sender, EventArgs e)
        {
            CurrentAttachedVersion = null;
            Trace.WriteLine("MainModel detected BurntMemory DEtach; Set current MCC version to null");
        }

        private void Events_ATTACH_EVENT(object? sender, Events.AttachedEventArgs e)
        {
            
            CurrentAttachedVersion = HaloMemory.HaloState.CurrentMCCVersion;
            Trace.WriteLine("MainModel detected BurntMemory attach; Set current MCC version");
            // TODO: add check on this event in mainviewmodel, that checks if mcc version is in pointercollections list of supported versions and pops a warning if not.
            CheckpointModel.RefreshCheckpointList(); // CheckpointList cares about the current MCC version when trying to interpret .bins that don't have a version string. It'll assume they are current version.
        }

        // MainViewModel triggers an event when the Tab Control has it's tab changed, which calls this. 
        public void HCMTabChanged(int selectedTabIndex)
        {
            SelectedTabIndex = selectedTabIndex;
            CheckpointModel.RefreshCheckpointList();
            CheckpointModel.RefreshSaveFolderTree();
        }

    }
}
