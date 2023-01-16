using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using System.Windows;
using HCM3.Services.Trainer;

namespace HCM3.ViewModels
{
    internal class SettingsViewModel : Presenter
    {
        public InternalServices InternalServices { get; set; }
        public PersistentCheatService PersistentCheatService { get; set; }

        public SettingsViewModel(InternalServices internalServices, PersistentCheatService persistentCheatService)
        { 
        InternalServices = internalServices;
            PersistentCheatService = persistentCheatService;
        }

        public bool DarkMode
        {
            get { return Properties.Settings.Default.Darkmode; }
            set {
                if (Properties.Settings.Default.Darkmode != value)
                {
                    Properties.Settings.Default.Darkmode = value;
                    string appTheme = HCM3.Properties.Settings.Default.Darkmode ? "Dark" : "Light";
                    Application.Current.Resources.MergedDictionaries[0].Source = new Uri($"/Themes/{appTheme}.xaml", UriKind.Relative);
                }
            }
        }

        public bool AutoName
        {
            get { return Properties.Settings.Default.AutoName; }
            set { Properties.Settings.Default.AutoName = value; }
        }

        public bool CheckForUpdates
        {
            get { return Properties.Settings.Default.CheckForUpdates; }
            set { Properties.Settings.Default.CheckForUpdates = value; }
        }

        public bool AutoRevert
        {
            get { return Properties.Settings.Default.AutoRevert; }
            set { Properties.Settings.Default.AutoRevert = value; }
        }

        public bool AutoCheckpoint
        {
            get { return Properties.Settings.Default.AutoCheckpoint; }
            set { Properties.Settings.Default.AutoCheckpoint = value; }
        }

        public bool H1Cores
        {
            get { return Properties.Settings.Default.H1Cores; }
            set { Properties.Settings.Default.H1Cores = value; }
        }

        public bool WarnInjectWrongVersion
        {
            get { return Properties.Settings.Default.WarnInjectWrongVersion; }
            set { Properties.Settings.Default.WarnInjectWrongVersion = value; }
        }

        public bool WarnInjectWrongLevel
        {
            get { return Properties.Settings.Default.WarnInjectWrongLevel; }
            set { Properties.Settings.Default.WarnInjectWrongLevel = value; }
        }

        public bool DisableOverlay
        {
            get { return Properties.Settings.Default.DisableOverlay; }
            set { 
                Properties.Settings.Default.DisableOverlay = value;
                
                if (value)
                {
                    InternalServices.RemoveHook();
                    PersistentCheatService.RemoveAllCheats();
                }
                else
                {
                    InternalServices.InjectInternal();
                }
            
            }
        }

        public Visibility DisableOverlayOptionEnabled
        {
            get 
            { 
                return Properties.Settings.Default.DisableOverlayOptionEnabled ? Visibility.Visible : Visibility.Collapsed;
            }
        }

        public bool ImageMode
        {
            get { return Properties.Settings.Default.ImageMode; }
            set { Properties.Settings.Default.ImageMode = value; }
        }

    }
}
