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


namespace WpfApp3
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
 
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            CS_MainList.SelectionChanged += List_SelectionChanged;
            CP_MainList.SelectionChanged += List_SelectionChanged;
            H2CP_MainList.SelectionChanged += List_SelectionChanged;

            if (File.Exists(globals.localdir + @"\config.txt"))
            {
                try
                {
                    globals.corefolderpath = File.ReadLines(globals.localdir + @"\config.txt").First();
                    globals.cpfolderpath = File.ReadLines(globals.localdir + @"\config.txt").Skip(1).Take(1).First();
                    globals.classicmode = bool.Parse(File.ReadLines(globals.localdir + @"\config.txt").Skip(2).Take(1).First());
                }
                catch
                {
                    
                }
            }
            else
            {
                try
                {
                    File.Create(globals.localdir + @"\config.txt").Dispose();
                }
                catch (Exception exp)
                {
                    MessageBox.Show("Couldn't create local config file. Show this to Burnt: " + exp.ToString());
                }

            }

            if (!File.Exists(globals.localdir + @"\log.txt"))
            {
                try
                {
                    File.Create(globals.localdir + @"\log.txt").Dispose();
                }
                catch (Exception exp)
                {
                    MessageBox.Show("Couldn't create local log file. Show this to Burnt: " + exp.ToString());
                }
            }
            

            if (!Directory.Exists(globals.localdir + @"\saves"))
            {
                try
                {
                    Directory.CreateDirectory(globals.localdir + @"\saves");
                }
                catch (Exception exp)
                {
                    Log(@"something went wrong trying to make a \saves folder: " + exp);
                }
            }
            if (!Directory.Exists(globals.localdir + @"\saves\h1cs"))
            {
                try
                {
                    Directory.CreateDirectory(globals.localdir + @"\saves\h1cs");
                }
                catch (Exception exp)
                {
                    Log(@"something went wrong trying to make a \saves\h1cs folder: " + exp);
                }
            }
            if (!Directory.Exists(globals.localdir + @"\saves\h1cp"))
            {
                try
                {
                    Directory.CreateDirectory(globals.localdir + @"\saves\h1cp");
                }
                catch (Exception exp)
                {
                    Log(@"something went wrong trying to make a \saves\h1cp folder: " + exp);
                }
            }
            if (!Directory.Exists(globals.localdir + @"\saves\h2cp"))
            {
                try
                {
                    Directory.CreateDirectory(globals.localdir + @"\saves\h2cp");
                }
                catch (Exception exp)
                {
                    Log(@"something went wrong trying to make a \saves\h2cp folder: " + exp);
                }
            }

            CS_MainList.Items.Clear();
            CP_MainList.Items.Clear();
            H2CP_MainList.Items.Clear();
            RefreshButton.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));

            if (DateTime.Now.Day == 25 && DateTime.Now.Month == 12)
            {
                globals.padowomode = true;
                H2CP_Sel_ConvertButton.Content = "Convert to PADAWO";
            }
        }


        void List_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            RefreshSel(sender, e);
        }


        public static class globals
        {
            public static string localdir = System.IO.Path.GetDirectoryName(System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName);


            public static string corefolderpath = null;
            public static string cpfolderpath = null;
            public static string h1cs = globals.localdir + @"\saves\h1cs";
            public static string h1cp = globals.localdir + @"\saves\h1cp";
            public static string h2cp = globals.localdir + @"\saves\h2cp";
            public static bool classicmode = true;
            public static bool padowomode = false;
            
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
                        movethis = globals.h1cs + @"\" + s2 + @".bin";

                        var item2 = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex - 1);
                        System.Type type3 = item2.GetType();
                        string s3 = (string)type3.GetProperty("Name").GetValue(item2, null);
                        abovefile = globals.h1cs + @"\" + s3 + @".bin";
                    }
                    break;

                case "CP_Sel_MoveUpButton":
                    if (CP_MainList.SelectedItem != null && CP_MainList.SelectedIndex != 0)
                    {
                        var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        movethis = globals.h1cp + @"\" + s2 + @".bin";

                        var item2 = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex - 1);
                        System.Type type3 = item2.GetType();
                        string s3 = (string)type3.GetProperty("Name").GetValue(item2, null);
                        abovefile = globals.h1cp + @"\" + s3 + @".bin";
                    }
                    break;

                case "H2CP_Sel_MoveUpButton":
                    if (H2CP_MainList.SelectedItem != null && H2CP_MainList.SelectedIndex != 0)
                    {
                        var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        movethis = globals.h2cp + @"\" + s2 + @".bin";

                        var item2 = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex - 1);
                        System.Type type3 = item2.GetType();
                        string s3 = (string)type3.GetProperty("Name").GetValue(item2, null);
                        abovefile = globals.h2cp + @"\" + s3 + @".bin";
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
                        movethis = globals.h1cs + @"\" + s2 + @".bin";

                        var item2 = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex + 1);
                        System.Type type3 = item2.GetType();
                        string s3 = (string)type3.GetProperty("Name").GetValue(item2, null);
                        belowfile = globals.h1cs + @"\" + s3 + @".bin";
                    }
                    break;

                case "CP_Sel_MoveDownButton":
                    if (CP_MainList.SelectedItem != null && CP_MainList.SelectedIndex != CP_MainList.Items.Count -1)
                    {
                        var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        movethis = globals.h1cp + @"\" + s2 + @".bin";

                        var item2 = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex + 1);
                        System.Type type3 = item2.GetType();
                        string s3 = (string)type3.GetProperty("Name").GetValue(item2, null);
                        belowfile = globals.h1cp + @"\" + s3 + @".bin";
                    }
                    break;

                case "H2CP_Sel_MoveDownButton":
                    if (H2CP_MainList.SelectedItem != null && H2CP_MainList.SelectedIndex != H2CP_MainList.Items.Count -1)
                    {
                        var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        movethis = globals.h2cp + @"\" + s2 + @".bin";

                        var item2 = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex + 1);
                        System.Type type3 = item2.GetType();
                        string s3 = (string)type3.GetProperty("Name").GetValue(item2, null);
                        belowfile = globals.h2cp + @"\" + s3 + @".bin";
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
                    backuploc = globals.h1cs;
                    if (globals.corefolderpath != null)
                    pathtotest = globals.corefolderpath + @"\core.bin";
                    break;

                case "CP_Loa_SaveButton":
                    backuploc = globals.h1cp;
                    if (globals.cpfolderpath != null)
                        pathtotest = globals.cpfolderpath + @"\autosave_Halo1.bin";
                    break;

                case "H2CP_Loa_SaveButton":
                    backuploc = globals.h2cp;
                    if (globals.cpfolderpath != null)
                        pathtotest = globals.cpfolderpath + @"\autosave_Halo2.bin";
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
                        backuploc = globals.h1cs + @"\" + s2 + @".bin";
                        pathtotest = globals.corefolderpath + @"\core.bin";
                    }
                    break;

                case "CP_Sel_LoadButton":
                    if (CP_MainList.SelectedItem != null)
                    {
                        var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backuploc = globals.h1cp + @"\" + s2 + @".bin";
                        pathtotest = globals.cpfolderpath + @"\autosave_Halo1.bin";
                    }
                    break;

                case "H2CP_Sel_LoadButton":
                    if (H2CP_MainList.SelectedItem != null)
                    {
                        var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backuploc = globals.h2cp + @"\" + s2 + @".bin";
                        pathtotest = globals.cpfolderpath + @"\autosave_Halo2.bin";
                    }
                    break;

                default:
                    break;

            }
           
            try
            {
                testme = System.IO.Path.GetDirectoryName(pathtotest);
            }
            catch {}
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
                        backup = globals.h1cs + @"\" + s2 + @".bin";
                        backuploc = globals.h1cs;
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
                        backup = globals.h1cp + @"\" + s2 + @".bin";
                        backuploc = globals.h1cp;
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
                        backup = globals.h2cp + @"\" + s2 + @".bin";
                        backuploc = globals.h2cp;
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
                        convertfrom = globals.h1cs + @"\" + s2 + @".bin";
                        convertto = globals.h1cp + @"\" + s2 + @".bin";
                        converttoloc = globals.h1cp;
                    }
                    break;

                case "CP_Sel_ConvertButton":
                    if (CP_MainList.SelectedItem != null)
                    {
                        var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        convertfrom = globals.h1cp + @"\" + s2 + @".bin";
                        convertto = globals.h1cs + @"\" + s2 + @".bin";
                        converttoloc = globals.h1cs;
                    }
                    break;
                
                case "H2CP_Sel_ConvertButton":
                    string link = "";
                    if (globals.padowomode)
                        link = "https://www.youtube.com/watch?v=dQ_d_VKrFgM";
                    else
                        link = "https://www.youtube.com/watch?v=5u4tQlVRKD8";
                    
                    
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
                        backup = globals.h1cs + @"\" + s2 + @".bin";
                    }
                    break;

                case "CP_Sel_DeleteButton":
                    if (CP_MainList.SelectedItem != null)
                    {
                        var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backup = globals.h1cp + @"\" + s2 + @".bin";
                    }
                    break;

                case "H2CP_Sel_DeleteButton":
                    if (H2CP_MainList.SelectedItem != null)
                    {
                        var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex);
                        System.Type type2 = item.GetType();
                        string s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backup = globals.h2cp + @"\" + s2 + @".bin";
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
            if (globals.corefolderpath != null)
                settingswindow.ChosenCore.Text = globals.corefolderpath;
            else
                settingswindow.ChosenCore.Text = "No folder chosen!";
            if (globals.cpfolderpath != null)
                settingswindow.ChosenCP.Text = globals.cpfolderpath;
            else
                settingswindow.ChosenCP.Text = "No folder chosen!";

            if (globals.classicmode)
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
                globals.corefolderpath = settingswindow.ChosenCore.Text;
                writeConfig();
                RefreshSel(sender, e);
            }
            else
                globals.corefolderpath = "";
            if (settingswindow.ChosenCP.Text != null)
            {
                globals.cpfolderpath = settingswindow.ChosenCP.Text;
                writeConfig();
                RefreshSel(sender, e);
            }
            else
                globals.cpfolderpath = "";

            if (settingswindow.modeanni.IsChecked ?? false)
            {
                globals.classicmode = false;
                writeConfig();
                RefreshLoa(sender, e);
                RefreshSel(sender, e);
            }
            else
            {

                globals.classicmode = true;
                writeConfig();
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
            if (File.Exists(globals.corefolderpath + @"\core.bin") && globals.corefolderpath != null)
            {
                var data = GetInfo(globals.corefolderpath + @"\core.bin");
                CS_Loa_LevelName.Text = codeToName(data.Item1.ToString());

                if (TestRange(Int32.Parse(data.Item2.ToString()), 0, 4))
                    CS_Loa_DiffName.Source = new BitmapImage(new Uri("diff_" + data.Item2.ToString() + ".png", UriKind.Relative));
                else
                    Log("somehow you had an invalid CS Loa difficulty value: " + data.Item2.ToString(), sender);

                CS_Loa_Time.Text = ticksToTime(data.Item3.ToString());
                if (globals.classicmode)
                {
                    try { CS_Loa_LevelImage.Source = new BitmapImage(new Uri(data.Item1.ToString() + "_clas.png", UriKind.Relative)); }
                    catch { }
                }
                else
                {
                    try { CS_Loa_LevelImage.Source = new BitmapImage(new Uri(data.Item1.ToString() + "_anni.png", UriKind.Relative)); }
                    catch { }
                }
            }
            else
            {
                CS_Loa_LevelName.Text = "N/A";
                CS_Loa_DiffName.Source = null;
                CS_Loa_Time.Text = "N/A";
                CS_Loa_LevelImage.Source = null;

            }

            //H1 CPs SECOND
            if (File.Exists(globals.cpfolderpath + @"\autosave_Halo1.bin") && globals.cpfolderpath != null)
            {
                var data = GetInfo(globals.cpfolderpath + @"\autosave_Halo1.bin");
                CP_Loa_LevelName.Text = codeToName(data.Item1.ToString());

                if (TestRange(Int32.Parse(data.Item2.ToString()), 0, 4))
                    CP_Loa_DiffName.Source = new BitmapImage(new Uri("diff_" + data.Item2.ToString() + ".png", UriKind.Relative));
                else
                    Log("somehow you had an invalid CP Loa difficulty value: " + data.Item2.ToString(), sender);

                CP_Loa_Time.Text = ticksToTime(data.Item3.ToString());
                if (globals.classicmode)
                {
                    try { CP_Loa_LevelImage.Source = new BitmapImage(new Uri(data.Item1.ToString() + "_clas.png", UriKind.Relative)); }
                    catch { }
                }
                else
                {
                    try { CP_Loa_LevelImage.Source = new BitmapImage(new Uri(data.Item1.ToString() + "_anni.png", UriKind.Relative)); }
                    catch { }
                }
            }
            else
            {
                CP_Loa_LevelName.Text = "N/A";
                CP_Loa_DiffName.Source = null;
                CP_Loa_Time.Text = "N/A";
                CP_Loa_LevelImage.Source = null;

            }

            //H2 CPs THIRD
            if (File.Exists(globals.cpfolderpath + @"\autosave_Halo2.bin") && globals.cpfolderpath != null)
            {
                var data = GetInfo(globals.cpfolderpath + @"\autosave_Halo2.bin");
                H2CP_Loa_LevelName.Text = codeToName(data.Item1.ToString());

                if (TestRange(Int32.Parse(data.Item2.ToString()), 0, 4))
                    H2CP_Loa_DiffName.Source = new BitmapImage(new Uri("diff_" + data.Item2.ToString() + ".png", UriKind.Relative));
                else
                    Log("somehow you had an invalid H2CP Loa difficulty value: " + data.Item2.ToString(), sender);

                H2CP_Loa_Time.Text = ticksToTime(data.Item3.ToString()); //might need to halve this if h2 really is 60 ticks per sec
                if (globals.classicmode)
                {
                    try { H2CP_Loa_LevelImage.Source = new BitmapImage(new Uri(data.Item1.ToString() + "_clas.png", UriKind.Relative)); }
                    catch { }
                }
                else
                {
                    try { H2CP_Loa_LevelImage.Source = new BitmapImage(new Uri(data.Item1.ToString() + "_anni.png", UriKind.Relative)); }
                    catch { }
                }
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
                
                var pathtotest = globals.h1cs + @"\" + s + @".bin";
                
                if (File.Exists(pathtotest))
                {
                    var data = GetInfo(pathtotest);
                    CS_Sel_LevelName.Text = codeToName(data.Item1.ToString());

                    if (TestRange(Int32.Parse(data.Item2.ToString()), 0, 4))
                        CS_Sel_DiffName.Source = new BitmapImage(new Uri("diff_" + data.Item2.ToString() + ".png", UriKind.Relative));
                    else
                        Log("somehow you had an invalid CS Sel difficulty value: " + data.Item2.ToString(), sender);

                    CS_Sel_Time.Text = ticksToTime(data.Item3.ToString());
                    CS_Sel_FileName.Text = s;
                    if (globals.classicmode)
                    {
                        try { CS_Sel_LevelImage.Source = new BitmapImage(new Uri(data.Item1.ToString() + "_clas.png", UriKind.Relative)); }
                        catch { }
                    }
                    else
                    {
                        try { CS_Sel_LevelImage.Source = new BitmapImage(new Uri(data.Item1.ToString() + "_anni.png", UriKind.Relative)); }
                        catch { }
                    }

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

                var pathtotest = globals.h1cp + @"\" + s + @".bin";

                if (File.Exists(pathtotest))
                {
                    var data = GetInfo(pathtotest);
                    CP_Sel_LevelName.Text = codeToName(data.Item1.ToString());

                    if (TestRange(Int32.Parse(data.Item2.ToString()), 0, 4))
                        CP_Sel_DiffName.Source = new BitmapImage(new Uri("diff_" + data.Item2.ToString() + ".png", UriKind.Relative));
                    else
                        Log("somehow you had an invalid CP Sel difficulty value: " + data.Item2.ToString(), sender);


                    CP_Sel_Time.Text = ticksToTime(data.Item3.ToString());
                    CP_Sel_FileName.Text = s;
                    if (globals.classicmode)
                    {
                        try { CP_Sel_LevelImage.Source = new BitmapImage(new Uri(data.Item1.ToString() + "_clas.png", UriKind.Relative)); }
                        catch { }
                    }
                    else
                    {
                        try { CP_Sel_LevelImage.Source = new BitmapImage(new Uri(data.Item1.ToString() + "_anni.png", UriKind.Relative)); }
                        catch { }
                    }
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

                var pathtotest = globals.h2cp + @"\" + s + @".bin";

                if (File.Exists(pathtotest))
                {
                    var data = GetInfo(pathtotest);
                    H2CP_Sel_LevelName.Text = codeToName(data.Item1.ToString());

                    if (TestRange(Int32.Parse(data.Item2.ToString()), 0, 4))
                        H2CP_Sel_DiffName.Source = new BitmapImage(new Uri("diff_" + data.Item2.ToString() + ".png", UriKind.Relative));
                    else
                        Log("somehow you had an invalid CP Sel difficulty value: " + data.Item2.ToString(), sender);


                    H2CP_Sel_Time.Text = ticksToTime(data.Item3.ToString()); //might need to halve this if h2 really is 60 ticks per sec
                    H2CP_Sel_FileName.Text = s;
                    if (globals.classicmode)
                    {
                        try { H2CP_Sel_LevelImage.Source = new BitmapImage(new Uri(data.Item1.ToString() + "_clas.png", UriKind.Relative)); }
                        catch { }
                    }
                    else
                    {
                        try { H2CP_Sel_LevelImage.Source = new BitmapImage(new Uri(data.Item1.ToString() + "_anni.png", UriKind.Relative)); }
                        catch { }
                    }
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
            if (Directory.Exists(globals.corefolderpath))
            {
                DirectoryInfo csdir = new DirectoryInfo(globals.corefolderpath);
            }
            if (Directory.Exists(globals.cpfolderpath))
            {
                DirectoryInfo cpdir = new DirectoryInfo(globals.cpfolderpath);
            }
            List<string> FilesPost = new List<string>();


            //h1 cores
            if (Directory.Exists(globals.h1cs)) // make sure path is valid
            {
                DirectoryInfo dir = new DirectoryInfo(globals.h1cs);
                FileInfo[] files = dir.GetFiles("*.bin").OrderByDescending(p => p.LastWriteTime).ToArray();
                FilesPost.Clear();


                foreach (FileInfo file in files)
                {

                    while (hasSameTime(files, file))
                    {
                        //do a thing to decrement hte time
                        // Console.WriteLine("before: " + file.LastWriteTime.ToString());
                        file.LastWriteTime = file.LastWriteTime.AddSeconds(1);
                        //Console.WriteLine("after: " + file.LastWriteTime.ToString());
                        //Console.WriteLine("decremented file");
                    }

                    FilesPost.Add(file.ToString());


                }

                if (FilesPost.ElementAtOrDefault(0) != null && FilesPost[0].ToString() != null)
                { 

                    CS_MainList.Items.Clear();
                    CS_MainList_Label.Content = "";
                    foreach (string File in FilesPost)
                    {
                        var data = GetInfo(globals.h1cs + "/" + File);
                        string _Lvl = data.Item1.ToString();
                        string _Diff = data.Item2.ToString();

                        if (TestRange(Int32.Parse(data.Item2.ToString()), 0, 4))
                            _Diff = "diff_" + data.Item2.ToString() + ".png";
                        else
                            Log("somehow you had an invalid CS List difficulty value: " + data.Item2.ToString(), sender);
                        

                        string _Time = ticksToTime(data.Item3.ToString());
                        string _Name = File.Substring(0, File.Length - 4);
                        CS_MainList.Items.Add(new { Lvl = _Lvl, Diff = _Diff, Time = _Time, Name = _Name });

                    }
                    CS_MainList.SelectedIndex = oldCSselected;
                    GridView gv = CS_MainList.View as GridView;
                    UpdateColumnWidths(gv);


                    // return;
                }
                else
                {
                    CS_MainList.Items.Clear();
                    CS_MainList_Label.Content = "No backup saves in local folder.";
                    //CS_MainList.Items.Add("No saves in folder :(");
                    //return;
                }
            
            }
            else
            {
                CS_MainList.Items.Clear();
                CS_MainList_Label.Content = "Core folder path is invalid, check Settings.";
                //CS_MainList.Items.Add("Core folder path is invalid :(");
                //return;
            }

            //h1 checkpoints second
            if (Directory.Exists(globals.h1cp)) // make sure path is valid
            {
                DirectoryInfo dir = new DirectoryInfo(globals.h1cp);
                FileInfo[] files = dir.GetFiles("*.bin").OrderByDescending(p => p.LastWriteTime).ToArray();
                FilesPost.Clear();
                foreach (FileInfo file in files)
                {
                    while (hasSameTime(files, file))
                    {
                        //do a thing to decrement hte time
                        // Console.WriteLine("before: " + file.LastWriteTime.ToString());
                        file.LastWriteTime = file.LastWriteTime.AddSeconds(1);
                        //Console.WriteLine("after: " + file.LastWriteTime.ToString());
                        //Console.WriteLine("decremented file");
                    }
                    FilesPost.Add(file.ToString());
                }


                if (FilesPost.ElementAtOrDefault(0) != null && FilesPost[0].ToString() != null)
                {
                    CP_MainList.Items.Clear();
                    CP_MainList_Label.Content = "";
                    foreach (string File in FilesPost)
                    {
                        var data = GetInfo(globals.h1cp + "/" + File);
                        string _Lvl = data.Item1.ToString();
                        string _Diff = data.Item2.ToString();

                        if (TestRange(Int32.Parse(data.Item2.ToString()), 0, 4))
                            _Diff = "diff_" + data.Item2.ToString() + ".png";
                        else
                            Log("somehow you had an invalid CP List difficulty value: " + data.Item2.ToString(), sender);

                        string _Time = ticksToTime(data.Item3.ToString());
                        string _Name = File.Substring(0, File.Length - 4);
                        CP_MainList.Items.Add(new { Lvl = _Lvl, Diff = _Diff, Time = _Time, Name = _Name });

                    }
                    CP_MainList.SelectedIndex = oldCPselected;
                    GridView gv = CP_MainList.View as GridView;
                    UpdateColumnWidths(gv);


                    //return;
                }
                else
                {
                    CP_MainList.Items.Clear();
                    CP_MainList_Label.Content = "No backup saves in local folder.";
                    //CP_MainList.Items.Add("No saves in folder :(");
                    //return;
                }
            }
            else
            {
                CP_MainList.Items.Clear();
                CP_MainList_Label.Content = "Checkpoint folder path is invalid, check Settings.";
                //CP_MainList.Items.Add("Checkpoint folder path is invalid :(");
                //return;
            }


            //h2 checkpoints THIRD
            if (Directory.Exists(globals.h2cp)) // make sure path is valid
            {
                DirectoryInfo dir = new DirectoryInfo(globals.h2cp);
                FileInfo[] files = dir.GetFiles("*.bin").OrderByDescending(p => p.LastWriteTime).ToArray();
                FilesPost.Clear();
                foreach (FileInfo file in files)
                {
                    while (hasSameTime(files, file))
                    {
                        //do a thing to decrement hte time
                        // Console.WriteLine("before: " + file.LastWriteTime.ToString());
                        file.LastWriteTime = file.LastWriteTime.AddSeconds(1);
                        //Console.WriteLine("after: " + file.LastWriteTime.ToString());
                        //Console.WriteLine("decremented file");
                    }
                    FilesPost.Add(file.ToString());
                }


                if (FilesPost.ElementAtOrDefault(0) != null && FilesPost[0].ToString() != null)
                {
                    H2CP_MainList.Items.Clear();
                    H2CP_MainList_Label.Content = "";
                    foreach (string File in FilesPost)
                    {
                        var data = GetInfo(globals.h2cp + "/" + File);
                        string _Lvl = data.Item1.ToString();
                        string _Diff = data.Item2.ToString();

                        if (TestRange(Int32.Parse(data.Item2.ToString()), 0, 4))
                            _Diff = "diff_" + data.Item2.ToString() + ".png";
                        else
                            Log("somehow you had an invalid H2CP List difficulty value: " + data.Item2.ToString(), sender);

                        string _Time = ticksToTime(data.Item3.ToString());
                        string _Name = File.Substring(0, File.Length - 4);
                        H2CP_MainList.Items.Add(new { Lvl = _Lvl, Diff = _Diff, Time = _Time, Name = _Name });

                    }
                    H2CP_MainList.SelectedIndex = oldH2CPselected;
                    GridView gv = H2CP_MainList.View as GridView;
                    UpdateColumnWidths(gv);


                    //return;
                }
                else
                {
                    H2CP_MainList.Items.Clear();
                    H2CP_MainList_Label.Content = "No backup saves in local folder.";
                    //CP_MainList.Items.Add("No saves in folder :(");
                    //return;
                }
            }
            else
            {
                H2CP_MainList.Items.Clear();
                H2CP_MainList_Label.Content = "Checkpoint folder path is invalid, check Settings.";
                //CP_MainList.Items.Add("Checkpoint folder path is invalid :(");
                //return;
            }
        }




        private (string, byte, UInt32) GetInfo(string thispath)
        {
            //btw I suspect the easiest way to deal with h2 is just to CHECK inside the file here to see if it's from h1 or h2, so I don't have to worry about GetInfo being passed either file type
            String[] line = { "a", "b", "c" };
            if (File.Exists(thispath))
            {
                FileStream readStream;
                try
                {
                    //get levelname
                    readStream = new FileStream(thispath, FileMode.Open);
                    BinaryReader readBinary = new BinaryReader(readStream);
                    readBinary.BaseStream.Seek(11, SeekOrigin.Begin);
                    string levelname = new string(readBinary.ReadChars(3));
                    //Console.WriteLine("levelname: " + levelname);

                    //get time
                    readBinary.BaseStream.Seek(756, SeekOrigin.Begin);
                    UInt32 timeinticks = readBinary.ReadUInt32();
                    //Console.WriteLine("time: " + timeinticks);

                    //get difficulty
                    readBinary.BaseStream.Seek(294, SeekOrigin.Begin);
                    Byte difficulty = readBinary.ReadByte();
                    //Console.WriteLine("difficulty: " + difficulty);

                    readStream.Close();
                    return (levelname, difficulty, timeinticks);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString());
                    
                    return ("0", 0, 0);
                }
            }
            return ("0", 0, 0);
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
        public bool hasSameTime(FileInfo[] FilesPost, FileInfo currentfile)
        {
            // For each column...
            foreach (FileInfo File in FilesPost)
            {
                //if lastwrite of currentfile = lastwritetime of file
                //Console.WriteLine("currentfilename: " + currentfile.Name.ToString());
                //Console.WriteLine("filename: " + File.Name.ToString());
                if (currentfile.Name != File.Name)
                {
                    if (File.LastWriteTime == currentfile.LastWriteTime)
                    {
                        return true;
                    }
                }
            }
            //Console.WriteLine("returned false on hassametime");
            return false;
        }

        public static void writeConfig()
        {
            if (File.Exists(globals.localdir + @"\config.txt"))
            {
                //write to first line of file
                // Read the old file.
                string[] lines = { globals.corefolderpath, globals.cpfolderpath, globals.classicmode.ToString() };
                string docPath = globals.localdir;
                using (StreamWriter outputFile = new StreamWriter(System.IO.Path.Combine(docPath, "config.txt")))
                {
                    foreach (string line in lines)
                        outputFile.WriteLine(line);
                }
            }
        }

        public string ticksToTime (string ticks)
        {
            string Time = "";
            int number = Int32.Parse(ticks);
            number = number / 30;
            Time = string.Format("{0:00}:{1:00}", number / 60, number % 60);
            return Time;
        }

        public string codeToName (string code)
        {
            string Name = code;

            switch (code)
            {
                case "a10":
                    Name = "Pillar of Autumn";
                    break;

                case "a30":
                    Name = "Halo";
                    break;

                case "a50":
                    Name = "T & R";
                    break;

                case "b30":
                    Name = "Silent Cartographer";
                    break;

                case "b40":
                    Name = "AotCR";
                    break;

                case "c10":
                    Name = "343 Guilty Spark";
                    break;

                case "c20":
                    Name = "The Library";
                    break;

                case "c40":
                    Name = "Two Betrayals";
                    break;

                case "d20":
                    Name = "Keyes";
                    break;

                case "d40":
                    Name = "The Maw";
                    break;

                case "00a":
                    Name = "The Heretic";
                    break;

                case "01a":
                    Name = "The Armory";
                    break;

                case "01b":
                    Name = "Cairo Station";
                    break;

                case "03a":
                    Name = "Outskirts";
                    break;

                case "03b":
                    Name = "Metropolis";
                    break;

                case "04a":
                    Name = "The Arbiter";
                    break;

                case "04b":
                    Name = "The Oracle";
                    break;

                case "05a":
                    Name = "Delta Halo";
                    break;

                case "05b":
                    Name = "Regret";
                    break;

                case "06a":
                    Name = "Sacred Icon";
                    break;

                case "06b":
                    Name = "Quarantine Zone";
                    break;

                case "07a":
                    Name = "Gravemind";
                    break;

                case "07b":
                    Name = "Uprising";
                    break;

                case "08a":
                    Name = "High Charity";
                    break;

                case "08b":
                    Name = "The Great Journey";
                    break;



                default:
                    break;
            }


            return Name;
        }

        public bool TestRange(int numberToCheck, int bottom, int top)
        {
            return (numberToCheck >= bottom && numberToCheck <= top);
        }

        public static void Log (string text, object sender)
        {
            System.Type type = sender.GetType();
            string s = (string)type.GetProperty("Name").GetValue(sender, null);
            List<string> addthis = new List<string> { DateTime.Now.ToString(), text, s };
            File.AppendAllLines(globals.localdir + @"\log.txt", addthis);
           

        }

        public static void Log(string text)
        {
            List<string> addthis = new List<string> { DateTime.Now.ToString(), text};
            try { File.AppendAllLines(globals.localdir + @"\log.txt", addthis); }
            catch
            {
                //what the fuck do we even do now?!
            }
        }

    }
}
