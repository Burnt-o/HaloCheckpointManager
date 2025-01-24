using HCMExternal.Services.Hotkeys;
using HCMExternal.Services.Hotkeys.Impl;
using HCMExternal.ViewModels.Commands;
using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.ViewModels
{
    public class TrainerViewModel
    {

        public HotkeyedButtonViewModel ForceCheckpointViewModel { get; init; }
        public HotkeyedButtonViewModel ForceRevertViewModel { get; init; }
        public HotkeyedButtonViewModel ForceDoubleRevertViewModel { get; init; }
        public HotkeyedButtonViewModel DumpCheckpointViewModel { get; init; }
        public HotkeyedButtonViewModel InjectCheckpointViewModel { get; init; }
        public ToggleViewModel CheckpointBeforeDumpViewModel { get; init; }
        public ToggleViewModel RevertAfterInjectViewModel { get; init; }
        public HotkeyedToggleViewModel DisableNaturalCheckpointsViewModel { get; init; }



        public TrainerViewModel(MainVMCommands mvmCommands, IHotkeyManager hotkeyManager)
        {
            ForceCheckpointViewModel = new HotkeyedButtonViewModel("Force Checkpoint", hotkeyManager, HotkeyEnum.ForceCheckpoint, mvmCommands.ForceCheckpoint);
            ForceRevertViewModel = new HotkeyedButtonViewModel("Force Revert", hotkeyManager, HotkeyEnum.ForceRevert, mvmCommands.ForceRevert);
            ForceDoubleRevertViewModel = new HotkeyedButtonViewModel("Force Double Revert", hotkeyManager, HotkeyEnum.DoubleRevert, mvmCommands.ForceDoubleRevert);
            DumpCheckpointViewModel = new HotkeyedButtonViewModel("Dump Checkpoint", hotkeyManager, HotkeyEnum.DumpCheckpoint, mvmCommands.DumpCheckpoint);
            InjectCheckpointViewModel = new HotkeyedButtonViewModel("Inject Checkpoint", hotkeyManager, HotkeyEnum.InjectCheckpoint, mvmCommands.InjectCheckpoint);


            CheckpointBeforeDumpViewModel = new ToggleViewModel(Properties.Settings.Default.CheckpointBeforeDump, "Force Checkpoint before Dumping");
            CheckpointBeforeDumpViewModel.PropertyChanged += (o, e) =>
            {
                Log.Verbose("Changing CheckpointBeforeDump value to " + CheckpointBeforeDumpViewModel.ToggleValue);
                Properties.Settings.Default.CheckpointBeforeDump = CheckpointBeforeDumpViewModel.ToggleValue;
                Properties.Settings.Default.Save();
            };



            RevertAfterInjectViewModel = new ToggleViewModel(Properties.Settings.Default.RevertAfterInject, "Force Revert after Injecting");
            RevertAfterInjectViewModel.PropertyChanged += (o, e) =>
            {
                Log.Verbose("Changing RevertAfterInject value to " + RevertAfterInjectViewModel.ToggleValue);
                Properties.Settings.Default.RevertAfterInject = RevertAfterInjectViewModel.ToggleValue;
                Properties.Settings.Default.Save();
            };


            DisableNaturalCheckpointsViewModel = new HotkeyedToggleViewModel(false, "Disable Natural Checkpoints", hotkeyManager, HotkeyEnum.DisableCheckpoints);
            DisableNaturalCheckpointsViewModel.PropertyChanged += (o, e) => { mvmCommands.DisableCheckpoint.Execute(DisableNaturalCheckpointsViewModel.ToggleValue); };

        }


    }
}
