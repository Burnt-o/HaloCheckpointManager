using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using System.Windows;


namespace HCM3.ViewModels
{
    internal class SettingsViewModel : Presenter
    {

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

        public bool AutoRevert
        {
            get { return Properties.Settings.Default.AutoRevert; }
            set { Properties.Settings.Default.AutoRevert = value; }
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

    }
}
