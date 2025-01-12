using HCMExternal.Services.Hotkeys;
using HCMExternal.Services.Hotkeys.Impl;
using HCMExternal.ViewModels.Commands;
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

        private enum ToggleSettingName
        {
            CheckpointBeforeDump,
            RevertAfterInject
        }

        bool? DeserialiseToggle(ToggleSettingName toggleSettingName)
        {
#if !HCM_DEBUG
            throw new NotImplementedException("Need to implement SerialiseToggle");
#endif
            return null;
        }

        void SerialiseToggle(ToggleSettingName toggleSettingName, bool value)
        {
#if !HCM_DEBUG
            throw new NotImplementedException("Need to implement SerialiseToggle");
#endif
        }



        public TrainerViewModel(MainVMCommands mvmCommands, IHotkeyManager hotkeyManager)
        {
            ForceCheckpointViewModel = new HotkeyedButtonViewModel("Force Checkpoint", hotkeyManager, HotkeyEnum.ForceCheckpoint, mvmCommands.ForceCheckpoint);
            ForceRevertViewModel = new HotkeyedButtonViewModel("Force Revert", hotkeyManager, HotkeyEnum.ForceRevert, mvmCommands.ForceRevert);
            ForceDoubleRevertViewModel = new HotkeyedButtonViewModel("Force Double Revert", hotkeyManager, HotkeyEnum.DoubleRevert, mvmCommands.ForceDoubleRevert);
            DumpCheckpointViewModel = new HotkeyedButtonViewModel("Dump Checkpoint", hotkeyManager, HotkeyEnum.DumpCheckpoint, mvmCommands.DumpCheckpoint);
            InjectCheckpointViewModel = new HotkeyedButtonViewModel("Inject Checkpoint", hotkeyManager, HotkeyEnum.InjectCheckpoint, mvmCommands.InjectCheckpoint);


            CheckpointBeforeDumpViewModel = new ToggleViewModel(DeserialiseToggle(ToggleSettingName.CheckpointBeforeDump) ?? true, "Force Checkpoint before Dumping");
            RevertAfterInjectViewModel = new ToggleViewModel(DeserialiseToggle(ToggleSettingName.RevertAfterInject) ?? true, "Force Revert after Injecting");
            DisableNaturalCheckpointsViewModel = new HotkeyedToggleViewModel(false, "Disable Natural Checkpoints", hotkeyManager, HotkeyEnum.DisableCheckpoints);
            DisableNaturalCheckpointsViewModel.PropertyChanged += (o, e) => { mvmCommands.DisableCheckpoint.Execute(DisableNaturalCheckpointsViewModel.ToggleValue); };

        }

  

        ~TrainerViewModel()
        {
            SerialiseToggle(ToggleSettingName.CheckpointBeforeDump, CheckpointBeforeDumpViewModel.ToggleValue);
            SerialiseToggle(ToggleSettingName.RevertAfterInject, CheckpointBeforeDumpViewModel.ToggleValue);
        }

    }
}
