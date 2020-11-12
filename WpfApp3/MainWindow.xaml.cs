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
using Microsoft.Win32;
using WpfApp3.Properties;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
//using System.Windows.Shapes;
using System.ComponentModel;
//using System.Windows.Forms;
using System.Windows.Threading;
using System.Security.Principal;
using System.Threading;

//TODO LIST

/*
//ERROR HANDLING STUFF

DONE --- Make returns more informational. Instead of just returning, return true or false (success) + error message that calling function can popup a dialog about. Will have to divide functions up a bit but it's for the best. 

DONE --- Add way more try catches to anywhere that reads memory, or handles files. Add file length check too.

DONE --- Add many user dialogs for when errors popup. 

Verify online json file vs off-line one? Uh no just delete offline json file on first run (after update). 
//also delete config I guess? jic

DONE --- Add mandatory update online checks. 

DONE --- Add state indicator checks/lockout so no funny business on loading screen/pgcr

NOT GONNA DO --- Implement busy flag in maintick

Remove debugs from maintick except when Vals change. 

    //FEATURE STUFF

Implement profiles.

DONE --- , decided not to do levellockout (but did add check)---- Add level check/lockout. Add seed check to maintick. 

Add options for list level name - code (current), acronym, thumbnail. 

Change settings to have per-game options (clean up h1 checkpoints stuff) 

Clean up about page,stop it autoshowing on first run. 

Add last selected tab and save to config and autoload on start. Also mainlist column widths. And vertical scrollbar position. 

Add sorting functionality. 

Get all mp level splashes (clas and anni). 

Implement custom level splash images. With auto resizing. 

Add tool tip to double revert or just make icon better. 

Add open in Explorer button. 

Implement core save and dump + inject and revert for h1cs.

*/

