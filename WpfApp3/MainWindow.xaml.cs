using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using System.Reflection;
using Microsoft.VisualBasic;
using Newtonsoft.Json;

namespace WpfApp3
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>

    public partial class MainWindow : Window
    {
        private class HCMConfig
        {
            public string CoreFolderPath;
            public string CheckpointFolderPath;
            public bool ClassicMode;
        }

        private static class HCMGlobal
        {
            public static readonly string LocalDir = System.IO.Path.GetDirectoryName(System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName);
            public static readonly string H1CoreSavePath = HCMGlobal.LocalDir + @"\saves\h1cs";
            public static readonly string H1CheckpointPath = HCMGlobal.LocalDir + @"\saves\h1cp";
            public static readonly string H2CheckpointPath = HCMGlobal.LocalDir + @"\saves\h2cp";
            public static readonly string ConfigPath = HCMGlobal.LocalDir + @"\config.json";
            public static readonly string LogPath = HCMGlobal.LocalDir + @"\log.txt";

            public static HCMConfig SavedConfig;

            public static string ImageModeSuffix => SavedConfig.ClassicMode ? "clas" : "anni";

            public static bool padowomode = false;
        }

        private enum HaloGame
        {
            Halo1,
            Halo2,
            Halo3,
            HaloODST,
            HaloReach,
            Halo4,
            Halo5
        }

        private enum Difficulty
        {
            Invalid = -1,
            Easy = 0,
            Normal = 1,
            Heroic = 2,
            Legendary = 3,
        }

        private readonly string[] RequiredFiles =
        {
            @"config.json",
            @"log.txt"
        };

        private readonly string[] RequiredFolders =
        {
            @"saves",
            @"saves\h1cs",
            @"saves\h1cp",
            @"saves\h2cp",
        };

        public MainWindow()
        {
            InitializeComponent();
            CS_MainList.SelectionChanged += List_SelectionChanged;
            CP_MainList.SelectionChanged += List_SelectionChanged;
            H2CP_MainList.SelectionChanged += List_SelectionChanged;

            // Validate that required folders exist
            foreach (var folder in RequiredFolders)
            {
                var folderPath = $@"{HCMGlobal.LocalDir}\{folder}";
                try
                {
                    Directory.CreateDirectory(folderPath);
                }
                catch (Exception exp)
                {
                    Log($@"Exception creating folder {folderPath}: {exp}");
                }
            }

            // Validate that required files exist
            foreach (var file in RequiredFiles)
            {
                var filePath = $@"{HCMGlobal.LocalDir}\{file}";
                try
                {
                    if (!File.Exists(filePath))
                    {
                        File.CreateText(filePath).Close();
                    }
                }
                catch (Exception exp)
                {
                    Log($@"Exception creating file {filePath}: {exp}");
                }
            }

            // Set up Config
            using (StreamReader r = new StreamReader(HCMGlobal.ConfigPath))
            {
                string json = r.ReadToEnd();
                HCMGlobal.SavedConfig = JsonConvert.DeserializeObject<HCMConfig>(json);
            }

            CS_MainList.Items.Clear();
            CP_MainList.Items.Clear();
            H2CP_MainList.Items.Clear();
            RefreshButton.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));

            if (DateTime.Now.Day == 25 && DateTime.Now.Month == 12)
            {
                HCMGlobal.padowomode = true;
                H2CP_Sel_ConvertButton.Content = "Convert to PADAWO";
            }
        }

        void List_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            RefreshSel(sender, e);
        }

        private void MoveUpButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);
            System.Type type = sender.GetType();
            string s = (string)type.GetProperty("Name").GetValue(sender, null);
            string movethis = "";
            string abovefile = "";
            switch (s)
            {
                case "CS_Sel_MoveUpButton":
                    if (CS_MainList.SelectedItem != null && CS_MainList.SelectedIndex != 0)
                    {
                        var item = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        movethis = HCMGlobal.H1CoreSavePath + @"\" + s2 + @".bin";

                        var item2 = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex - 1);
                        System.Type type3 = item2.GetType();
                        string s3 = (string)type3.GetProperty("Name").GetValue(item2, null);
                        abovefile = HCMGlobal.H1CoreSavePath + @"\" + s3 + @".bin";
                    }
                    break;

                case "CP_Sel_MoveUpButton":
                    if (CP_MainList.SelectedItem != null && CP_MainList.SelectedIndex != 0)
                    {
                        var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        movethis = HCMGlobal.H1CheckpointPath + @"\" + s2 + @".bin";

                        var item2 = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex - 1);
                        System.Type type3 = item2.GetType();
                        string s3 = (string)type3.GetProperty("Name").GetValue(item2, null);
                        abovefile = HCMGlobal.H1CheckpointPath + @"\" + s3 + @".bin";
                    }
                    break;

                case "H2CP_Sel_MoveUpButton":
                    if (H2CP_MainList.SelectedItem != null && H2CP_MainList.SelectedIndex != 0)
                    {
                        var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        movethis = HCMGlobal.H2CheckpointPath + @"\" + s2 + @".bin";

                        var item2 = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex - 1);
                        System.Type type3 = item2.GetType();
                        string s3 = (string)type3.GetProperty("Name").GetValue(item2, null);
                        abovefile = HCMGlobal.H2CheckpointPath + @"\" + s3 + @".bin";
                    }
                    break;

                default:
                    break;

            }

            if (File.Exists(movethis) && File.Exists(abovefile))
            {
                DateTime currentfilestime = File.GetLastWriteTime(movethis);
                DateTime abovefilestime = File.GetLastWriteTime(abovefile);
                File.SetLastWriteTime(movethis, abovefilestime);
                File.SetLastWriteTime(abovefile, currentfilestime);
                switch (s)
                {
                    case "CS_Sel_MoveUpButton":
                        CS_MainList.SelectedIndex = CS_MainList.SelectedIndex - 1;
                        break;
                    case "CP_Sel_MoveUpButton":
                        CP_MainList.SelectedIndex = CP_MainList.SelectedIndex - 1;
                        break;
                    case "H2CP_Sel_MoveUpButton":
                        H2CP_MainList.SelectedIndex = H2CP_MainList.SelectedIndex - 1;
                        break;
                    default:
                        break;
                }

                RefreshList(sender, e);
            }
            else
            {
                Log("something went wrong: fileexists(movethis): " + File.Exists(movethis).ToString(), sender);
                Log("something went wrong: fileexists(abovefile): " + File.Exists(abovefile).ToString(), sender);
            }
        }

        private void MoveDownButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);
            System.Type type = sender.GetType();
            string s = (string)type.GetProperty("Name").GetValue(sender, null);
            string movethis = "";
            string belowfile = "";
            switch (s)
            {
                case "CS_Sel_MoveDownButton":
                    if (CS_MainList.SelectedItem != null && CS_MainList.SelectedIndex != CS_MainList.Items.Count - 1)
                    {
                        var item = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        movethis = HCMGlobal.H1CoreSavePath + @"\" + s2 + @".bin";

                        var item2 = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex + 1);
                        System.Type type3 = item2.GetType();
                        string s3 = (string)type3.GetProperty("Name").GetValue(item2, null);
                        belowfile = HCMGlobal.H1CoreSavePath + @"\" + s3 + @".bin";
                    }
                    break;

                case "CP_Sel_MoveDownButton":
                    if (CP_MainList.SelectedItem != null && CP_MainList.SelectedIndex != CP_MainList.Items.Count - 1)
                    {
                        var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        movethis = HCMGlobal.H1CheckpointPath + @"\" + s2 + @".bin";

                        var item2 = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex + 1);
                        System.Type type3 = item2.GetType();
                        string s3 = (string)type3.GetProperty("Name").GetValue(item2, null);
                        belowfile = HCMGlobal.H1CheckpointPath + @"\" + s3 + @".bin";
                    }
                    break;

                case "H2CP_Sel_MoveDownButton":
                    if (H2CP_MainList.SelectedItem != null && H2CP_MainList.SelectedIndex != H2CP_MainList.Items.Count - 1)
                    {
                        var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        movethis = HCMGlobal.H2CheckpointPath + @"\" + s2 + @".bin";

                        var item2 = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex + 1);
                        System.Type type3 = item2.GetType();
                        string s3 = (string)type3.GetProperty("Name").GetValue(item2, null);
                        belowfile = HCMGlobal.H2CheckpointPath + @"\" + s3 + @".bin";
                    }
                    break;

                default:
                    break;

            }
            if (File.Exists(movethis) && File.Exists(belowfile))
            {
                DateTime currentfilestime = File.GetLastWriteTime(movethis);
                DateTime belowfilestime = File.GetLastWriteTime(belowfile);
                File.SetLastWriteTime(movethis, belowfilestime);
                File.SetLastWriteTime(belowfile, currentfilestime);
                switch (s)
                {
                    case "CS_Sel_MoveDownButton":
                        CS_MainList.SelectedIndex = CS_MainList.SelectedIndex + 1;
                        break;
                    case "CP_Sel_MoveDownButton":
                        CP_MainList.SelectedIndex = CP_MainList.SelectedIndex + 1;
                        break;
                    case "H2CP_Sel_MoveDownButton":
                        H2CP_MainList.SelectedIndex = H2CP_MainList.SelectedIndex + 1;
                        break;
                    default:
                        break;
                }

                RefreshList(sender, e);
            }
            else
            {
                Log("something went wrong: fileexists movethis: " + File.Exists(movethis).ToString(), sender);
                Log("something went wrong: fileexists belowfile: " + File.Exists(belowfile).ToString(), sender);
            }
        }

        private void SaveButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshLoa(sender, e);

            string backuploc = "";
            string pathtotest = "";
            System.Type type = sender.GetType();
            string s = (string)type.GetProperty("Name").GetValue(sender, null);

            switch (s)
            {
                case "CS_Loa_SaveButton":
                    backuploc = HCMGlobal.H1CoreSavePath;
                    if (HCMGlobal.SavedConfig.CoreFolderPath != null)
                        pathtotest = HCMGlobal.SavedConfig.CoreFolderPath + @"\core.bin";
                    break;

                case "CP_Loa_SaveButton":
                    backuploc = HCMGlobal.H1CheckpointPath;
                    if (HCMGlobal.SavedConfig.CheckpointFolderPath != null)
                        pathtotest = HCMGlobal.SavedConfig.CheckpointFolderPath + @"\autosave_Halo1.bin";
                    break;

                case "H2CP_Loa_SaveButton":
                    backuploc = HCMGlobal.H2CheckpointPath;
                    if (HCMGlobal.SavedConfig.CheckpointFolderPath != null)
                        pathtotest = HCMGlobal.SavedConfig.CheckpointFolderPath + @"\autosave_Halo2.bin";
                    break;

                default:
                    break;

            }

            if (File.Exists(pathtotest) && Directory.Exists(backuploc) && pathtotest != "")
            {
                var userinput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                                   "Name your backup save",
                                   "",
                                   -1, -1);
                string proposedsave = (backuploc + @"\" + userinput + @".bin");

                //Console.WriteLine("proposed save: " + proposedsave);

                try
                {
                    File.Copy(pathtotest, proposedsave);
                    RefreshList(sender, e);
                }
                catch (Exception exp)
                {
                    Log("something went wrong trying to save a save: " + exp, sender);
                    //need to make this a popup to let user know what was bad
                }
            }
        }

        private void LoadButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);
            RefreshLoa(sender, e);

            string backuploc = "";
            string pathtotest = "";
            System.Type type = sender.GetType();
            string s = (string)type.GetProperty("Name").GetValue(sender, null);
            var testme = "";

            switch (s)
            {
                case "CS_Sel_LoadButton":
                    if (CS_MainList.SelectedItem != null)
                    {
                        var item = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backuploc = HCMGlobal.H1CoreSavePath + @"\" + s2 + @".bin";
                        pathtotest = HCMGlobal.SavedConfig.CoreFolderPath + @"\core.bin";
                    }
                    break;

                case "CP_Sel_LoadButton":
                    if (CP_MainList.SelectedItem != null)
                    {
                        var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backuploc = HCMGlobal.H1CheckpointPath + @"\" + s2 + @".bin";
                        pathtotest = HCMGlobal.SavedConfig.CheckpointFolderPath + @"\autosave_Halo1.bin";
                    }
                    break;

                case "H2CP_Sel_LoadButton":
                    if (H2CP_MainList.SelectedItem != null)
                    {
                        var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backuploc = HCMGlobal.H2CheckpointPath + @"\" + s2 + @".bin";
                        pathtotest = HCMGlobal.SavedConfig.CheckpointFolderPath + @"\autosave_Halo2.bin";
                    }
                    break;

                default:
                    break;

            }

            try
            {
                testme = System.IO.Path.GetDirectoryName(pathtotest);
            }
            catch { }
            if (Directory.Exists(testme) && File.Exists(backuploc))
            {
                try
                {
                    File.Copy(backuploc, pathtotest, true);
                    RefreshLoa(sender, e);
                }
                catch (Exception exp)
                {
                    Log("something went wrong trying to load a save: " + exp);
                }
            }
            else
            {
                //need to make this a popup to let user know what was bad
                Log("something went wrong trying to load a save: Directory.Exists(testme) : " + Directory.Exists(testme).ToString(), sender);
                Log("something went wrong trying to load a save: File.Exists(backuploc) : " + File.Exists(backuploc).ToString(), sender);
            }
        }

        private void RenameButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);
            string backuploc = "";
            string backup = "";
            System.Type type = sender.GetType();
            string s = (string)type.GetProperty("Name").GetValue(sender, null);
            string s2 = "";
            string proposedsave = "";

            switch (s)
            {
                case "CS_Sel_RenameButton":
                    if (CS_MainList.SelectedItem != null)
                    {
                        var item = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backup = HCMGlobal.H1CoreSavePath + @"\" + s2 + @".bin";
                        backuploc = HCMGlobal.H1CoreSavePath;
                        var userinput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                 "Rename your backup save",
                         s2,
                             -1, -1);
                        proposedsave = (backuploc + @"\" + userinput + @".bin");
                    }
                    break;

                case "CP_Sel_RenameButton":
                    if (CP_MainList.SelectedItem != null)
                    {
                        var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backup = HCMGlobal.H1CheckpointPath + @"\" + s2 + @".bin";
                        backuploc = HCMGlobal.H1CheckpointPath;
                        var userinput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                 "Rename your backup save",
                         s2,
                             -1, -1);
                        proposedsave = (backuploc + @"\" + userinput + @".bin");
                    }
                    break;

                case "H2CP_Sel_RenameButton":
                    if (H2CP_MainList.SelectedItem != null)
                    {
                        var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backup = HCMGlobal.H2CheckpointPath + @"\" + s2 + @".bin";
                        backuploc = HCMGlobal.H2CheckpointPath;
                        var userinput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                 "Rename your backup save",
                         s2,
                             -1, -1);
                        proposedsave = (backuploc + @"\" + userinput + @".bin");
                    }
                    break;

                default:
                    break;

            }

            if (File.Exists(backup) && !File.Exists(proposedsave))
            {
                try
                {
                    File.Move(backup, proposedsave);
                    RefreshList(sender, e);
                    RefreshSel(sender, e);
                }
                catch (Exception exp)
                {
                    Log("something went wrong trying to rename a save: " + exp, sender);
                    //need to make this a popup to let user know what was bad
                }
            }
            else
            {
                //need to make this a popup to let user know what was bad
                Log("something went wrong trying to rename a save: File.Exists(backuploc) : " + File.Exists(backup).ToString());
                Log("something went wrong trying to rename a save: !File.Exists(proposedsave) : " + (!File.Exists(proposedsave)).ToString());
                //actually this can popup if the user just cancels the input box.
            }

        }

        private void ConvertButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);
            string convertfrom = "";
            string convertto = "";
            string converttoloc = "";
            System.Type type = sender.GetType();
            string s = (string)type.GetProperty("Name").GetValue(sender, null);
            string s2 = "";

            switch (s)
            {
                case "CS_Sel_ConvertButton":

                    if (CS_MainList.SelectedItem != null)
                    {
                        var item = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        convertfrom = HCMGlobal.H1CoreSavePath + @"\" + s2 + @".bin";
                        convertto = HCMGlobal.H1CheckpointPath + @"\" + s2 + @".bin";
                        converttoloc = HCMGlobal.H1CheckpointPath;
                    }
                    break;

                case "CP_Sel_ConvertButton":
                    if (CP_MainList.SelectedItem != null)
                    {
                        var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        convertfrom = HCMGlobal.H1CheckpointPath + @"\" + s2 + @".bin";
                        convertto = HCMGlobal.H1CoreSavePath + @"\" + s2 + @".bin";
                        converttoloc = HCMGlobal.H1CoreSavePath;
                    }
                    break;

                case "H2CP_Sel_ConvertButton":
                    string link = HCMGlobal.padowomode 
                        ? "https://www.youtube.com/watch?v=dQ_d_VKrFgM"
                        : "https://www.youtube.com/watch?v=5u4tQlVRKD8";

                    var psi = new System.Diagnostics.ProcessStartInfo
                    {
                        FileName = link,
                        UseShellExecute = true
                    };
                    System.Diagnostics.Process.Start(psi);
                    break;


                default:
                    break;

            }


            if (File.Exists(convertfrom) && Directory.Exists(converttoloc) && !(File.Exists(convertto)))
            {

                try
                {
                    File.Copy(convertfrom, convertto);
                    RefreshList(sender, e);
                }
                catch (Exception exp)
                {
                    Log("something went wrong trying to convert a save: " + exp, sender);
                    //need to make this a popup to let user know what was bad
                }
            }
            else
            {
                //need to make this a popup to let user know what was bad
                Log("something went wrong trying to convert a save: File.Exists(convertfrom) : " + File.Exists(convertfrom).ToString(), sender);
                Log("something went wrong trying to convert a save: Directory.Exists(converttoloc) : " + Directory.Exists(converttoloc).ToString(), sender);
                Log("something went wrong trying to convert a save: !File.Exists(convertto) : " + (!File.Exists(convertto)).ToString(), sender);
            }


        }

        private void DeleteButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);
            var oldCSselected = CS_MainList.SelectedIndex;
            var oldCPselected = CP_MainList.SelectedIndex;
            string backup = "";
            System.Type type = sender.GetType();
            string s = (string)type.GetProperty("Name").GetValue(sender, null);


            switch (s)
            {
                case "CS_Sel_DeleteButton":
                    if (CS_MainList.SelectedItem != null)
                    {
                        var item = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backup = HCMGlobal.H1CoreSavePath + @"\" + s2 + @".bin";
                    }
                    break;

                case "CP_Sel_DeleteButton":
                    if (CP_MainList.SelectedItem != null)
                    {
                        var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backup = HCMGlobal.H1CheckpointPath + @"\" + s2 + @".bin";
                    }
                    break;

                case "H2CP_Sel_DeleteButton":
                    if (H2CP_MainList.SelectedItem != null)
                    {
                        var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backup = HCMGlobal.H2CheckpointPath + @"\" + s2 + @".bin";
                    }
                    break;

                default:
                    break;

            }

            if (File.Exists(backup))
            {
                try
                {
                    File.Delete(backup);
                    RefreshList(sender, e);

                    CS_MainList.SelectedIndex = oldCSselected;

                    CP_MainList.SelectedIndex = oldCPselected;
                    RefreshSel(sender, e);
                }
                catch (Exception exp)
                {
                    Log("something went wrong trying to delete a save: " + exp, sender);
                    //need to make this a popup to let user know what was bad
                }
            }
            else
            {
                //need to make this a popup to let user know what was bad
                Log("something went wrong trying to delete a save: File.Exists(backup) : " + File.Exists(backup).ToString());
            }
        }


        private void RefreshButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshLoa(sender, e);
            RefreshList(sender, e);
            RefreshSel(sender, e);
        }

        private void SettingsButton_Click(object sender, RoutedEventArgs e)
        {
            Window1 settingswindow = new Window1();
            if (HCMGlobal.SavedConfig.CoreFolderPath != null)
                settingswindow.ChosenCore.Text = HCMGlobal.SavedConfig.CoreFolderPath;
            else
                settingswindow.ChosenCore.Text = "No folder chosen!";
            if (HCMGlobal.SavedConfig.CheckpointFolderPath != null)
                settingswindow.ChosenCP.Text = HCMGlobal.SavedConfig.CheckpointFolderPath;
            else
                settingswindow.ChosenCP.Text = "No folder chosen!";

            if (HCMGlobal.SavedConfig.ClassicMode)
            {
                settingswindow.modeanni.IsChecked = false;
                settingswindow.modeclas.IsChecked = true;
            }
            else
            {
                settingswindow.modeanni.IsChecked = true;
                settingswindow.modeclas.IsChecked = false;
            }

            settingswindow.ShowDialog();

            if (settingswindow.ChosenCore.Text != null)
            {
                HCMGlobal.SavedConfig.CoreFolderPath = settingswindow.ChosenCore.Text;
                WriteConfig();
                RefreshSel(sender, e);
            }
            else
                HCMGlobal.SavedConfig.CoreFolderPath = "";
            if (settingswindow.ChosenCP.Text != null)
            {
                HCMGlobal.SavedConfig.CheckpointFolderPath = settingswindow.ChosenCP.Text;
                WriteConfig();
                RefreshSel(sender, e);
            }
            else
                HCMGlobal.SavedConfig.CheckpointFolderPath = "";

            if (settingswindow.modeanni.IsChecked ?? false)
            {
                HCMGlobal.SavedConfig.ClassicMode = false;
                WriteConfig();
                RefreshLoa(sender, e);
                RefreshSel(sender, e);
            }
            else
            {
                HCMGlobal.SavedConfig.ClassicMode = true;
                WriteConfig();
                RefreshLoa(sender, e);
                RefreshSel(sender, e);
            }

        }

        private void AboutButton_Click(object sender, RoutedEventArgs e)
        {
            about aboutwindow = new about();
            aboutwindow.ShowDialog();
        }


        private void RefreshLoa(object sender, RoutedEventArgs e)
        {
            //H1 CORES FIRST
            if (File.Exists(HCMGlobal.SavedConfig.CoreFolderPath + @"\core.bin") && HCMGlobal.SavedConfig.CoreFolderPath != null)
            {
                var data = GetSaveFileMetadata(HCMGlobal.SavedConfig.CoreFolderPath + @"\core.bin");
                CS_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode);

                if(data.Difficulty != Difficulty.Invalid)
                    CS_Loa_DiffName.Source = new BitmapImage(new Uri($"images/diff_{data.Difficulty}.png", UriKind.Relative));

                CS_Loa_Time.Text = TickToTimeString(data.StartTick);
                CS_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
            }
            else
            {
                CS_Loa_LevelName.Text = "N/A";
                CS_Loa_DiffName.Source = null;
                CS_Loa_Time.Text = "N/A";
                CS_Loa_LevelImage.Source = null;
            }

            //H1 CPs SECOND
            if (File.Exists(HCMGlobal.SavedConfig.CheckpointFolderPath + @"\autosave_Halo1.bin") && HCMGlobal.SavedConfig.CheckpointFolderPath != null)
            {
                var data = GetSaveFileMetadata(HCMGlobal.SavedConfig.CheckpointFolderPath + @"\autosave_Halo1.bin");
                CP_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode);

                if (data.Difficulty != Difficulty.Invalid)
                    CP_Loa_DiffName.Source = new BitmapImage(new Uri($"images/diff_{data.Difficulty}.png", UriKind.Relative));

                CP_Loa_Time.Text = TickToTimeString(data.StartTick);
                CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
            }
            else
            {
                CP_Loa_LevelName.Text = "N/A";
                CP_Loa_DiffName.Source = null;
                CP_Loa_Time.Text = "N/A";
                CP_Loa_LevelImage.Source = null;
            }

            //H2 CPs THIRD
            if (File.Exists(HCMGlobal.SavedConfig.CheckpointFolderPath + @"\autosave_Halo2.bin") && HCMGlobal.SavedConfig.CheckpointFolderPath != null)
            {
                var data = GetSaveFileMetadata(HCMGlobal.SavedConfig.CheckpointFolderPath + @"\autosave_Halo2.bin");
                H2CP_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode);

                if (data.Difficulty != Difficulty.Invalid)
                    H2CP_Loa_DiffName.Source = new BitmapImage(new Uri($"images/diff_{data.Difficulty}.png", UriKind.Relative));

                H2CP_Loa_Time.Text = TickToTimeString(data.StartTick); //might need to halve this if h2 really is 60 ticks per sec
                H2CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
            }
            else
            {
                H2CP_Loa_LevelName.Text = "N/A";
                H2CP_Loa_DiffName.Source = null;
                H2CP_Loa_Time.Text = "N/A";
                H2CP_Loa_LevelImage.Source = null;
            }
        }

        private void RefreshSel(object sender, RoutedEventArgs e)
        {
            //H1 CORES FIRST
            if (CS_MainList.SelectedItem != null)
            {
                var item = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex);
                System.Type type = item.GetType();
                string s = (string)type.GetProperty("Name").GetValue(item, null);
                var pathtotest = HCMGlobal.H1CoreSavePath + @"\" + s + @".bin";

                if (File.Exists(pathtotest))
                {
                    var data = GetSaveFileMetadata(pathtotest);
                    CS_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode);

                    if (data.Difficulty != Difficulty.Invalid)
                        CS_Sel_DiffName.Source = new BitmapImage(new Uri($"images/diff_{data.Difficulty}.png", UriKind.Relative));

                    CS_Sel_Time.Text = TickToTimeString(data.StartTick);
                    CS_Sel_FileName.Text = s;
                    CS_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                }
            }
            else
            {
                CS_Sel_LevelName.Text = "N/A";
                CS_Sel_DiffName.Source = null;
                CS_Sel_Time.Text = "N/A";
                CS_Sel_FileName.Text = "N/A";
                CS_Sel_LevelImage.Source = null;
            }
            //Console.WriteLine("CP_MainList.SelectedItem" + CP_MainList?.SelectedItem?.ToString());

            //H1 CPs SECOND
            if (CP_MainList.SelectedItem != null)
            {
                //Console.WriteLine("ahh" + CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex).ToString());
                var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
                System.Type type = item.GetType();
                string s = (string)type.GetProperty("Name").GetValue(item, null);
                var pathtotest = HCMGlobal.H1CheckpointPath + @"\" + s + @".bin";

                if (File.Exists(pathtotest))
                {
                    var data = GetSaveFileMetadata(pathtotest);
                    CP_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode);

                    if (data.Difficulty != Difficulty.Invalid)
                        CP_Sel_DiffName.Source = new BitmapImage(new Uri($"images/diff_{data.Difficulty}.png", UriKind.Relative));

                    CP_Sel_Time.Text = TickToTimeString(data.StartTick);
                    CP_Sel_FileName.Text = s;
                    CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                }
            }
            else
            {
                CP_Sel_LevelName.Text = "N/A";
                CP_Sel_DiffName.Source = null;
                CP_Sel_Time.Text = "N/A";
                CP_Sel_FileName.Text = "N/A";
                CP_Sel_LevelImage.Source = null;
            }

            //H2 CPs THIRD
            if (H2CP_MainList.SelectedItem != null)
            {
                //Console.WriteLine("ahh" + H2CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex).ToString());
                var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex);
                System.Type type = item.GetType();
                string s = (string)type.GetProperty("Name").GetValue(item, null);
                var pathtotest = HCMGlobal.H2CheckpointPath + @"\" + s + @".bin";

                if (File.Exists(pathtotest))
                {
                    var data = GetSaveFileMetadata(pathtotest);
                    H2CP_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode);

                    if (data.Difficulty != Difficulty.Invalid)
                        H2CP_Sel_DiffName.Source = new BitmapImage(new Uri($"images/diff_{data.Difficulty}.png", UriKind.Relative));

                    H2CP_Sel_Time.Text = TickToTimeString(data.StartTick); //might need to halve this if h2 really is 60 ticks per sec
                    H2CP_Sel_FileName.Text = s;
                    H2CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                }
            }
            else
            {
                H2CP_Sel_LevelName.Text = "N/A";
                H2CP_Sel_DiffName.Source = null;
                H2CP_Sel_Time.Text = "N/A";
                H2CP_Sel_FileName.Text = "N/A";
                H2CP_Sel_LevelImage.Source = null;
            }
        }

        private void RefreshList(object sender, RoutedEventArgs e)
        {
            var oldCSselected = CS_MainList.SelectedIndex;
            var oldCPselected = CP_MainList.SelectedIndex;
            var oldH2CPselected = H2CP_MainList.SelectedIndex;

            //code to populate the list
            if (Directory.Exists(HCMGlobal.SavedConfig.CoreFolderPath))
            {
                DirectoryInfo csdir = new DirectoryInfo(HCMGlobal.SavedConfig.CoreFolderPath);
            }
            if (Directory.Exists(HCMGlobal.SavedConfig.CheckpointFolderPath))
            {
                DirectoryInfo cpdir = new DirectoryInfo(HCMGlobal.SavedConfig.CheckpointFolderPath);
            }
            List<string> FilesPost = new List<string>();

            //h1 cores
            if (Directory.Exists(HCMGlobal.H1CoreSavePath)) // make sure path is valid
            {
                DirectoryInfo dir = new DirectoryInfo(HCMGlobal.H1CoreSavePath);
                FileInfo[] files = dir.GetFiles("*.bin").OrderByDescending(p => p.LastWriteTime).ToArray();
                FilesPost.Clear();

                foreach (FileInfo file in files)
                {
                    while (FileHasSameTime(files, file))
                    {
                        file.LastWriteTime = file.LastWriteTime.AddSeconds(1);
                    }
                    FilesPost.Add(file.ToString());
                }

                if (FilesPost.ElementAtOrDefault(0) != null && FilesPost[0].ToString() != null)
                {
                    CS_MainList.Items.Clear();
                    CS_MainList_Label.Content = "";
                    foreach (string File in FilesPost)
                    {
                        var data = GetSaveFileMetadata(HCMGlobal.H1CoreSavePath + "/" + File);
                        string _Lvl = data.LevelCode;
                        string _Diff = data.Difficulty.ToString();

                        if(data.Difficulty != Difficulty.Invalid)
                            _Diff = $"images/diff_{(int)data.Difficulty}.png";

                        string _Time = TickToTimeString(data.StartTick);
                        string _Name = File.Substring(0, File.Length - 4);
                        CS_MainList.Items.Add(new { Lvl = _Lvl, Diff = _Diff, Time = _Time, Name = _Name });
                    }

                    CS_MainList.SelectedIndex = oldCSselected;
                    GridView gv = CS_MainList.View as GridView;
                    UpdateColumnWidths(gv);
                }
                else
                {
                    CS_MainList.Items.Clear();
                    CS_MainList_Label.Content = "No backup saves in local folder.";
                }
            }
            else
            {
                CS_MainList.Items.Clear();
                CS_MainList_Label.Content = "Core folder path is invalid, check Settings.";
            }

            //h1 checkpoints second
            if (Directory.Exists(HCMGlobal.H1CheckpointPath)) // make sure path is valid
            {
                DirectoryInfo dir = new DirectoryInfo(HCMGlobal.H1CheckpointPath);
                FileInfo[] files = dir.GetFiles("*.bin").OrderByDescending(p => p.LastWriteTime).ToArray();
                FilesPost.Clear();
                foreach (FileInfo file in files)
                {
                    while (FileHasSameTime(files, file))
                    {
                        file.LastWriteTime = file.LastWriteTime.AddSeconds(1);
                    }
                    FilesPost.Add(file.ToString());
                }

                if (FilesPost.ElementAtOrDefault(0) != null && FilesPost[0].ToString() != null)
                {
                    CP_MainList.Items.Clear();
                    CP_MainList_Label.Content = "";
                    foreach (string File in FilesPost)
                    {
                        var data = GetSaveFileMetadata(HCMGlobal.H1CheckpointPath + "/" + File);
                        string _Lvl = data.LevelCode;
                        string _Diff = data.Difficulty.ToString();

                        if (data.Difficulty != Difficulty.Invalid)
                            _Diff = $"images/diff_{(int)data.Difficulty}.png";

                        string _Time = TickToTimeString(data.StartTick);
                        string _Name = File.Substring(0, File.Length - 4);
                        CP_MainList.Items.Add(new { Lvl = _Lvl, Diff = _Diff, Time = _Time, Name = _Name });

                    }
                    CP_MainList.SelectedIndex = oldCPselected;
                    GridView gv = CP_MainList.View as GridView;
                    UpdateColumnWidths(gv);
                }
                else
                {
                    CP_MainList.Items.Clear();
                    CP_MainList_Label.Content = "No backup saves in local folder.";
                }
            }
            else
            {
                CP_MainList.Items.Clear();
                CP_MainList_Label.Content = "Checkpoint folder path is invalid, check Settings.";
            }

            //h2 checkpoints THIRD
            if (Directory.Exists(HCMGlobal.H2CheckpointPath)) // make sure path is valid
            {
                DirectoryInfo dir = new DirectoryInfo(HCMGlobal.H2CheckpointPath);
                FileInfo[] files = dir.GetFiles("*.bin").OrderByDescending(p => p.LastWriteTime).ToArray();
                FilesPost.Clear();
                foreach (FileInfo file in files)
                {
                    while (FileHasSameTime(files, file))
                    {
                        file.LastWriteTime = file.LastWriteTime.AddSeconds(1);
                    }
                    FilesPost.Add(file.ToString());
                }

                if (FilesPost.ElementAtOrDefault(0) != null && FilesPost[0].ToString() != null)
                {
                    H2CP_MainList.Items.Clear();
                    H2CP_MainList_Label.Content = "";
                    foreach (string File in FilesPost)
                    {
                        var data = GetSaveFileMetadata(HCMGlobal.H2CheckpointPath + "/" + File);
                        string _Lvl = data.LevelCode;
                        string _Diff = data.Difficulty.ToString();

                        if (data.Difficulty != Difficulty.Invalid)
                            _Diff = $"images/diff_{(int)data.Difficulty}.png";

                        string _Time = TickToTimeString(data.StartTick);
                        string _Name = File.Substring(0, File.Length - 4);
                        H2CP_MainList.Items.Add(new { Lvl = _Lvl, Diff = _Diff, Time = _Time, Name = _Name });

                    }

                    H2CP_MainList.SelectedIndex = oldH2CPselected;
                    GridView gv = H2CP_MainList.View as GridView;
                    UpdateColumnWidths(gv);
                }
                else
                {
                    H2CP_MainList.Items.Clear();
                    H2CP_MainList_Label.Content = "No backup saves in local folder.";
                }
            }
            else
            {
                H2CP_MainList.Items.Clear();
                H2CP_MainList_Label.Content = "Checkpoint folder path is invalid, check Settings.";
            }
        }

        private class HaloSaveFileMetadata
        {
            public HaloGame HaloGame = HaloGame.Halo1;
            public string LevelCode = "xxx";
            public uint StartTick = 0;
            public Difficulty Difficulty = Difficulty.Easy;
        }

        private HaloSaveFileMetadata GetSaveFileMetadata(string saveFilePath)
        {
            //btw I suspect the easiest way to deal with h2 is just to CHECK inside the file here to see if it's from h1 or h2, so I don't have to worry about GetInfo being passed either file type

            HaloSaveFileMetadata metadata = new HaloSaveFileMetadata();

            if (File.Exists(saveFilePath))
            {
                try
                {
                    using (FileStream readStream = new FileStream(saveFilePath, FileMode.Open))
                    {
                        BinaryReader readBinary = new BinaryReader(readStream);

                        //get levelname
                        readBinary.BaseStream.Seek(11, SeekOrigin.Begin);
                        metadata.LevelCode = new string(readBinary.ReadChars(3));

                        //get time
                        readBinary.BaseStream.Seek(756, SeekOrigin.Begin);
                        metadata.StartTick = readBinary.ReadUInt32();

                        //get difficulty
                        readBinary.BaseStream.Seek(294, SeekOrigin.Begin);
                        metadata.Difficulty = (Difficulty)readBinary.ReadByte();
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString());
                }
            }

            return metadata;
        }

        public static void UpdateColumnWidths(GridView gridView)
        {
            // For each column...
            foreach (var column in gridView.Columns)
            {
                // If this is an "auto width" column...
                if (double.IsNaN(column.Width))
                {
                    // Set its Width back to NaN to auto-size again
                    column.Width = 0;
                    column.Width = double.NaN;
                }
            }
        }

        public bool FileHasSameTime(FileInfo[] FilesPost, FileInfo currentfile)
        {
            foreach (FileInfo File in FilesPost)
            {
                if (currentfile.Name != File.Name)
                {
                    if (File.LastWriteTime == currentfile.LastWriteTime)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        public static void WriteConfig()
        {
            string json = JsonConvert.SerializeObject(HCMGlobal.SavedConfig, Formatting.Indented);
            File.WriteAllText(HCMGlobal.ConfigPath, json);
        }

        public string TickToTimeString(uint ticks)
        {
            int seconds = (int)ticks / 30;
            TimeSpan ts = new TimeSpan(seconds / (60*60), seconds / (60), seconds % 60);
            return ts.ToString(@"mm\:ss");
        }

        readonly Dictionary<string, string> LevelCodeToName = new Dictionary<string, string>()
        {
            // Halo 1
            { "a10", "Pillar of Autumn" },
            { "a30", "Halo" },
            { "a50", "Truth and Rec" },
            { "b30", "Silent Cartographer" },
            { "b40", "AotCR" },
            { "c10", "343 Guilty Spark" },
            { "c20", "The Library" },
            { "c40", "Two Betrayals" },
            { "d20", "Keyes" },
            { "d40", "The Maw" },

            // Halo 2
            { "00a", "The Heretic" },
            { "01a", "The Armory" },
            { "01b", "Cairo Station" },
            { "03a", "Outskirts" },
            { "03b", "Metropolis" },
            { "04a", "The Arbiter" },
            { "04b", "The Oracle" },
            { "05a", "Delta Halo" },
            { "05b", "Regret" },
            { "06a", "Sacred Icon" },
            { "06b", "Quarantine Zone" },
            { "07a", "Gravemind" },
            { "07b", "Uprising" },
            { "08a", "High Charity" },
            { "08b", "The Great Journey" },
        };

        public string LevelCodeToFullName(string code)
        {
            string Name;
            if (LevelCodeToName.TryGetValue(code, out Name))
            {
                return Name;
            }

            return code;
        }

        public static void Log(string text, object sender)
        {
            System.Type type = sender.GetType();
            string s = (string)type.GetProperty("Name").GetValue(sender, null);
            List<string> addthis = new List<string> { DateTime.Now.ToString(), text, s };
            File.AppendAllLines(HCMGlobal.LogPath, addthis);
        }

        public static void Log(string text)
        {
            List<string> addthis = new List<string> { DateTime.Now.ToString(), text };
            try { File.AppendAllLines(HCMGlobal.LogPath, addthis); }
            catch
            {
                //what the fuck do we even do now?!
            }
        }

    }
}
