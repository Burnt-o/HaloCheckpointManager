using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows;


namespace HCM3.ViewModels
{
    //heavily adapted from https://stackoverflow.com/a/21633849

    public class SettingParameter : INotifyPropertyChanged
    {
        public string SettingName { get; init; }
        public bool IsAutoUpdating { get; init; }
        private CheatViewModel ParentCheatViewModel { get; init; }

        private object _boundValue;
        public object BoundValue
        {
            get { return _boundValue; }
            set
            {
                _boundValue = value;
                if (IsAutoUpdating)
                {
                    try { _boundValue = Convert.ChangeType(_boundValue, Properties.Settings.Default[SettingName].GetType()); }
                    catch { _boundValue = Properties.Settings.Default[SettingName]; Trace.WriteLine("FAILEDCONVERSION"); }
                    Properties.Settings.Default[SettingName] = _boundValue;
                }

                RaisePropertyChanged(nameof(BoundValue));
            }
        }

        // Used by explicitly-updating settings when CancelSettingChanges is commanded
        private object OldValue { get; set; }

        public SettingParameter(string settingName, bool isAutoUpdating, CheatViewModel parentCheatViewModel)
        {
            _boundValue = Properties.Settings.Default[settingName];
            OldValue = _boundValue;
            SettingName = settingName;
            IsAutoUpdating = isAutoUpdating;
            ParentCheatViewModel = parentCheatViewModel;

            if (!IsAutoUpdating)
            {
                ParentCheatViewModel.APPLYEXPLICITSETTINGS_EVENT += ApplyThisExplicitSetting;
                ParentCheatViewModel.CANCELEXPLICITSETTINGS_EVENT += CancelThisExplicitSetting;
            }

            ParentCheatViewModel.RESETEXPLICITSETTINGS_EVENT += ParentCheatViewModel_RESETEXPLICITSETTINGS_EVENT;
        }

        private void ParentCheatViewModel_RESETEXPLICITSETTINGS_EVENT(object? sender, EventArgs e)
        {
            // Reset all settings to what they are when the user runs HCM for the very first time
            BoundValue = Properties.Settings.Default.Properties[SettingName].DefaultValue;
            Properties.Settings.Default[SettingName] = BoundValue;
            OldValue = BoundValue;
        }

        private void CancelThisExplicitSetting(object? sender, EventArgs e)
        {
            // Revert Value to OldValue
            Trace.WriteLine("Cancelling explicit setting: setting BoundValue to " + OldValue);
            BoundValue = OldValue;
        }

        private void ApplyThisExplicitSetting(object? sender, EventArgs e)
        {
            Trace.WriteLine("Applying explicit setting, SettingName: " + SettingName + ", SettingName.GetType(): " + Properties.Settings.Default[SettingName].GetType() + ", object: " + BoundValue + ", object.GetType(): " + BoundValue.GetType());
            try { BoundValue = Convert.ChangeType(BoundValue, Properties.Settings.Default[SettingName].GetType()); }
            catch { BoundValue = Properties.Settings.Default[SettingName]; Trace.WriteLine("FAILEDCONVERSION, rev to " + BoundValue); }

            OldValue = BoundValue;
            Properties.Settings.Default[SettingName] = BoundValue;
            RaisePropertyChanged(nameof(BoundValue));
        }

        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        public void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }



    }
}