namespace WpfApp3
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 

    public partial class MainWindow : Window
    {
        //for reading/writing from process memory
        [DllImport("kernel32.dll")]
        public static extern IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);
        [DllImport("kernel32.dll")]
        public static extern bool ReadProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, int dwSize, out int lpNumberOfBytesRead);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, int dwSize, out int lpNumberOfBytesWritten);

        const int PROCESS_WM_READ = 0x0010;
        const int PROCESS_ALL_ACCESS = 0x1F0FFF;

        private class HCMConfig
        {
            public string[] RanVersions;
            public string CoreFolderPath;
            public string CheckpointFolderPath;
            public bool ClassicMode = true;
        }

        private static class HCMGlobal
        {
            public static readonly string HCMversion = "0.9.0";

            public static readonly string LocalDir = System.IO.Path.GetDirectoryName(System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName);
            public static readonly string H1CoreSavePath = LocalDir + @"\saves\h1cs";
            public static readonly string H1CheckpointPath = LocalDir + @"\saves\h1cp";
            public static readonly string H2CheckpointPath = LocalDir + @"\saves\h2cp";
            public static readonly string HRCheckpointPath = LocalDir + @"\saves\hrcp";

            public static readonly string ConfigPath = LocalDir + @"\config.json";
            public static readonly string LogPath = LocalDir + @"\log.txt";
            public static readonly string OffsetsPath = LocalDir + @"\offsets\";

            public static HCMConfig SavedConfig;
            public static Offsets LoadedOffsets;
            public static MandatoryUpdates MandatoryUpdates;

            public static string ImageModeSuffix => SavedConfig.ClassicMode ? "clas" : "anni";

            public static bool padowomode = false;

            public static Int32 ProcessID;
            public static IntPtr GlobalProcessHandle;

            public static bool WinFlag = false; //false == steam, used for knowing which offsets to use
            public static bool BusyFlag = false; //turned true when injecting/dumping so we don't do it twice
            public static string AttachedGame = "No"; //No, Mn (menu), HR, H1, H2, H3, OD (ODST), H4
            public static string AttachedLevel; 
            public static bool VersionCheckedFlag = false;
            public static bool CheckedForOnlineOffsets = false;
            public static string MCCversion;
            public static bool GiveUpFlag = false; //set to true if attachment checking should cease forever
            public static bool OffsetsAcquired = false;
            public static IntPtr BaseAddress;
        }

        private class Offsets
        {
            //offsets are gonna be stored as 2-unit arrays, first position is winstore, second is steam
            //that way when we're calling them from elsewhere we can just call Offsets.WhateverOffset[HCMGlobal.WinFlag] and it'll give us the one we want
            //the units will themselves be arbitary length arrays (each position for each offset in a multi-level pointer)

            //the actual values will be populated from the json file corrosponding to the attached mcc version

            //general
            public int[][] gameindicator;
            public int[][] menuindicator;
            public int[][] stateindicator;

            //h1
            public int[][] H1_LevelName;
            public int[][] H1_CoreSave;
            public int[][] H1_CoreLoad;
            public int[][] H1_CheckString;
            public int[][] H1_TickCounter;
            public int[][] H1_Message;

            //hr
            public int[][] HR_LevelName;
            public int[][] HR_CheckString;
            public int[][] HR_Checkpoint; //for forcing checkpoints
            public int[][] HR_Revert; //for forcing reverts
            public int[][] HR_DRflag; //dr as in "double revert"
            public int[][] HR_CPLocation;
            public int[][] HR_LoadedSeed;

            //public static int[][] HR_StartSeed = new int[2][]; //seed of the level start - you get a different seed in reach every time you start the level from the main menu

        }

        private class MandatoryUpdates
        {
            public string[] VersionString;
        }

        public enum HaloGame
        {
            Halo1,
            Halo2,
            Halo3,
            HaloODST,
            HaloReach,
            Halo4,
            Halo5
        }

        public enum Difficulty
        {
            Invalid = -1,
            Easy = 0,
            Normal = 1,
            Heroic = 2,
            Legendary = 3,
            Multiplayer = 4,
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
            @"saves\hrcp",
            @"offsets",
        };

        public static bool IsElevated
        {
            get
            {
                return WindowsIdentity.GetCurrent().Owner
                  .IsWellKnown(WellKnownSidType.BuiltinAdministratorsSid);
            }
        }

        public ObservableCollection<HaloSaveFileMetadata> Halo1CoreSaves { get; set; } = new ObservableCollection<HaloSaveFileMetadata>();
        public ObservableCollection<HaloSaveFileMetadata> Halo1Checkpoints { get; set; } = new ObservableCollection<HaloSaveFileMetadata>();
        public ObservableCollection<HaloSaveFileMetadata> Halo2Checkpoints { get; set; } = new ObservableCollection<HaloSaveFileMetadata>();

        public ObservableCollection<HaloSaveFileMetadata> HaloReachCheckpoints { get; set; } = new ObservableCollection<HaloSaveFileMetadata>();

        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;

            if (IsElevated == false)
            {
                //popup error message that we need admin privledges, then close the application
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(" HCM needs admin privileges to operate, the application will now close. \n To run as admin, right click the exe and 'Run As Administrator' \n \n If you're (rightfully) cautious of giving software admin privs, \n feel free to inspect/build the source from over at \n github.com/Burnt-o/HaloCheckpointManager ", "Error", System.Windows.MessageBoxButton.OK);
                System.Windows.Application.Current.Shutdown();
            }


            //checking for HCM updates
                try
                {
                    String url = "https://raw.githubusercontent.com/Burnt-o/HaloCheckpointManager/master/WpfApp3/offsets/Updates.json";
                    System.Net.WebClient client = new System.Net.WebClient();
                    String json = client.DownloadString(url);
                    Debug("accessed updates file!" + json);

                    HCMGlobal.MandatoryUpdates = JsonConvert.DeserializeObject<MandatoryUpdates>(json);

                    //now check if our version matches any of the strings in the updates string array

                    foreach (string i in HCMGlobal.MandatoryUpdates.VersionString)
                    {
                        if (i == HCMGlobal.HCMversion)
                        {
                            MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show("It looks like HCM has recieved an important update. \nFeel free to continue using this version, but you may be missing important features or bugfixes. \n \nGrab the new version over at \n https://github.com/Burnt-o/HaloCheckpointManager/releases ", "Update", System.Windows.MessageBoxButton.OK);
                        }
                    }
                    Debug("finished checking hcm version");
                }
                catch
                {
                    Debug("failed to check hcm version");
                }
  





            SetEnabledUI(); //will initialize all the attachment-dependent stuff to be disabled

            //need to initialize timer that will check for attachment to mcc process
            //wasn't sure whether to put this here or in the above function but whatever
            DispatcherTimer dtClockTime = new DispatcherTimer();
            dtClockTime.Interval = new TimeSpan(0, 0, 1); //in Hour, Minutes, Second.
            dtClockTime.Tick += maintick;
            dtClockTime.Start();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            //some testing stuff

            //Debug("test: " + ReadLevelFromMemory());

            //end testing stuff

            CS_MainList.SelectionChanged += List_SelectionChanged;
            CP_MainList.SelectionChanged += List_SelectionChanged;
            H2CP_MainList.SelectionChanged += List_SelectionChanged;
            HRCP_MainList.SelectionChanged += List_SelectionChanged;
            bool firstrun = false;

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
                        firstrun = true; //for raising AboutWindow
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

            // Verify config was loaded, otherwise create a new one
            if (HCMGlobal.SavedConfig == null)
            {
                HCMGlobal.SavedConfig = new HCMConfig();
            }


            //if this is the first run of this hcm version, we need to do some STUFF
            //check if our version name is on first line of config file, if not, delete/remake config & offsets.json
            //then add our version name to config file.

            bool needtorefresh = true;
            foreach (string i in HCMGlobal.SavedConfig.RanVersions)
            {
                if (i == HCMGlobal.HCMversion)
                    needtorefresh = false;
            }
            Debug("needtorefresh is: " + needtorefresh.ToString());
            if (needtorefresh)
            {
                //delete offsets file if they exist (need to redownload, happens in maintick)
                try
                {
                    System.IO.DirectoryInfo di = new DirectoryInfo("offsets\\");


                    foreach (FileInfo file in di.GetFiles())
                    {
                        if (file.Extension == ".json")
                        {
                            file.Delete();
                        }
                    }
                }
                catch 
                {
                    Debug("error occured while deleting old offset jsons");
                }

                //then remake config file and add current hcm version to it
                string[] oldranversions = HCMGlobal.SavedConfig.RanVersions;
                HCMGlobal.SavedConfig = new HCMConfig();
                oldranversions = oldranversions.Append(HCMGlobal.HCMversion).ToArray();
                HCMGlobal.SavedConfig.RanVersions = oldranversions;

                foreach (string i in oldranversions)
                {
                    Debug("ver: " + i);
                }

                WriteConfig();
                //then reread config file
                using (StreamReader r = new StreamReader(HCMGlobal.ConfigPath))
                {
                    string json = r.ReadToEnd();
                    HCMGlobal.SavedConfig = JsonConvert.DeserializeObject<HCMConfig>(json);
                }
            }




            if (HCMGlobal.SavedConfig.CheckpointFolderPath == null)
            {

                //autodetect checkpoint folder path
                var CheckpointAutoPath = System.IO.Path.Combine(
                    Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
                    @"..\LocalLow\MCC\Config").ToString();

                if (Directory.Exists(CheckpointAutoPath))
                {
                    Log("Autodetected CP folder: " + CheckpointAutoPath);
                    HCMGlobal.SavedConfig.CheckpointFolderPath = System.IO.Path.GetFullPath(CheckpointAutoPath);
                    WriteConfig();
                }
            }

            if (HCMGlobal.SavedConfig.CoreFolderPath == null)
            {
                //autodetect core folder path
                string CoreAutoPath = GetInstallPath("Halo: The Master Chief Collection");
                if (CoreAutoPath != null && CoreAutoPath.Contains("steamapps")) //latter ought to be true if steam version of MCC
                {
                    CoreAutoPath = System.IO.Path.Combine(CoreAutoPath, @"..\core_saves");

                    if (Directory.Exists(CoreAutoPath))
                    {
                        Log("autodetected Core folder: " + CoreAutoPath);
                        HCMGlobal.SavedConfig.CoreFolderPath = System.IO.Path.GetFullPath(CoreAutoPath);
                        WriteConfig();
                    }
                }
            }

            if (firstrun == true)
            {
                AboutButton.RaiseEvent(new RoutedEventArgs(Button.ClickEvent)); //raise aboutwindow
                firstrun = false;
            }

            RefreshButton.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));

        }

        private void TabSelectionChanged(object sender, SelectionChangedEventArgs e)
        {

            if (e.Source is TabControl) //if this event fired from TabControl then enter
            {
                Debug("Tab selection changed");
                RefreshSel(sender, e);
                RefreshLoa(sender, e);
                RefreshList(sender, e);
            }

        }

        void List_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            RefreshSel(sender, e);
        }

        private void SwapFileTimes(string PathA, string PathB)
        {
            if (File.Exists(PathA) && File.Exists(PathB))
            {
                DateTime currentfilestime = File.GetLastWriteTime(PathA);
                DateTime abovefilestime = File.GetLastWriteTime(PathB);
                File.SetLastWriteTime(PathA, abovefilestime);
                File.SetLastWriteTime(PathB, currentfilestime);
            }
            else
            {
                Log("something went wrong: fileexists(movethis): " + File.Exists(PathA).ToString());
                Log("something went wrong: fileexists(abovefile): " + File.Exists(PathB).ToString());
            }
        }

        private void MaxFileTimes(string PathA, string PathB, bool ToTop)
        {
            //where A is the thing we want to move to the top
            if (File.Exists(PathA) && File.Exists(PathB))
            {
                DateTime currentfilestime = File.GetLastWriteTime(PathA);
                DateTime abovefilestime = File.GetLastWriteTime(PathB);
                if (ToTop)
                    File.SetLastWriteTime(PathA, abovefilestime.AddMinutes(1));
                else
                    File.SetLastWriteTime(PathA, abovefilestime.AddMinutes(-1));
                //File.SetLastWriteTime(PathB, currentfilestime);
            }
            else
            {
                Log("something went wrong: fileexists(movethis): " + File.Exists(PathA).ToString());
                Log("something went wrong: fileexists(abovefile): " + File.Exists(PathB).ToString());
            }

        }

        private void ArbitaryFileTimeMove(int startindex, int destindex, ListView mainlist, string path)
        {
            //need to iterate over mainlist and create array of times
            try
            {
                List<DateTime> arrayoftimes = new List<DateTime>();

                for (int i = 0; i < mainlist.Items.Count; i++)
                {
                    var filedata = mainlist.Items.GetItemAt(i) as HaloSaveFileMetadata;
                    arrayoftimes.Add(File.GetLastWriteTime($@"{path}\{filedata.Name}.bin"));
                }

                var tempfile = mainlist.Items.GetItemAt(destindex) as HaloSaveFileMetadata;
                var tempdate = File.GetLastWriteTime($@"{path}\{tempfile.Name}.bin");
                arrayoftimes.RemoveAt(destindex);
                arrayoftimes.Insert(startindex, tempdate);

                //now just need to apply the new times

                for (int i = 0; i < mainlist.Items.Count; i++)
                {
                    var filedata = mainlist.Items.GetItemAt(i) as HaloSaveFileMetadata;
                    File.SetLastWriteTime($@"{path}\{filedata.Name}.bin", arrayoftimes[i]);
                }
            }
            catch (Exception ex)
            {
                Log("unknown error occured: " + ex.ToString());
            }
        }

        private void MoveUpButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);

            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            switch (parent_name)
            {
                case "H1CS":
                    if (CS_MainList.SelectedIndex >= 1)
                    {
                        var fileBelow = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileAbove = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex - 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string movethis = $@"{HCMGlobal.H1CoreSavePath}\{fileBelow.Name}.bin";
                            string abovefile = $@"{HCMGlobal.H1CoreSavePath}\{fileAbove.Name}.bin";
                            SwapFileTimes(movethis, abovefile);
                            CS_MainList.SelectedIndex--;
                        }
                    }
                    break;
                case "H1CP":
                    if (CP_MainList.SelectedIndex >= 1)
                    {
                        var fileBelow = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileAbove = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex - 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string movethis = $@"{HCMGlobal.H1CheckpointPath}\{fileBelow.Name}.bin";
                            string abovefile = $@"{HCMGlobal.H1CheckpointPath}\{fileAbove.Name}.bin";
                            SwapFileTimes(movethis, abovefile);
                            CP_MainList.SelectedIndex--;
                        }
                    }
                    break;
                case "H2CP":
                    if (H2CP_MainList.SelectedIndex >= 1)
                    {
                        var fileBelow = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileAbove = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex - 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string movethis = $@"{HCMGlobal.H2CheckpointPath}\{fileBelow.Name}.bin";
                            string abovefile = $@"{HCMGlobal.H2CheckpointPath}\{fileAbove.Name}.bin";
                            SwapFileTimes(movethis, abovefile);
                            H2CP_MainList.SelectedIndex--;
                        }
                    }
                    break;
                case "HRCP":
                    if (HRCP_MainList.SelectedIndex >= 1)
                    {
                        var fileBelow = HRCP_MainList.Items.GetItemAt(HRCP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileAbove = HRCP_MainList.Items.GetItemAt(HRCP_MainList.SelectedIndex - 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string movethis = $@"{HCMGlobal.HRCheckpointPath}\{fileBelow.Name}.bin";
                            string abovefile = $@"{HCMGlobal.HRCheckpointPath}\{fileAbove.Name}.bin";
                            SwapFileTimes(movethis, abovefile);
                            HRCP_MainList.SelectedIndex--;
                        }
                    }
                    break;
            }

            RefreshList(sender, e);
        }

        private void MoveUpButton_All_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);

            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            parent = (FrameworkElement)((FrameworkElement)parent).Parent;
            string parent_name = parent.Name;

            switch (parent_name)
            {
                case "H1CS":
                    if (CS_MainList.SelectedIndex >= 0)
                    {
                        Debug("ee");
                        var fileBelow = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileAbove = CS_MainList.Items.GetItemAt(0) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {

                            string movethis = $@"{HCMGlobal.H1CoreSavePath}\{fileBelow.Name}.bin";
                            string abovefile = $@"{HCMGlobal.H1CoreSavePath}\{fileAbove.Name}.bin";
                            ArbitaryFileTimeMove(CS_MainList.SelectedIndex, 0, CS_MainList, HCMGlobal.H1CoreSavePath);
                            CS_MainList.SelectedIndex = 0;
                        }
                    }
                    break;
                case "H1CP":
                    if (CP_MainList.SelectedIndex >= 0)
                    {
                        var fileBelow = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileAbove = CP_MainList.Items.GetItemAt(0) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string movethis = $@"{HCMGlobal.H1CheckpointPath}\{fileBelow.Name}.bin";
                            string abovefile = $@"{HCMGlobal.H1CheckpointPath}\{fileAbove.Name}.bin";
                            ArbitaryFileTimeMove(CP_MainList.SelectedIndex, 0, CP_MainList, HCMGlobal.H1CheckpointPath);
                            CP_MainList.SelectedIndex = 0;
                        }
                    }
                    break;
                case "H2CP":
                    if (H2CP_MainList.SelectedIndex >= 0)
                    {
                        var fileBelow = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileAbove = H2CP_MainList.Items.GetItemAt(0) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string movethis = $@"{HCMGlobal.H2CheckpointPath}\{fileBelow.Name}.bin";
                            string abovefile = $@"{HCMGlobal.H2CheckpointPath}\{fileAbove.Name}.bin";
                            ArbitaryFileTimeMove(H2CP_MainList.SelectedIndex, 0, H2CP_MainList, HCMGlobal.H2CheckpointPath);
                            H2CP_MainList.SelectedIndex = 0;
                        }
                    }
                    break;
                case "HRCP":
                    if (HRCP_MainList.SelectedIndex >= 0)
                    {
                        var fileBelow = HRCP_MainList.Items.GetItemAt(HRCP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileAbove = HRCP_MainList.Items.GetItemAt(0) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string movethis = $@"{HCMGlobal.HRCheckpointPath}\{fileBelow.Name}.bin";
                            string abovefile = $@"{HCMGlobal.HRCheckpointPath}\{fileAbove.Name}.bin";
                            ArbitaryFileTimeMove(HRCP_MainList.SelectedIndex, 0, HRCP_MainList, HCMGlobal.HRCheckpointPath);
                            HRCP_MainList.SelectedIndex = 0;
                        }
                    }
                    break;
            }

            RefreshList(sender, e);

        }

        private void MoveDownButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);

            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            switch (parent_name)
            {
                case "H1CS":
                    if (CS_MainList.SelectedItem != null && CS_MainList.SelectedIndex != CS_MainList.Items.Count - 1)
                    {
                        var fileAbove = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileBelow = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex + 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string pathAbove = $@"{HCMGlobal.H1CoreSavePath}\{fileAbove.Name}.bin";
                            string pathBelow = $@"{HCMGlobal.H1CoreSavePath}\{fileBelow.Name}.bin";
                            SwapFileTimes(pathAbove, pathBelow);
                            CS_MainList.SelectedIndex++;
                        }
                    }
                    break;
                case "H1CP":
                    if (CP_MainList.SelectedItem != null && CP_MainList.SelectedIndex != CP_MainList.Items.Count - 1)
                    {
                        var fileAbove = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileBelow = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex + 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string pathAbove = $@"{HCMGlobal.H1CheckpointPath}\{fileAbove.Name}.bin";
                            string pathBelow = $@"{HCMGlobal.H1CheckpointPath}\{fileBelow.Name}.bin";
                            SwapFileTimes(pathAbove, pathBelow);
                            CP_MainList.SelectedIndex++;
                        }
                    }
                    break;
                case "H2CP":
                    if (H2CP_MainList.SelectedItem != null && H2CP_MainList.SelectedIndex != H2CP_MainList.Items.Count - 1)
                    {
                        var fileAbove = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileBelow = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex + 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string pathAbove = $@"{HCMGlobal.H2CheckpointPath}\{fileAbove.Name}.bin";
                            string pathBelow = $@"{HCMGlobal.H2CheckpointPath}\{fileBelow.Name}.bin";
                            SwapFileTimes(pathAbove, pathBelow);
                            H2CP_MainList.SelectedIndex++;
                        }
                    }
                    break;
                case "HRCP":
                    if (HRCP_MainList.SelectedItem != null && HRCP_MainList.SelectedIndex != HRCP_MainList.Items.Count - 1)
                    {
                        var fileAbove = HRCP_MainList.Items.GetItemAt(HRCP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileBelow = HRCP_MainList.Items.GetItemAt(HRCP_MainList.SelectedIndex + 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string pathAbove = $@"{HCMGlobal.HRCheckpointPath}\{fileAbove.Name}.bin";
                            string pathBelow = $@"{HCMGlobal.HRCheckpointPath}\{fileBelow.Name}.bin";
                            SwapFileTimes(pathAbove, pathBelow);
                            HRCP_MainList.SelectedIndex++;
                        }
                    }
                    break;
            }

            RefreshList(sender, e);
        }

        private void MoveDownButton_All_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);

            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            parent = (FrameworkElement)((FrameworkElement)parent).Parent;
            string parent_name = parent.Name;

            switch (parent_name)
            {
                case "H1CS":
                    if (CS_MainList.SelectedItem != null && CS_MainList.SelectedIndex != CS_MainList.Items.Count - 1)
                    {
                        var fileAbove = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileBelow = CS_MainList.Items.GetItemAt(CS_MainList.Items.Count - 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string pathAbove = $@"{HCMGlobal.H1CoreSavePath}\{fileAbove.Name}.bin";
                            string pathBelow = $@"{HCMGlobal.H1CoreSavePath}\{fileBelow.Name}.bin";
                            ArbitaryFileTimeMove(CS_MainList.SelectedIndex, CS_MainList.Items.Count - 1, CS_MainList, HCMGlobal.H1CoreSavePath);
                            CS_MainList.SelectedIndex = CS_MainList.Items.Count - 1;
                        }
                    }
                    break;
                case "H1CP":
                    if (CP_MainList.SelectedItem != null && CP_MainList.SelectedIndex != CP_MainList.Items.Count - 1)
                    {
                        var fileAbove = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileBelow = CP_MainList.Items.GetItemAt(CP_MainList.Items.Count - 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string pathAbove = $@"{HCMGlobal.H1CheckpointPath}\{fileAbove.Name}.bin";
                            string pathBelow = $@"{HCMGlobal.H1CheckpointPath}\{fileBelow.Name}.bin";
                            ArbitaryFileTimeMove(CP_MainList.SelectedIndex, CP_MainList.Items.Count - 1, CP_MainList, HCMGlobal.H1CheckpointPath);
                            CP_MainList.SelectedIndex = CP_MainList.Items.Count - 1;
                        }
                    }
                    break;
                case "H2CP":
                    if (H2CP_MainList.SelectedItem != null && H2CP_MainList.SelectedIndex != H2CP_MainList.Items.Count - 1)
                    {
                        var fileAbove = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileBelow = H2CP_MainList.Items.GetItemAt(H2CP_MainList.Items.Count - 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string pathAbove = $@"{HCMGlobal.H2CheckpointPath}\{fileAbove.Name}.bin";
                            string pathBelow = $@"{HCMGlobal.H2CheckpointPath}\{fileBelow.Name}.bin";
                            ArbitaryFileTimeMove(H2CP_MainList.SelectedIndex, H2CP_MainList.Items.Count - 1, H2CP_MainList, HCMGlobal.H2CheckpointPath);
                            H2CP_MainList.SelectedIndex = H2CP_MainList.Items.Count - 1;
                        }
                    }
                    break;
                case "HRCP":
                    if (HRCP_MainList.SelectedItem != null && HRCP_MainList.SelectedIndex != HRCP_MainList.Items.Count - 1)
                    {
                        var fileAbove = HRCP_MainList.Items.GetItemAt(HRCP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        var fileBelow = HRCP_MainList.Items.GetItemAt(HRCP_MainList.Items.Count - 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string pathAbove = $@"{HCMGlobal.HRCheckpointPath}\{fileAbove.Name}.bin";
                            string pathBelow = $@"{HCMGlobal.HRCheckpointPath}\{fileBelow.Name}.bin";
                            ArbitaryFileTimeMove(HRCP_MainList.SelectedIndex, HRCP_MainList.Items.Count - 1, HRCP_MainList, HCMGlobal.HRCheckpointPath);
                            HRCP_MainList.SelectedIndex = HRCP_MainList.Items.Count - 1;
                        }
                    }
                    break;
            }

            RefreshList(sender, e);

        }

        private void RenameButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);

            var btn = sender as Button;
            if (btn == null)
                return;

            string s2 = "";
            string proposedsave = "";
            string backup = "";
            string backuploc;
            switch (btn.Name)
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

        /* private void ConvertButton_Click(object sender, RoutedEventArgs e)
         {
             RefreshSel(sender, e);

             var btn = sender as Button;
             if (btn == null)
                 return;

             string convertfrom = "";
             string convertto = "";
             string converttoloc = "";
             string s2 = "";

             switch (btn.Name)
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
         }*/

        private void DeleteSaveFile(string path)
        {
            try
            {
                File.Delete(path);
            }
            catch (Exception exp)
            {
                Log("something went wrong trying to delete a save: " + exp);
                //need to make this a popup to let user know what was bad
            }
        }

        private void DeleteButton_Click(object sender, RoutedEventArgs e)
        {
            var oldselected = 0;

            RefreshSel(sender, e);
            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            switch (parent_name)
            {
                case "H1CS":
                    oldselected = CS_MainList.SelectedIndex;
                    if (CS_MainList.SelectedItem != null)
                    {
                        var item = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        string path = $@"{HCMGlobal.H1CoreSavePath}\{item.Name}.bin";

                        MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show("Delete " + item.Name + "?", "Delete Confirmation", System.Windows.MessageBoxButton.YesNo);
                        if (messageBoxResult == MessageBoxResult.Yes)
                        {
                            DeleteSaveFile(path);
                        }
                    }
                    RefreshList(sender, e);
                    CS_MainList.SelectedIndex = oldselected;
                    RefreshSel(sender, e);
                    break;

                case "HRCP":
                    oldselected = HRCP_MainList.SelectedIndex;
                    if (HRCP_MainList.SelectedItem != null)
                    {
                        var item = HRCP_MainList.Items.GetItemAt(HRCP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        string path = $@"{HCMGlobal.HRCheckpointPath}\{item.Name}.bin";

                        MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show("Delete " + item.Name + "?", "Delete Confirmation", System.Windows.MessageBoxButton.YesNo);
                        if (messageBoxResult == MessageBoxResult.Yes)
                        {
                            DeleteSaveFile(path);
                        }
                    }
                    RefreshList(sender, e);
                    HRCP_MainList.SelectedIndex = oldselected;
                    RefreshSel(sender, e);
                    break;

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
            SettingsWindow settingswindow = new SettingsWindow();

            settingswindow.ChosenCore.Text = HCMGlobal.SavedConfig?.CoreFolderPath ?? "No folder chosen!";
            settingswindow.ChosenCP.Text = HCMGlobal.SavedConfig?.CheckpointFolderPath ?? "No folder chosen!";
            settingswindow.modeanni.IsChecked = !HCMGlobal.SavedConfig?.ClassicMode ?? true;
            settingswindow.modeclas.IsChecked = HCMGlobal.SavedConfig?.ClassicMode ?? false;

            settingswindow.ShowDialog();

            HCMGlobal.SavedConfig.CoreFolderPath =
                (settingswindow.ChosenCore.Text != null && settingswindow.ChosenCore.Text != "No Folder chosen!")
                ? HCMGlobal.SavedConfig.CoreFolderPath = settingswindow.ChosenCore.Text
                : HCMGlobal.SavedConfig.CoreFolderPath = "";

            HCMGlobal.SavedConfig.CheckpointFolderPath =
                (settingswindow.ChosenCP.Text != null && settingswindow.ChosenCore.Text != "No Folder chosen!")
                ? HCMGlobal.SavedConfig.CheckpointFolderPath = settingswindow.ChosenCP.Text
                : HCMGlobal.SavedConfig.CheckpointFolderPath = "";

            HCMGlobal.SavedConfig.ClassicMode = settingswindow.modeclas.IsChecked ?? false;

            WriteConfig();
            RefreshLoa(sender, e);
            RefreshSel(sender, e);
        }

        private void AboutButton_Click(object sender, RoutedEventArgs e)
        {
            AboutWindow aboutwindow = new AboutWindow();
            aboutwindow.ShowDialog();
        }

        private void RefreshLoa(object sender, RoutedEventArgs e)
        {

            switch (TabList.SelectedIndex)
            {
                case 0: //H1 CORES 

                    if (HCMGlobal.SavedConfig != null && File.Exists(HCMGlobal.SavedConfig.CoreFolderPath + @"\core.bin") && HCMGlobal.SavedConfig.CoreFolderPath != null)
                    {
                        var data = GetSaveFileMetadata(HCMGlobal.SavedConfig.CoreFolderPath + @"\core.bin", HaloGame.Halo1);
                        H1CS_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode);

                        if (data.Difficulty != Difficulty.Invalid)
                            H1CS_Loa_DiffName.Source = new BitmapImage(new Uri($"images/H1/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                        H1CS_Loa_Time.Text = TickToTimeString(data.StartTick, true);

                        if (LevelCodeToGameType(data.LevelCode))
                        {
                            H1CS_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/H1/mp.png", UriKind.Relative));
                        }
                        else
                        {
                            H1CS_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/H1/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                        }

                    }
                    else
                    {
                        H1CS_Loa_LevelName.Text = "N/A";
                        H1CS_Loa_DiffName.Source = null;
                        H1CS_Loa_Time.Text = "N/A";
                        H1CS_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
                    }



                    break;

                case 3: //reach
                    if (HCMGlobal.AttachedGame == "HR" && ValidCheck_HR())
                    {
                        //data to get; level code, diff, time

                        //first check double revert flag
                        int bytesWritten;
                        bool DRflag;
                        byte[] DRbuffer = new byte[1];
                        var data = new HaloSaveFileMetadata();

                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                        {
                            DRflag = Convert.ToBoolean(DRbuffer[0]);
                        }
                        else { Debug("oh no"); NullReach(); return; }

                        int offset;
                        if (!DRflag)
                        {
                            offset = -0xA10000; //first cp
                        }
                        else
                        {
                            offset = 0x0; //second cp
                        }

                        int[] addy = HCMGlobal.LoadedOffsets.HR_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                        addy[3] = offset;
                        //setup done, let's get our data
                        //levelcode
                        byte[] buffer = new byte[32];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + 0xFD73, buffer, buffer.Length, out bytesWritten))
                        {
                            char[] exceptions = new char[] { '_' };
                            data.LevelCode = String.Concat(Encoding.UTF8.GetString(buffer, 0, buffer.Length).Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                        }
                        else { Debug("oh no"); NullReach(); return; }

                        //difficulty
                        if (LevelCodeToGameType(data.LevelCode))
                        {
                            data.Difficulty = (Difficulty)4;
                        }
                        else
                        {
                            buffer = new byte[1];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + 0xFEFC, buffer, buffer.Length, out bytesWritten))
                            {
                                data.Difficulty = (Difficulty)buffer[0];
                            }
                            else { Debug("oh no"); NullReach(); return; }
                        }
                        //tickcount
                        buffer = new byte[4];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + 0x10FD54, buffer, buffer.Length, out bytesWritten))
                        {
                            data.StartTick = BitConverter.ToUInt32(buffer, 0);
                        }
                        else { Debug("oh no"); NullReach(); return; }

                        //level start seed
                        buffer = new byte[4];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + 0x148, buffer, buffer.Length, out bytesWritten))
                        {
                            data.Seed = (Convert.ToString(BitConverter.ToUInt32(buffer, 0), 16)).ToUpper();
                        }
                        else { Debug("oh no"); NullReach(); return; }

                        //now assign to ui
                        HRCP_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode);

                        HRCP_Loa_Time.Text = TickToTimeString(data.StartTick, true);

                        if (data.Difficulty != Difficulty.Invalid)
                            HRCP_Loa_DiffName.Source = new BitmapImage(new Uri($"images/HR/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                        if (LevelCodeToGameType(data.LevelCode))
                        {
                            HRCP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/HR/mp.png", UriKind.Relative));
                        }
                        else
                        {
                            HRCP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/HR/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                        }

                        HRCP_Loa_Seed.Content = "Seed: " + data.Seed;

                    }
                    else { Debug("oh no"); NullReach(); }



                    break;

                    void NullReach()
                    {
                        HRCP_Loa_LevelName.Text = "N/A";
                        HRCP_Loa_DiffName.Source = null;
                        HRCP_Loa_Time.Text = "N/A";
                        HRCP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
                        HRCP_Loa_Seed.Content = "Seed: N/A";
                    }

            }

        }

        private void RefreshSel(object sender, RoutedEventArgs e)
        {

            switch (TabList.SelectedIndex)
            {
                case 0: //H1 CORES 
                    if (CS_MainList.SelectedItem != null)
                    {
                        var item = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex);
                        System.Type type = item.GetType();
                        string s = (string)type.GetProperty("Name").GetValue(item, null);
                        var pathtotest = HCMGlobal.H1CoreSavePath + @"\" + s + @".bin";

                        if (File.Exists(pathtotest))
                        {
                            var data = GetSaveFileMetadata(pathtotest, HaloGame.Halo1);
                            H1CS_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode);

                            if (data.Difficulty != Difficulty.Invalid)
                                H1CS_Sel_DiffName.Source = new BitmapImage(new Uri($"images/H1/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                            H1CS_Sel_Time.Text = TickToTimeString(data.StartTick, true);
                            H1CS_Sel_FileName.Text = s;

                            if (LevelCodeToGameType(data.LevelCode))
                            {
                                H1CS_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/H1/mp.png", UriKind.Relative));
                            }
                            else
                            {
                                H1CS_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/H1/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                            }

                        }
                    }
                    else
                    {
                        H1CS_Sel_LevelName.Text = "N/A";
                        H1CS_Sel_DiffName.Source = null;
                        H1CS_Sel_Time.Text = "N/A";
                        H1CS_Sel_FileName.Text = "N/A";
                        H1CS_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
                    }
                    break;

                case 3: //reach
                    if (HRCP_MainList.SelectedItem != null)
                    {
                        var item = HRCP_MainList.Items.GetItemAt(HRCP_MainList.SelectedIndex);
                        System.Type type = item.GetType();
                        string s = (string)type.GetProperty("Name").GetValue(item, null);
                        var pathtotest = HCMGlobal.HRCheckpointPath + @"\" + s + @".bin";

                        if (File.Exists(pathtotest))
                        {
                            var data = GetSaveFileMetadata(pathtotest, HaloGame.HaloReach);
                            HRCP_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode);

                            if (data.Difficulty != Difficulty.Invalid)
                                HRCP_Sel_DiffName.Source = new BitmapImage(new Uri($"images/HR/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                            HRCP_Sel_Time.Text = TickToTimeString(data.StartTick, true);
                            HRCP_Sel_FileName.Text = s;

                            if (LevelCodeToGameType(data.LevelCode))
                            {
                                HRCP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/HR/mp.png", UriKind.Relative));
                            }
                            else
                            {
                                HRCP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/HR/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                            }

                            HRCP_Sel_Seed.Content = "Seed: " + data.Seed;
                        }
                    }
                    else
                    {
                        HRCP_Sel_LevelName.Text = "N/A";
                        HRCP_Sel_DiffName.Source = null;
                        HRCP_Sel_Time.Text = "N/A";
                        HRCP_Sel_FileName.Text = "N/A";
                        HRCP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
                        HRCP_Sel_Seed.Content = "Seed: N/A";
                    }
                    break;

                default:
                    break;

            }

        }

        private void HRCP_Sel_LevelImage_ImageFailed(object sender, ExceptionRoutedEventArgs e)
        {
            throw new NotImplementedException();
        }

        private void RefreshList(object sender, RoutedEventArgs e)
        {
            var oldCSselected = CS_MainList.SelectedIndex;
            var oldCPselected = CP_MainList.SelectedIndex;
            var oldH2CPselected = H2CP_MainList.SelectedIndex;
            var oldHRCPselected = HRCP_MainList.SelectedIndex;

            List<string> FilesPost = new List<string>();

            //let's only update the active tab (we'll later also add a RefreshList whenever the tab is changed)
            Debug(TabList.SelectedIndex.ToString());
            switch (TabList.SelectedIndex)
            {
                case 0: //h1 cores
                    Halo1CoreSaves.Clear();
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

                        if (FilesPost.Count > 0)
                        {
                            H1CS_MainList_Label.Content = "";
                            foreach (string File in FilesPost)
                            {
                                var data = GetSaveFileMetadata(HCMGlobal.H1CoreSavePath + "/" + File, HaloGame.Halo1);
                                data.Name = File.Substring(0, File.Length - 4);
                                Halo1CoreSaves.Add(data);
                            }

                            CS_MainList.SelectedIndex = oldCSselected;
                            GridView gv = CS_MainList.View as GridView;
                            UpdateColumnWidths(gv);
                        }
                        else
                        {
                            H1CS_MainList_Label.Content = "No backup saves in local folder.";
                        }
                    }
                    else
                    {
                        H1CS_MainList_Label.Content = "Core folder path is invalid, check Settings.";
                    }
                    break;

                case 3: //halo reach
                    HaloReachCheckpoints.Clear();
                    if (Directory.Exists(HCMGlobal.HRCheckpointPath)) // make sure path is valid
                    {
                        DirectoryInfo dir = new DirectoryInfo(HCMGlobal.HRCheckpointPath);
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

                        if (FilesPost.Count > 0)
                        {
                            HRCP_MainList_Label.Content = "";
                            foreach (string File in FilesPost)
                            {
                                var data = GetSaveFileMetadata(HCMGlobal.HRCheckpointPath + "/" + File, HaloGame.HaloReach);
                                data.Name = File.Substring(0, File.Length - 4);
                                HaloReachCheckpoints.Add(data);
                            }

                            HRCP_MainList.SelectedIndex = oldHRCPselected;
                            GridView gv = HRCP_MainList.View as GridView;
                            UpdateColumnWidths(gv);
                        }
                        else
                        {
                            HRCP_MainList_Label.Content = "No backup saves in local folder.";
                        }
                    }
                    else
                    {
                        //this should never happen lmao
                        Debug("how in the goddamn fuck?");
                    }

                    break;

                default:
                    break;

            }

        }

        public class HaloSaveFileMetadata
        {
            public HaloGame Game { get; set; }
            public string LevelCode { get; set; } = "xxx";
            public uint StartTick { get; set; } = 0;
            public Difficulty Difficulty { get; set; } = Difficulty.Easy;

            public string Seed { get; set; } = "N/A";
            public string Name { get; set; }

            //split difficultyimage into these two, probably a dumb way to do this but idk how this shit works ¯\_(ツ)_/¯
            public string DifficultyImageH1 => $"images/H1/diff_{(int)Difficulty}.png";
            public string DifficultyImageHR => $"images/HR/diff_{(int)Difficulty}.png";
            public string TimeString => TickToTimeString(StartTick, false);
        }

        private HaloSaveFileMetadata GetSaveFileMetadata(string saveFilePath, HaloGame game)
        {
            int offsetLevelCode;
            int offsetStartTick;
            int offsetDifficulty;
            int offsetSeed;

            switch (game)
            {
                case HaloGame.Halo1:
                    offsetLevelCode = 11;
                    offsetStartTick = 756;
                    offsetDifficulty = 294;
                    offsetSeed = 0;
                    break;
                case HaloGame.Halo2:
                    offsetLevelCode = 23;
                    offsetStartTick = 10808;
                    offsetDifficulty = 974;
                    offsetSeed = 0;
                    break;
                case HaloGame.HaloReach:
                    offsetLevelCode = 0xFD73;
                    offsetStartTick = 0x10FD54;
                    offsetDifficulty = 0xFEFC;
                    offsetSeed = 0x148;
                    break;
                default:
                    throw new NotSupportedException();
            }

            HaloSaveFileMetadata metadata = new HaloSaveFileMetadata() { Game = game };

            if (File.Exists(saveFilePath))

            {
                FileInfo f = new FileInfo(saveFilePath);
                if (f.Length > 10000)
                {
                    try
                    {
                        using (FileStream readStream = new FileStream(saveFilePath, FileMode.Open))
                        {
                            BinaryReader readBinary = new BinaryReader(readStream);

                            //get levelname
                            readBinary.BaseStream.Seek(offsetLevelCode, SeekOrigin.Begin);
                            metadata.LevelCode = new string(readBinary.ReadChars(32));
                            metadata.LevelCode = metadata.LevelCode.Substring(metadata.LevelCode.LastIndexOf("\\") + 1);
                            metadata.LevelCode = metadata.LevelCode.Substring(metadata.LevelCode.LastIndexOf("\\") + 1);
                            char[] exceptions = new char[] { '_' };
                            metadata.LevelCode = String.Concat(metadata.LevelCode.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                            //get time
                            readBinary.BaseStream.Seek(offsetStartTick, SeekOrigin.Begin);
                            UInt32 holdme = readBinary.ReadUInt32();
                            if (game != HaloGame.Halo1)
                                metadata.StartTick = holdme;
                            else
                                metadata.StartTick = holdme * 2;

                            //get difficulty
                            readBinary.BaseStream.Seek(offsetDifficulty, SeekOrigin.Begin);

                            if (LevelCodeToGameType(metadata.LevelCode))
                            {
                                metadata.Difficulty = (Difficulty)4;
                            }
                            else
                            {
                                metadata.Difficulty = (Difficulty)readBinary.ReadByte();
                            }
                            if (game == HaloGame.HaloReach)
                            {
                                readBinary.BaseStream.Seek(offsetSeed, SeekOrigin.Begin);
                                metadata.Seed = (Convert.ToString((readBinary.ReadUInt32()), 16)).ToUpper();
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(ex.ToString());
                    }
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
                if (currentfile.Name != File.Name && File.LastWriteTime == currentfile.LastWriteTime)
                {
                    return true;
                }
            }
            return false;
        }

        public static void WriteConfig()
        {
            try
            {
                string json = JsonConvert.SerializeObject(HCMGlobal.SavedConfig, Formatting.Indented);
                File.WriteAllText(HCMGlobal.ConfigPath, json);
            }
            catch (Exception e)
            {
                Log("problem writing config" + e);
            }
        }

        public static string TickToTimeString(uint ticks, bool subseconds)
        {
            int seconds = (int)ticks / 60;
            if (subseconds)
            {
                float milliseconds = 1000 * (((float)ticks % 60) / 60);
                TimeSpan ts = new TimeSpan(0, seconds / (60 * 60), seconds / (60), seconds % 60, (int)Math.Round((float)milliseconds));
                return ts.ToString(@"mm\:ss\.ff");
            }
            else
            {
                TimeSpan ts = new TimeSpan(seconds / (60 * 60), seconds / (60), seconds % 60);
                return ts.ToString(@"mm\:ss");
            }

        }

        readonly Dictionary<string, string> LevelCodeToName = new Dictionary<string, string>()
        {
            //maybe I'll add multiplayer levels to this eventually. or extra entry for "acronym'd name"

            // Halo 1
            //SP
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
            //MP
            { "beavercreek", "Battle Creek" },
            { "boardingaction", "Boarding Action" },
            { "bloodgulch", "Blood Gulch" },
            { "carousel", "Derelict" },
            { "chillout", "Chill Out" },
            { "damnation", "Damnation" },
            { "dangercanyon", "Danger Canyon" },
            { "deathisland", "Death Island" },
            { "gephyrophobia", "Gephyrophobia" },
            { "hangemhigh", "Hang 'Em High" },
            { "icefields", "Ice Fields" },
            { "infinity", "Infinity" },
            { "longest", "Longest" },
            { "prisoner", "Prisoner" },
            { "putput", "Chiron TL-34" },
            { "ratrace", "Rat Race" },
            { "sidewinder", "Sidewinder" },
            { "timberland", "Timberland" },
            { "wizard", "Wizard" },

            // Halo 2
            //SP
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
            //MP
            //tbd

            // Halo Reach
            //SP
            { "m05", "Noble Actual" },
            { "m10", "Winter Contingency" },
            { "m20", "ONI: Sword Base" },
            { "m30", "Nightfall" },
            { "m35", "Tip of the Spear" },
            { "m45", "Long Night of Solace" },
            { "m50", "Exodus" },
            { "m52", "New Alexandria" },
            { "m60", "The Package" },
            { "m70", "The Pillar of Autumn" },
            { "m70_a", "Credits" },
            { "m70_bonus", "Lone Wolf" },
            //MP
            { "20_sword_slayer", "Sword Base" },
            { "30_settlement", "Powerhouse" },
            { "35_island", "Spire" },
            { "45_aftship", "Zealot" },
            { "45_launch_station", "Countdown" },
            { "50_panopticon", "Boardwalk" },
            { "52_ivory_tower", "Reflection" },
            { "70_boneyard", "Boneyard" },
            { "forge_halo", "Forge World" },
            { "dlc_invasion ", "Breakpoint" },
            { "dlc_medium ", "Tempest" },
            { "dlc_slayer ", "Anchor 9" },
            { "cex_beaver_creek ", "Battle Canyon" },
            { "cex_damnation ", "Penance" },
            { "cex_ff_halo ", "Installation 04" },
            { "cex_hangemhigh ", "High Noon" },
            { "cex_headlong ", "Breakneck" },
            { "cex_prisoner ", "Solitary" },
            { "cex_timberland ", "Ridgeline" },
            { "condemned ", "Condemned" },
            { "ff_unearthed ", "Unearthed" },
            { "trainingpreserve ", "Highlands" },
            { "ff10_prototype ", "Overlook" },
            { "ff20_courtyard ", "Courtyard" },
            { "ff30_waterfront ", "Waterfront" },
            { "ff45_corvette ", "Corvette" },
            { "ff50_park ", "Beachhead" },
            { "ff60_airview ", "Outpost" },
            { "ff60 icecave ", "Glacier" },
            { "ff70_holdout ", "Holdout" },

        };

        readonly Dictionary<string, bool> LevelCodeToType = new Dictionary<string, bool>()
        {
            //false = solo, true = multi
            //again, there's gotta be a better way to code this than having two dictionaries but eh

            // Halo 1
            //SP
            { "a10", false },
            { "a30", false },
            { "a50", false },
            { "b30", false },
            { "b40", false },
            { "c10", false },
            { "c20", false },
            { "c40", false },
            { "d20", false },
            { "d40", false },
            //MP
            { "beavercreek", true },
            { "boardingaction", true },
            { "bloodgulch", true },
            { "carousel", true },
            { "chillout", true },
            { "damnation", true },
            { "dangercanyon", true },
            { "deathisland", true },
            { "gephyrophobia", true },
            { "hangemhigh", true },
            { "icefields", true },
            { "infinity", true },
            { "longest", true },
            { "prisoner", true },
            { "putput", true },
            { "ratrace", true},
            { "sidewinder", true },
            { "timberland", true },
            { "wizard", true },

            // Halo 2
            //SP
            { "00a", false },
            { "01a", false },
            { "01b", false },
            { "03a", false },
            { "03b", false },
            { "04a", false },
            { "04b", false },
            { "05a", false },
            { "05b", false },
            { "06a", false },
            { "06b", false },
            { "07a", false },
            { "07b", false },
            { "08a", false },
            { "08b", false },
            //MP
            //tbd

            // Halo Reach
            //SP
            { "m05", false },
            { "m10", false },
            { "m20", false },
            { "m30", false },
            { "m35", false },
            { "m45", false },
            { "m50", false },
            { "m52", false },
            { "m60", false },
            { "m70", false },
            { "m70_a", false },
            { "m70_bonus", false },
            //MP
            { "20_sword_slayer", true },
            { "30_settlement", true },
            { "35_island", true },
            { "45_aftship", true },
            { "45_launch_station", true },
            { "50_panopticon", true },
            { "52_ivory_tower", true },
            { "70_boneyard", true },
            { "forge_halo", true },
            { "dlc_invasion ", true },
            { "dlc_medium ", true },
            { "dlc_slayer ", true },
            { "cex_beaver_creek ", true },
            { "cex_damnation ", true },
            { "cex_ff_halo ", true },
            { "cex_hangemhigh ", true },
            { "cex_headlong ", true },
            { "cex_prisoner ", true },
            { "cex_timberland ", true },
            { "condemned ", true },
            { "ff_unearthed ", true },
            { "trainingpreserve ", true },
            { "ff10_prototype ", true },
            { "ff20_courtyard ", true },
            { "ff30_waterfront ", true },
            { "ff45_corvette ", true },
            { "ff50_park ", true },
            { "ff60_airview ", true },
            { "ff60 icecave ", true },
            { "ff70_holdout ", true },

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

        public bool LevelCodeToGameType(string code)
        {
            //return false for solo, true for multi
            bool Type;
            if (LevelCodeToType.TryGetValue(code, out Type))
            {
                return Type;
            }

            return false;
        }

        public static void Log(string text, object sender)
        {
            List<string> addthis = new List<string> { $"{DateTime.Now} - {(sender as Control)?.Name}", text };
            File.AppendAllLines(HCMGlobal.LogPath, addthis);
            Debug(addthis.ToString());
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

        public static string GetInstallPath(string c_name)
        {
            string displayName;

            string registryKey = @"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall";
            RegistryKey key = Registry.LocalMachine.OpenSubKey(registryKey);
            if (key != null)
            {
                foreach (RegistryKey subkey in key.GetSubKeyNames().Select(keyName => key.OpenSubKey(keyName)))
                {
                    displayName = subkey.GetValue("DisplayName") as string;
                    if (displayName != null && displayName.Equals(c_name))
                    {
                        return subkey.GetValue("InstallLocation") as string;
                    }
                }
                key.Close();
            }
            return null;
        }

        private void ForceCPButton_Click(object sender, RoutedEventArgs e)
        {
            //figure out which game this was for
            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            var (success, error) = ForceCPFunction(parent_name, sender, e);

            if (success == false)
            {
                Debug("Failed to force save, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to force save, " + error, "Error", System.Windows.MessageBoxButton.OK);
                return;
            }
            RefreshLoa(sender, e);

        }

        private (bool success, string error) ForceCPFunction(string game, object sender, RoutedEventArgs e)
        {
            int bytesWritten;
            switch (game)
            {

                case "H1CS":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "H1" && ValidCheck_H1())
                        {

                            byte[] buffer;

                            //next, the custom message stuff
                            //acquire the current tickcount
                            buffer = new byte[4];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H1_TickCounter[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h1: custom message 1: success");
                            }
                            else
                            {
                                return (false, "message 1 failure");
                            }

                            //buffer will be equal to the tickcounter so just paste it into the new spot
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H1_Message[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h1: custom message 2: success");
                            }
                            else
                            {
                                return (false, "message 2 failure");
                            }

                            buffer = new byte["Core Save... done".Length * 2]; //halo uses widechar for it's message strings, so double the length needed.
                            buffer = Encoding.Unicode.GetBytes("Core Save... done");
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H1_Message[Convert.ToInt32(HCMGlobal.WinFlag)]) + 0x4, buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h1: custom message 3: success");
                            }
                            else
                            {
                                return (false, "message 3 failure");
                            }

                            buffer = new byte[8] { 0, 0, 1, 0, 0xFF, 0xFF, 0xFF, 0xFF }; //setting showmessage flag to true
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H1_Message[Convert.ToInt32(HCMGlobal.WinFlag)]) + 0x80, buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h1: custom message 4: success");
                            }
                            else
                            {
                                return (false, "message 4 failure");
                            }

                            buffer = new byte[1] { 1 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H1_CoreSave[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h1: made core save");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "failed to write core save byte");
                            }

                        }
                        else
                        {
                            return (false, "not attached to game");
                        }
                    }
                    catch (Exception ex)
                    {
                        return (false, "unknown error occured: " + ex.ToString());
                    }

                case "HRCP":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "HR" && ValidCheck_HR())
                        {
                            byte[] buffer = new byte[1] { 1 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_Checkpoint[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("hr: made cp");
                                return (true, "success!");
                            }
                            else
                            {
                                Debug("hr: failed to make cp");
                                return (false, "failed to write cp byte");
                            }
                        }
                        else
                        {
                            return (false, "failed to make cp because not attached");
                        }

                    }
                    catch (Exception ex)
                    {
                        return (false, "unknown error occured: " + ex.ToString());
                    }

                default:
                    return (false, "invalid game");

            }

        }

        private void ForceRevertButton_Click(object sender, RoutedEventArgs e)
        {
            //figure out which game this was for
            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            //do reverty things
            var (success, error) = ForceRevertFunction(parent_name, sender, e);

            if (success == false)
            {
                Debug("Failed to force load, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to force load, " + error, "Error", System.Windows.MessageBoxButton.OK);
            }

        }

        private (bool success, string error) ForceRevertFunction(string game, object sender, RoutedEventArgs e)
        {
            int bytesWritten;
            switch (game)
            {

                case "H1CS":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "H1" && ValidCheck_H1())
                        {

                            byte[] buffer = { 1 };

                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H1_CoreLoad[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "failed to write core load byte");
                            }

                        }
                        else
                        {
                            return (false, "not attached to game");
                        }

                    }
                    catch (Exception ex)
                    {
                        return (false, "unknown error occured: " + ex.ToString());
                    }

                case "HRCP":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "HR" && ValidCheck_HR())
                        {
                            byte[] buffer = new byte[1] { 1 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_Revert[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("hr: made revert");
                                return (true, "success!");
                            }
                            else
                            {
                                Debug("hr: failed to make revert");
                                return (false, "failed to write revert byte");
                            }
                        }
                        else
                        {
                            return (false, "failed to force revert because not attached");
                        }

                    }
                    catch (Exception ex)
                    {
                        return (false, "unknown error occured: " + ex.ToString());
                    }

                default:
                    return (false, "invalid game");

            }

        }

        private void DoubleRevertButton_Click(object sender, RoutedEventArgs e)
        {
            //figure out which game this was for
            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            parent = (FrameworkElement)((FrameworkElement)parent).Parent;
            string parent_name = parent.Name; //have to go up two steps cos reasons

            //do double reverty things
            var (success, error) = DoubleRevertFunction(parent_name, sender, e);

            if (success == false)
            {
                Debug("Failed to double revert, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to double revert, " + error, "Error", System.Windows.MessageBoxButton.OK);
                return;
            }

            RefreshLoa(sender, e);

            (success, error) = ForceRevertFunction(parent_name, sender, e);

            if (success == false)
            {
                Debug("Failed to revert, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to revert, " + error, "Error", System.Windows.MessageBoxButton.OK);
                return;
            }



            
        }

        private (bool success, string error) DoubleRevertFunction(string game, object sender, RoutedEventArgs e)
        {
            int bytesWritten;
            switch (game)
            {

                case "HRCP":

                    try
                    {
                        if (HCMGlobal.AttachedGame == "HR" && ValidCheck_HR())
                        {
                            byte[] buffer = new byte[1];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("hr: read double revert flag");
                            }
                            else
                            {
                                return (false, "failed to read double revert flag");
                            }

                            buffer[0] = Convert.ToByte(!(BitConverter.ToBoolean(buffer, 0))); //just flip the value lmao
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("hr: made double revert");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "hr: failed to make double revert");
                            }

                        }
                        else
                        {
                            return (false, "failed to make double revert because not attached");
                        }
                    }
                    catch (Exception ex)
                    {
                        return (false, "unknown error occured: " + ex.ToString());
                    }

                default:
                    return (false, "invalid game");

            }

        }

        private void DumpButton_Click(object sender, RoutedEventArgs e)
        {

            //figure out which game this was for
            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            RefreshLoa(sender, e);
            var (success, error) = DumpFunction(parent_name, sender, e);

            if (error == "cancelclick")
                return;

            if (success == false)
            {
                Debug("Failed to dump, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to dump, " + error, "Error", System.Windows.MessageBoxButton.OK);
                return;
            }

            RefreshList(sender, e);

        }

        private (bool success, string error) DumpFunction(string game, object sender, RoutedEventArgs e)

        {

            switch (game)
            {

                case "H1CS":
                    try
                    {
                        string backuploc = "";
                        string pathtotest = "";

                        backuploc = HCMGlobal.H1CoreSavePath;
                        if (HCMGlobal.SavedConfig.CoreFolderPath != null)
                        {
                            pathtotest = HCMGlobal.SavedConfig.CoreFolderPath + @"\core.bin";
                        }
                        else
                        {
                            return (false, "core folder path not set, check settings!");
                        }

                        if (!File.Exists(pathtotest))
                            return (false, "core.bin at corefolderpath didn't exist!");

                        if (!Directory.Exists(backuploc))
                            return (false, "local core saves folder didn't exist");

                        if (pathtotest == "")
                            return (false, "corebin path invalid somehow"); //might remove this, doesn't seem reachable

                        var userinput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                                           "Name your backup save",
                                           "",
                                           -1, -1);
                        string proposedsave = (backuploc + @"\" + userinput + @".bin");
                        if (userinput == "")
                            return (false, "cancelclick");

                        try
                        {
                            File.Copy(pathtotest, proposedsave);
                            return (true, "success!");
                        }
                        catch (Exception exp)
                        {
                            return (false, "failed to copy the file! possible invalid filename? " + proposedsave);
                        }

                    }
                    catch (Exception ex)
                    {
                        return (false, "unknown error occured: " + ex.ToString());
                    }

                case "HRCP":
                    try
                    {

                        if (HCMGlobal.AttachedGame == "HR" && ValidCheck_HR())
                        {

                            int bytesWritten;
                            bool DRflag;
                            byte[] DRbuffer = new byte[1];

                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                            {
                                DRflag = Convert.ToBoolean(DRbuffer[0]);
                            }
                            else
                            {
                                Debug("something went wrong trying to read DR flag for reach dumping");
                                return (false, "Failed to read double-revert byte");
                            }

                            byte[] buffer = new byte[10551296];
                            int offset;
                            if (!DRflag)
                            {
                                offset = -0xA10000; //first cp
                            }
                            else
                            {
                                offset = 0x0; //second cp
                            }

                            int[] addy = HCMGlobal.LoadedOffsets.HR_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                            addy[3] = offset;
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy), buffer, buffer.Length, out bytesWritten))
                            {

                                string backuploc = HCMGlobal.HRCheckpointPath;

                                if (Directory.Exists(backuploc))
                                {
                                    var userinput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                                                       "Name your backup save",
                                                       "",
                                                       -1, -1);
                                    string proposedsave = (backuploc + @"\" + userinput + @".bin");
                                    if (userinput == "")
                                        return (false, "cancelclick");
                                    try
                                    {
                                        File.WriteAllBytes(proposedsave, buffer);
                                        FileInfo test = new FileInfo(proposedsave);
                                        if (File.Exists(test.ToString()) && test.Length > 1000)
                                        {
                                            Debug("SUCESSFULLY DUMPED HR CP, LENGTH: " + test.Length.ToString());
                                        }
                                    }
                                    catch
                                    {
                                        Debug("something went wrong trying to save a save: " + game + ", " + proposedsave);
                                        return (false, "couldn't write save file to disk, did you have an invalid name? proposed save: " + proposedsave);
                                        //need to make this a popup to let user know what was bad
                                    }
                                }
                                else
                                {
                                    return (false, "back up directory didn't exist: " + backuploc);
                                }
                            }
                            else
                            {
                                return (false, "failed to read checkpoint from memory");
                            }

                            return (true, "success!");

                        }
                        else
                        {
                            return (false, "not attached to game");

                        }
                    }
                    catch (Exception ex)
                    {
                        return (false, "unknown error occured: " + ex.ToString());
                    }

                default:
                    return (false, "invalid game");

            }
        }

        private void InjectButton_Click(object sender, RoutedEventArgs e)
        {
            //figure out which game this was for
            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            //do inject things
            RefreshSel(sender, e);
            var (success, error) = InjectFunction(parent_name, sender, e);

            if (success == false)
            {
                Debug("Failed to inject save, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to inject save, " + error, "Error", System.Windows.MessageBoxButton.OK);
                return;
            }
            RefreshLoa(sender, e);

        }

        private (bool success, string error) InjectFunction(string game, object sender, RoutedEventArgs e)
        {

            switch (game)
            {

                case "H1CS":
                    try
                    {
                        Debug("attempting to inject h1cs");

                        if (CS_MainList.SelectedItem == null)
                        {
                            return (false, "no save file selected!");
                        }

                        var item = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        string sourcePath = HCMGlobal.H1CoreSavePath + @"\" + item.Name + @".bin";
                        string targetPath = HCMGlobal.SavedConfig.CoreFolderPath + @"\core.bin";
                        string targetFolder = "";

                        try
                        {
                            targetFolder = System.IO.Path.GetDirectoryName(targetPath);
                        }
                        catch
                        {
                            return (false, "couldn't navigate up from core.bin");
                        }

                        if (!Directory.Exists(targetFolder))
                            return (false, "folder above core.bin didn't exist");

                        if (!File.Exists(sourcePath))
                            return (false, "selected save file somehow didn't actually exist");

                        try
                        {
                            File.Copy(sourcePath, targetPath, true);
                            return (true, "success!");
                        }
                        catch (Exception exp)
                        {
                            return (false, "failed to copy the save file");
                        }

                    }
                    catch (Exception ex)
                    {
                        return (false, "unknown error occured: " + ex.ToString());
                    }

                case "HRCP":
                    try
                    {
                        Debug("attempting to inject reach checkpoint");
                        if (HRCP_MainList.SelectedItem == null)
                        {
                            return (false, "no save file selected!");
                        }

                        if (HCMGlobal.AttachedGame == "HR" && ValidCheck_HR())
                        {
                            var item = HRCP_MainList.Items.GetItemAt(HRCP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                            string sourcePath = HCMGlobal.HRCheckpointPath + @"\" + item.Name + @".bin";
                            byte[] buffer;
                            try
                            {
                                FileStream fs = new FileStream(sourcePath, FileMode.Open, FileAccess.Read);
                                // Create a byte array of file stream length
                                buffer = System.IO.File.ReadAllBytes(sourcePath);
                                //Read block of bytes from stream into the byte array
                                fs.Read(buffer, 0, System.Convert.ToInt32(fs.Length));
                                //Close the File Stream
                                fs.Close();
                                Console.WriteLine("nearly ready to inject, buffer length: " + buffer.Length.ToString());
                            }
                            catch
                            {
                                return (false, "failed to access save file");
                            }

                            //okay first we need to read from memory the bytes that we need to preserve and overwrite them in buffer
                            //let's check the DR flag first
                            int bytesWritten;
                            bool DRflag;
                            byte[] DRbuffer = new byte[1];

                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                            {
                                DRflag = Convert.ToBoolean(DRbuffer[0]);
                            }
                            else
                            {
                                return (false, "something went wrong trying to read DR flag for reach injecting");
                            }

                            int offset;
                            if (!DRflag)
                            {
                                offset = -0xA10000; //first cp
                            }
                            else
                            {
                                offset = 0x0; //second cp
                            }

                            int[] addy = HCMGlobal.LoadedOffsets.HR_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                            addy[3] = offset;

                            //setup a 2d array with the values we need to preserve (offset, length)
                            int[][] PreserveLocations = new int[][] { new int[] { 0x594A74, 4 }, new int[] { 0x58F4F0, 16 } };

                            foreach (int[] i in PreserveLocations)
                            {
                                byte[] tempbuffer = new byte[i[1]];
                                if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + i[0], tempbuffer, tempbuffer.Length, out bytesWritten))
                                {
                                    //overwrite the stored cp buffer with new vals
                                    Array.ConstrainedCopy(tempbuffer, 0, buffer, i[0], i[1]);
                                    Debug("successfully copied over buffer at " + i[0]);
                                }
                                else
                                {
                                    return (false, "failed reading current vals for reach injection");
                                }
                            }

                            //now to inject into memory
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("successfully injected reach cp, " + buffer.Length.ToString() + " bytes!");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "failed injecting reach cp");
                            }

                        }
                        else
                        {
                            return (false, "not attached to game");
                        }
                    }
                    catch (Exception ex)
                    {
                        return (false, "unknown error occured: " + ex.ToString());
                    }

                default:
                    return (false, "invalid game");

            }

        }
        private void InjectRevertButton_Click(object sender, RoutedEventArgs e)
        {
            //figure out which game this was for
            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            //do inject things
            var (success, error) = InjectFunction(parent_name, sender, e);

            if (success == false)
            {
                Debug("Failed to inject, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to inject, " + error, "Error", System.Windows.MessageBoxButton.OK);
                return;
            }
            RefreshLoa(sender, e);
            //dump
            (success, error) = ForceRevertFunction(parent_name, sender, e);

            if (success == false)
            {
                Debug("Failed to force load, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to force load, " + error, "Error", System.Windows.MessageBoxButton.OK);
            }

        }

        private void ForceCPDumpButton_Click(object sender, RoutedEventArgs e)
        {

            //figure out which game this was for
            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            //do force cp things, sleep 100ms, then do dump things
            var (success, error) = ForceCPFunction(parent_name, sender, e);

            if (success == false)
            {
                Debug("Failed to force save, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to force save, " + error, "Error", System.Windows.MessageBoxButton.OK);
                return;
            }
            Thread.Sleep(100);
            RefreshLoa(sender, e);
            //dump
            (success, error) = DumpFunction(parent_name, sender, e);

            if (error == "cancelclick")
                return;

            if (success == false)
            {
                Debug("Failed to dump, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to dump, " + error, "Error", System.Windows.MessageBoxButton.OK);
            }

        }

        private static void Debug(string text,
                        [CallerFilePath] string file = "",
                        [CallerMemberName] string member = "",
                        [CallerLineNumber] int line = 0)
        {
            Console.WriteLine("{0}_{1}({2}): {3}", System.IO.Path.GetFileName(file), member, line, text);
        }

        private void maintick(object sender, EventArgs e)
        {
            string game = HCMGlobal.AttachedGame;
            string level = HCMGlobal.AttachedLevel;

            //all the magic happens here
            try
            {
                dtClockTime_Tick(sender, e);
            }
            catch (Exception ex)
            {
                Log("unknown maintick error: " + ex.ToString());
            }

            RefreshLoa(sender, new RoutedEventArgs());

            if (game != HCMGlobal.AttachedGame)
            {

                SetEnabledUI(); //only change the ui if the attached game changed
                switch (HCMGlobal.AttachedGame)
                {

                    case "Mn":
                        GameLabel.Content = "Game: Menu";
                        LevelLabel.Content = "Level: None";
                        break;
                    default:
                    case "No":
                        GameLabel.Content = "Game: None";
                        LevelLabel.Content = "Level: None";
                        break;
                    case "H1":
                        GameLabel.Content = "Game: H1";
                        break;
                    case "H2":
                        GameLabel.Content = "Game: H2";
                        break;
                    case "H3":
                        GameLabel.Content = "Game: H3";
                        break;
                    case "OD":
                        GameLabel.Content = "Game: ODST";
                        break;
                    case "H4":
                        GameLabel.Content = "Game: H4";
                        break;

                    case "HR":
                        GameLabel.Content = "Game: Reach";
                        break;

                }
            }

            if (level != HCMGlobal.AttachedLevel)
            {
                LevelLabel.Content = "Level: " + HCMGlobal.AttachedLevel;
            }
        }

        private void dtClockTime_Tick(object sender, EventArgs e)
        {

            /* PLAN:::
            check if process exists
            check if our global process id matches it's id
            if so do nothing (we're attached) (well actually proceed to next checks but eh)
            if not (but process exists)
            then try opening it and setting global process id etc
            catch - failed to open - popup warning about needing admin priv + mcc eac off. then do nothing (not attached)
            if process didn't exist
            do nothing (not attached)

            during all this we need to set a steam vs winstore flag

            then; check game version (do we have the offsets to support it?)
            check gameindicator and menuindicator to check whether we're in game (and which game) vs menu
                then disable the ui functions related to the games that we're not in (the ones that need attachment anyway, like dump/inject)

            also we're gonna need to set some sort of "busy flag" that's enabled when injections/dumps are happening that tells all this to not happen
             */

            //if busy, don't do things since it'll cause access errors
            if (HCMGlobal.BusyFlag || HCMGlobal.GiveUpFlag)
                return;

            //first, we'll have a global process ID that we check to see if we're already attached

            //string AttachedGame = "No"; //temp var, will modify this as we go through the checks
            IntPtr processHandle;
            Process myProcess;

            try
            {
                myProcess = Process.GetProcessById(HCMGlobal.ProcessID); //todo; think about this - https://stackoverflow.com/questions/1545270/how-to-determine-if-a-process-id-exists
                if (myProcess.ProcessName == "MCC-Win64-Shipping")
                {
                    Debug("MCC already attached at " + (Convert.ToString(HCMGlobal.ProcessID, 16)).ToUpper());
                    HCMGlobal.WinFlag = false;
                }
                else if (myProcess.ProcessName == "MCC-Win64-Shipping-Winstore")
                {
                    Debug("MCC already attached at " + (Convert.ToString(HCMGlobal.ProcessID, 16)).ToUpper());
                    HCMGlobal.WinFlag = true;
                }
                else
                {
                    throw new Exception("using exceptions as control flow is a terrible idea, they said");
                    //but we need to go down and reattach to the process now
                }

            }
            catch
            {
                try
                {
                    myProcess = Process.GetProcessesByName("MCC-Win64-Shipping")[0]; //could add a check here that the array isn't null, and if so to uh.. throw our own exception to continue control flow ree
                    processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, myProcess.Id);
                    HCMGlobal.ProcessID = myProcess.Id;
                    HCMGlobal.GlobalProcessHandle = processHandle;
                    Debug("MCC found with ID " + (Convert.ToString(myProcess.Id, 16)).ToUpper());
                    HCMGlobal.WinFlag = false;
                }
                catch
                {
                    try
                    {
                        myProcess = Process.GetProcessesByName("MCC-Win64-Shipping-WinStore")[0];
                        processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, myProcess.Id);
                        HCMGlobal.ProcessID = myProcess.Id;
                        HCMGlobal.GlobalProcessHandle = processHandle;
                        Debug("MCC found with ID " + (Convert.ToString(myProcess.Id, 16)).ToUpper());
                        HCMGlobal.WinFlag = true;
                    }
                    catch
                    {
                        Debug("MCC not found");
                        HCMGlobal.AttachedGame = "No";
                        SetEnabledUI();
                        return;
                    }
                }
            }

            //now that we're attached (we would've returned if we hadn't), we can proceed to next checks
            //what mcc version are we? only need to check this once
            if (!HCMGlobal.VersionCheckedFlag)
            {
                try
                {
                    HCMGlobal.MCCversion = myProcess.MainModule.FileVersionInfo.ProductVersion.ToString();
                    Debug("MCC version: " + HCMGlobal.MCCversion + ", WinFlag: " + HCMGlobal.WinFlag.ToString());
                    HCMGlobal.VersionCheckedFlag = true;

                }

                catch
                {
                    Debug("MCC - failed to find version info");
                    HCMGlobal.AttachedGame = "No";
                    SetEnabledUI();
                    return;
                }
            }


            if (!HCMGlobal.OffsetsAcquired)
            {
                bool failedlocalcheck = true;
                //now need to check if we have the offsets for this version in our local /offsets/ folder
                if (File.Exists(HCMGlobal.OffsetsPath + HCMGlobal.MCCversion + ".json"))
                {
                    //need to unpack the json and setup our offsets..
                    //gotta learn how json works lmao
                    Debug("file exists! unpacking json");
                    using (StreamReader r = new StreamReader(HCMGlobal.OffsetsPath + HCMGlobal.MCCversion + ".json"))
                    {
                        string json = r.ReadToEnd();
                        r.Close();
                        try
                        {
                            HCMGlobal.LoadedOffsets = JsonConvert.DeserializeObject<Offsets>(json);
                        }
                        catch
                        {
                            Debug("something went horrifically wrong trying to load local json file");
                            HCMGlobal.AttachedGame = "No";
                            SetEnabledUI();
                            HCMGlobal.GiveUpFlag = true;

                            if (File.Exists("offsets\\" + HCMGlobal.MCCversion + ".json"))
                            {
                                try
                                {
                                    File.Delete("offsets\\" + HCMGlobal.MCCversion + ".json");
                                    Debug("deleted local json file since it was bunk");
                                }
                                catch
                                {
                                    Debug("failed to delete bunk local json file");
                                }
                            }
                            return;

                        }
                        //Debug("json string: " + json);
                    }

                    if (HCMGlobal.LoadedOffsets.gameindicator == null)
                    {
                        Debug("something went horribly wrong loading the json");
                    }
                    else
                    {
                        Debug("gi: " + HCMGlobal.LoadedOffsets.gameindicator.ToString());
                        Debug("gi00: " + HCMGlobal.LoadedOffsets.gameindicator[0][0].ToString());
                        Debug("gi01: " + HCMGlobal.LoadedOffsets.gameindicator[0][1].ToString());
                        Debug("gi10: " + HCMGlobal.LoadedOffsets.gameindicator[1][0].ToString());
                        Debug("gi11: " + HCMGlobal.LoadedOffsets.gameindicator[1][1].ToString());
                        failedlocalcheck = false;
                        //if (HCMGlobal.SavedConfig.CoreFolderPath == null)
                    }
                }
                //if no offsets found, let's check the github and try to download them!
                if (!HCMGlobal.CheckedForOnlineOffsets && failedlocalcheck)
                {
                    HCMGlobal.CheckedForOnlineOffsets = true;
                    try
                    {
                        String url = "https://raw.githubusercontent.com/Burnt-o/HaloCheckpointManager/master/WpfApp3/offsets/" + HCMGlobal.MCCversion + ".json";
                        System.Net.WebClient client = new System.Net.WebClient();
                        String json = client.DownloadString(url);
                        System.IO.File.WriteAllText("offsets\\" + HCMGlobal.MCCversion + ".json", json);

                        Debug("downloaded json file!" + json);

                        //copy pasted code from above
                        if (File.Exists(HCMGlobal.OffsetsPath + HCMGlobal.MCCversion + ".json"))
                        {
                            //need to unpack the json and setup our offsets..
                            //gotta learn how json works lmao
                            Debug("file exists! unpacking json");
                            using (StreamReader r = new StreamReader(HCMGlobal.OffsetsPath + HCMGlobal.MCCversion + ".json"))
                            {
                                string json2 = r.ReadToEnd();
                                r.Close();
                                try
                                {
                                    HCMGlobal.LoadedOffsets = JsonConvert.DeserializeObject<Offsets>(json2);
                                }
                                catch
                                {
                                    Debug("something went horrifically wrong trying to load downloaded local json file");
                                    HCMGlobal.AttachedGame = "No";
                                    SetEnabledUI();
                                    HCMGlobal.GiveUpFlag = true;

                                    if (File.Exists("offsets\\" + HCMGlobal.MCCversion + ".json"))
                                    {
                                        try
                                        {
                                            File.Delete("offsets\\" + HCMGlobal.MCCversion + ".json");
                                            Debug("deleted downloaded local json file since it was bunk");
                                        }
                                        catch
                                        {
                                            Debug("failed to delete bunk downloaded local json file");
                                        }
                                    }
                                    return;

                                }
                                //Debug("json string: " + json);
                            }

                            if (HCMGlobal.LoadedOffsets.gameindicator == null)
                            {
                                Debug("something went horribly wrong loading the json");
                            }
                            else
                            {
                                Debug("gi: " + HCMGlobal.LoadedOffsets.gameindicator.ToString());
                                Debug("gi00: " + HCMGlobal.LoadedOffsets.gameindicator[0][0].ToString());
                                Debug("gi01: " + HCMGlobal.LoadedOffsets.gameindicator[0][1].ToString());
                                Debug("gi10: " + HCMGlobal.LoadedOffsets.gameindicator[1][0].ToString());
                                Debug("gi11: " + HCMGlobal.LoadedOffsets.gameindicator[1][1].ToString());
                                //if (HCMGlobal.SavedConfig.CoreFolderPath == null)
                            }
                        }

                    }
                    catch
                    {
                        //failed to find/download json!
                        Debug("online json file didn't exist!");
                    }

                    //nah
                }
                //check that offsets aren't null
                try
                {
                    if (HCMGlobal.LoadedOffsets.gameindicator[0][0] > 0)
                    {
                        //success!
                        Debug("SUCCESS");
                        HCMGlobal.OffsetsAcquired = true;
                    }
                    else
                    {
                        throw new Exception("using exceptions as control flow is a terrible idea, they said - part 2 electric boogaloo");
                    }
                }
                catch
                {
                    Debug("failed to find offsets for current MCC version! Need to popup a user dialog to let em know");
                    HCMGlobal.AttachedGame = "No";
                    SetEnabledUI();
                    HCMGlobal.GiveUpFlag = true;

                    if (HCMGlobal.CheckedForOnlineOffsets && File.Exists("offsets\\" + HCMGlobal.MCCversion + ".json"))
                    {
                        try
                        {
                            File.Delete("offsets\\" + HCMGlobal.MCCversion + ".json");
                            Debug("deleted downloaded json file since it was bunk");
                        }
                        catch
                        {
                            Debug("failed to delete bunk downloaded json file");
                        }
                    }
                    return;
                }
            }

            //OKAY we need to get the base address of mcc.exe
            ProcessModule myProcessModule;
            ProcessModuleCollection myProcessModuleCollection = myProcess.Modules;

            for (int i = 0; i < myProcessModuleCollection.Count; i++)
            {
                myProcessModule = myProcessModuleCollection[i];

                switch (myProcessModule.ModuleName)
                {
                    case "MCC-Win64-Shipping.exe":
                        HCMGlobal.BaseAddress = myProcessModule.BaseAddress;
                        break;

                    case "MCC-Win64-Shipping-Winstore.exe":
                        HCMGlobal.BaseAddress = myProcessModule.BaseAddress;
                        break;

                    default:
                        break;

                }
            }

            if (HCMGlobal.BaseAddress == null)
            {
                Debug("How did execution even get to here? Something went really wrong");
                return;
            }

            //OKAY we're attached, and we have our offsets (or we would've returned by now)
            //let's check whether we're in game (as opposed to menu), and which game we're in
            byte[] buffer = new byte[1];

            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.gameindicator[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out int bytesRead2))
            {
                switch (buffer[0])
                {
                    case 0:
                        HCMGlobal.AttachedGame = "H1";
                        break;
                    case 1:
                        HCMGlobal.AttachedGame = "H2";
                        break;
                    case 2:
                        HCMGlobal.AttachedGame = "H3";
                        break;
                    case 3:
                        HCMGlobal.AttachedGame = "Mn"; //halo 4 maybe?
                        break;
                    case 4:
                        HCMGlobal.AttachedGame = "Mn"; //halo 4 maybe?
                        break;
                    case 5:
                        HCMGlobal.AttachedGame = "OD";
                        break;
                    case 6:
                        HCMGlobal.AttachedGame = "HR";
                        break;
                    default:
                        HCMGlobal.AttachedGame = "Mn";
                        break;

                }
            }
            else //means reading the gameindicator failed
            {
                Debug("failed to read game indicator");
                return;
            }

            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.menuindicator[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out int bytesRead))
            {
                Debug("menu indicator is: " + buffer[0]);
                if (buffer[0] == 0x0B)
                    HCMGlobal.AttachedGame = "Mn";

            }
            else
            {
                Debug("failed to read menu indicator");
                return;
            }

            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.stateindicator[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesRead))
            {
                Debug("state indicator is: " + buffer[0]);
                if (buffer[0] == 0x44 || buffer[0] == 57)
                    HCMGlobal.AttachedGame = "Mn";

            }
            else
            {
                Debug("failed to read state indicator");
                return;
            }


            Debug("All checks succeeded, attached game is: " + HCMGlobal.AttachedGame);
            //is that it? I think we're done

            //NEXT, level check!
            buffer = new byte[32];
            string holdstring;
            switch (HCMGlobal.AttachedGame)
            {
                case "H2":
                case "H3":
                case "OD":
                case "H4"://these above games are not supported yet

                default:
                case "Mn":
                case "No":
                    HCMGlobal.AttachedLevel = null;
                    break;

                case "H1":
                    if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H1_LevelName[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesRead))
                    {
                        holdstring = Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                        holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                        holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                        char[] exceptions = new char[] { '_' };
                        holdstring = String.Concat(holdstring.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                        Debug("read h1 level: " + holdstring);
                        HCMGlobal.AttachedLevel = holdstring;
                    }
                    else
                    {
                        Debug("failed to read h1 level");
                        HCMGlobal.AttachedLevel = null;
                    }
                    break;

                case "HR":
                    if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_LevelName[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesRead))
                    {
                        holdstring = Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                        holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                        holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                        char[] exceptions = new char[] { '_' };
                        holdstring = String.Concat(holdstring.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                        Debug("read hr level: " + holdstring);
                        HCMGlobal.AttachedLevel = holdstring;
                    }
                    else
                    {
                        Debug("failed to read hr level");
                        HCMGlobal.AttachedLevel = null;
                    }
                    break;

            }


            //if game is reach & tab is reach, then do a seed check
            if (TabList.SelectedIndex == 3 && HCMGlobal.AttachedGame == "HR" && ValidCheck_HR())
            {
                buffer = new byte[4];
                uint seedint;
                string seedstring;
                if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_LoadedSeed[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesRead))
                {
                    seedint = BitConverter.ToUInt32(buffer, 0);
                    seedstring = (Convert.ToString(seedint, 16)).ToUpper();
                    ReachLoadedSeed.Content = "Loaded Seed: " + seedstring;
                }
                else
                {
                    Debug("failed to read reach seed");
                    ReachLoadedSeed.Content = "Loaded Seed: N/A";
                }


            }
            else
                ReachLoadedSeed.Content = "Loaded Seed: N/A";

        }

        private void SetEnabledUI()
        {
            //here we'll set certain UI elements to be enabled/disabled based on what we're attached too
            //eg if we're in H1, we disable the inject/dump buttons for reach (& force checkpoint/revert)
            //or if we're in menu/not attached, we disable those buttons for all games
            //we'll just use the xaml .IsEnabled flag, when false this greys out the button and makes it unclickable

            //todo; implement this
            switch (HCMGlobal.AttachedGame)
            {
                case "H2":
                case "H3":
                case "OD":
                case "H4"://these above games are not supported yet

                default:
                case "Mn":
                case "No":
                    SetH1(false);
                    SetHR(false);
                    break;

                case "H1":
                    SetH1(HCMGlobal.SavedConfig.CoreFolderPath != null); //false if corefolderpath not set correctly
                    SetHR(false);
                    break;

                case "HR":
                    SetH1(false);
                    SetHR(true);
                    break;

            }

            try
            {
                if (HCMGlobal.SavedConfig.CoreFolderPath == null)
                {
                    SetH1(false);
                }
            }
            catch { SetH1(false); }

            void SetH1(bool state)
            {
                H1CS_ForceCheckpoint.IsEnabled = state;
                H1CS_ForceRevert.IsEnabled = state;
                H1CS_Loa_ForceCPDump.IsEnabled = state;
                H1CS_Sel_InjectRevertButton.IsEnabled = state;

                if (state == false && (!HCMGlobal.WinFlag || HCMGlobal.AttachedGame == "no"))
                {
                    H1CS_Loa_DumpButton.IsEnabled = true;
                    H1CS_Sel_InjectButton.IsEnabled = true;
                }
                else if (state == false)
                {
                    H1CS_Loa_DumpButton.IsEnabled = false;
                    H1CS_Sel_InjectButton.IsEnabled = false;
                }
                else
                {
                    H1CS_Loa_DumpButton.IsEnabled = true;
                    H1CS_Sel_InjectButton.IsEnabled = true;
                }
            }

            void SetHR(bool state)
            {
                HRCP_ForceCheckpoint.IsEnabled = state;
                HRCP_ForceRevert.IsEnabled = state;
                HRCP_ForceDR.IsEnabled = state;
                HRCP_Loa_DumpButton.IsEnabled = state;
                HRCP_Loa_ForceCPDump.IsEnabled = state;
                HRCP_Sel_InjectButton.IsEnabled = state;
                HRCP_Sel_InjectRevertButton.IsEnabled = state;
            }



        }

        public static IntPtr FindPointerAddy(IntPtr hProc, IntPtr ptr, int[] offsets)
        {
            var buffer = new byte[IntPtr.Size];

            ptr = ptr + offsets[0];
            if (offsets.Length == 1)
            {
                return ptr;
            }

            offsets = offsets.Skip(1).ToArray();

            foreach (int i in offsets)
            {
                ReadProcessMemory(hProc, ptr, buffer, buffer.Length, out int read);

                ptr = (IntPtr.Size == 4)
                ? IntPtr.Add(new IntPtr(BitConverter.ToInt32(buffer, 0)), i)
                : ptr = IntPtr.Add(new IntPtr(BitConverter.ToInt64(buffer, 0)), i);
            }
            return ptr;
        }

        private static bool ValidCheck_H1()
        {

            try
            {
                byte[] buffer = new byte[6];
                if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H1_CheckString[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out int bytesRead2))
                {
                    if (Encoding.UTF8.GetString(buffer, 0, buffer.Length) == "levels")
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            catch
            {
                return false;
            }

        }

        private static bool ValidCheck_HR()
        {

            try
            {
                byte[] buffer = new byte[4];
                if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_CheckString[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out int bytesRead2))
                {
                    if (Encoding.UTF8.GetString(buffer, 0, buffer.Length) == "maps")
                    {
                        Debug("reach check success");
                        return true;
                    }
                    else
                    {
                        Debug("oh no");
                        return false;
                    }
                }
                else
                {
                    Debug("oh no");
                    return false;
                }
            }
            catch
            {
                Debug("oh no");
                return false;
            }

        }

    }
}
