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
        public static extern bool ReadProcessMemory(int hProcess,
            Int64 lpBaseAddress, byte[] lpBuffer, int dwSize, ref int lpNumberOfBytesRead);
        const int PROCESS_WM_READ = 0x0010;


        private class HCMConfig
        {
            public string CoreFolderPath;
            public string CheckpointFolderPath;
            public bool ClassicMode = true;
        }

        private static class HCMGlobal
        {
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

            public static string ImageModeSuffix => SavedConfig.ClassicMode ? "clas" : "anni";

            public static bool padowomode = false;

            public static Int32 ProcessID;
            public static IntPtr GlobalProcessHandle;

            public static bool SteamFlag = true; //false == winstore, used for knowing which offsets to use
            public static bool BusyFlag = false; //turned true when injecting/dumping so we don't do it twice
            public static string AttachedGame = "No"; //No, Mn (menu), HR, H1, H2, H3, OD (ODST), H4
            public static bool VersionCheckedFlag = false;
            public static string MCCversion;
        }


        private class Offsets
        {
            //offsets are gonna be stored as 2-unit arrays, first position is winstore, second is steam
            //that way when we're calling them from elsewhere we can just call Offsets.WhateverOffset[HCMGlobal.SteamFlag] and it'll give us the one we want
            //the units will themselves be arbitary length arrays (each position for each offset in a multi-level pointer)

            //the actual values will be populated from the json file corrosponding to the attached mcc version

                //general
            public int[][] gameindicator;
            public int[][] menuindicator;

             //h1
            public static int[][] H1_CoreSave = new int[2][];
            public static int[][] H1_CoreLoad = new int[2][];
            //need to 

                //hr
            public static int[][] HR_Checkpoint = new int[2][]; //for forcing checkpoints
            public static int[][] HR_Revert = new int[2][]; //for forcing reverts
            public static int[][] HR_CPLocation = new int[2][];
            public static int[][] HR_DRflag = new int[2][]; //dr as in "double revert"
            public static int[][] HR_StartSeed = new int[2][]; //seed of the level start - you get a different seed in reach every time you start the level from the main menu
            




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
        };

        public ObservableCollection<HaloSaveFileMetadata> Halo1CoreSaves { get; set; } = new ObservableCollection<HaloSaveFileMetadata>();
        public ObservableCollection<HaloSaveFileMetadata> Halo1Checkpoints { get; set; } = new ObservableCollection<HaloSaveFileMetadata>();
        public ObservableCollection<HaloSaveFileMetadata> Halo2Checkpoints { get; set; } = new ObservableCollection<HaloSaveFileMetadata>();

        public ObservableCollection<HaloSaveFileMetadata> HaloReachCheckpoints { get; set; } = new ObservableCollection<HaloSaveFileMetadata>();

        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;



            //need to initialize timer that will check for attachment to mcc process
            //wasn't sure whether to put this here or in the above function but whatever
            
            DispatcherTimer dtClockTime = new DispatcherTimer();
            dtClockTime.Interval = new TimeSpan(0, 0, 1); //in Hour, Minutes, Second.
            dtClockTime.Tick += dtClockTime_Tick;
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
                            ArbitaryFileTimeMove(CS_MainList.SelectedIndex, CS_MainList.Items.Count -1, CS_MainList, HCMGlobal.H1CoreSavePath);
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

        private void H1CSDumpButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshLoa(sender, e);



            string backuploc = "";
            string pathtotest = "";


                    backuploc = HCMGlobal.H1CoreSavePath;
                    if (HCMGlobal.SavedConfig.CoreFolderPath != null)
                        pathtotest = HCMGlobal.SavedConfig.CoreFolderPath + @"\core.bin";



            if (File.Exists(pathtotest) && Directory.Exists(backuploc) && pathtotest != "")
            {
                var userinput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                                   "Name your backup save",
                                   "",
                                   -1, -1);
                string proposedsave = (backuploc + @"\" + userinput + @".bin");

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


        private void H1CSForceCPDumpButton_Click(object sender, RoutedEventArgs e)
        {
            //do force cp things, set a timer to wait for checkpoint to complete, then..
            H1CSDumpButton_Click(sender, e);

        }

        private void CopySaveFile(string sourcePath, string targetPath)
        {
            Debug("hm yes");
            string targetFolder = "";
            try
            {
                targetFolder = System.IO.Path.GetDirectoryName(targetPath);
            }
            catch {
                Debug("that's not good");
            }
            if (Directory.Exists(targetFolder) && File.Exists(sourcePath))
            {
                try
                {
                    File.Copy(sourcePath, targetPath, true);
                }
                catch (Exception exp)
                {
                    Log("something went wrong trying to load a save: " + exp);
                }
            }
            else
            {
                //need to make this a popup to let user know what was bad
                Log("something went wrong trying to load a save: (sourcePath) : " + sourcePath);
                Log("something went wrong trying to load a save: (targetPath) : " + targetPath);
            }
        }

        private void H1CSInjectButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);
            RefreshLoa(sender, e);
            Debug("attempting to inject h1cs");


                    if (CS_MainList.SelectedItem != null)
                    {
                        Debug("uh huh");
                        var item = CS_MainList.Items.GetItemAt(CS_MainList.SelectedIndex) as HaloSaveFileMetadata;
                        string sourcePath = HCMGlobal.H1CoreSavePath + @"\" + item.Name + @".bin";
                        string targetPath = HCMGlobal.SavedConfig.CoreFolderPath + @"\core.bin";
                        CopySaveFile(sourcePath, targetPath);
                    }


            RefreshLoa(sender, e);
        }

        private void H1CSInjectRevertButton_Click(object sender, RoutedEventArgs e)
        {
            H1CSInjectButton_Click(sender, e);
            //then do force revert things
        
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
            //H1 CORES FIRST
            if (HCMGlobal.SavedConfig != null && File.Exists(HCMGlobal.SavedConfig.CoreFolderPath + @"\core.bin") && HCMGlobal.SavedConfig.CoreFolderPath != null)
            {
                var data = GetSaveFileMetadata(HCMGlobal.SavedConfig.CoreFolderPath + @"\core.bin", HaloGame.Halo1);
                H1CS_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode);

                if(data.Difficulty != Difficulty.Invalid)
                    H1CS_Loa_DiffName.Source = new BitmapImage(new Uri($"images/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                H1CS_Loa_Time.Text = TickToTimeString(data.StartTick);
                H1CS_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
            }
            else
            {
                H1CS_Loa_LevelName.Text = "N/A";
                H1CS_Loa_DiffName.Source = null;
                H1CS_Loa_Time.Text = "N/A";
                H1CS_Loa_LevelImage.Source = null;
            }

            //H1 CPs SECOND
            //if (HCMGlobal.SavedConfig != null && File.Exists(HCMGlobal.SavedConfig.CheckpointFolderPath + @"\autosave_Halo1.bin") && HCMGlobal.SavedConfig.CheckpointFolderPath != null)
            //{
            //    var data = GetSaveFileMetadata(HCMGlobal.SavedConfig.CheckpointFolderPath + @"\autosave_Halo1.bin", HaloGame.Halo1);
            //    CP_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode);

            //    if (data.Difficulty != Difficulty.Invalid)
            //        CP_Loa_DiffName.Source = new BitmapImage(new Uri($"images/diff_{(int)data.Difficulty}.png", UriKind.Relative));

            //    CP_Loa_Time.Text = TickToTimeString(data.StartTick);
            //    CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
            //}
            //else
            //{
            //    CP_Loa_LevelName.Text = "N/A";
            //    CP_Loa_DiffName.Source = null;
            //    CP_Loa_Time.Text = "N/A";
            //    CP_Loa_LevelImage.Source = null;
            //}

            //H2 CPs THIRD
            //if (HCMGlobal.SavedConfig != null && File.Exists(HCMGlobal.SavedConfig.CheckpointFolderPath + @"\autosave_Halo2.bin") && HCMGlobal.SavedConfig.CheckpointFolderPath != null)
            //{
            //    var data = GetSaveFileMetadata(HCMGlobal.SavedConfig.CheckpointFolderPath + @"\autosave_Halo2.bin", HaloGame.Halo2);
            //    H2CP_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode);

            //    if (data.Difficulty != Difficulty.Invalid)
            //        H2CP_Loa_DiffName.Source = new BitmapImage(new Uri($"images/diff_-1.png", UriKind.Relative));

            //    H2CP_Loa_Time.Text = TickToTimeString(data.StartTick);
            //    H2CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
            //}
            //else
            //{
            //    H2CP_Loa_LevelName.Text = "N/A";
            //    H2CP_Loa_DiffName.Source = null;
            //    H2CP_Loa_Time.Text = "N/A";
            //    H2CP_Loa_LevelImage.Source = null;
            //}
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
                    var data = GetSaveFileMetadata(pathtotest, HaloGame.Halo1);
                    H1CS_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode);

                    if (data.Difficulty != Difficulty.Invalid)
                        H1CS_Sel_DiffName.Source = new BitmapImage(new Uri($"images/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                    H1CS_Sel_Time.Text = TickToTimeString(data.StartTick);
                    H1CS_Sel_FileName.Text = s;
                    H1CS_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                }
            }
            else
            {
                H1CS_Sel_LevelName.Text = "N/A";
                H1CS_Sel_DiffName.Source = null;
                H1CS_Sel_Time.Text = "N/A";
                H1CS_Sel_FileName.Text = "N/A";
                H1CS_Sel_LevelImage.Source = null;
            }
            //Debug("CP_MainList.SelectedItem" + CP_MainList?.SelectedItem?.ToString());

            //H1 CPs SECOND
            //if (CP_MainList.SelectedItem != null)
            //{
            //    //Debug("ahh" + CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex).ToString());
            //    var item = CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex);
            //    System.Type type = item.GetType();
            //    string s = (string)type.GetProperty("Name").GetValue(item, null);
            //    var pathtotest = HCMGlobal.H1CheckpointPath + @"\" + s + @".bin";

            //    if (File.Exists(pathtotest))
            //    {
            //        var data = GetSaveFileMetadata(pathtotest, HaloGame.Halo1);
            //        CP_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode);

            //        if (data.Difficulty != Difficulty.Invalid)
            //            CP_Sel_DiffName.Source = new BitmapImage(new Uri($"images/diff_{(int)data.Difficulty}.png", UriKind.Relative));

            //        CP_Sel_Time.Text = TickToTimeString(data.StartTick);
            //        CP_Sel_FileName.Text = s;
            //        CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
            //    }
            //}
            //else
            //{
            //    CP_Sel_LevelName.Text = "N/A";
            //    CP_Sel_DiffName.Source = null;
            //    CP_Sel_Time.Text = "N/A";
            //    CP_Sel_FileName.Text = "N/A";
            //    CP_Sel_LevelImage.Source = null;
            //}

            //H2 CPs THIRD
            //if (H2CP_MainList.SelectedItem != null)
            //{
            //    //Debug("ahh" + H2CP_MainList.Items.GetItemAt(CP_MainList.SelectedIndex).ToString());
            //    var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex);
            //    System.Type type = item.GetType();
            //    string s = (string)type.GetProperty("Name").GetValue(item, null);
            //    var pathtotest = HCMGlobal.H2CheckpointPath + @"\" + s + @".bin";

            //    if (File.Exists(pathtotest))
            //    {
            //        var data = GetSaveFileMetadata(pathtotest, HaloGame.Halo2);
            //        H2CP_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode);

            //        if (data.Difficulty != Difficulty.Invalid)
            //            H2CP_Sel_DiffName.Source = new BitmapImage(new Uri($"images/diff_{(int)data.Difficulty}.png", UriKind.Relative));

            //        H2CP_Sel_Time.Text = TickToTimeString(data.StartTick); //might need to halve this if h2 really is 60 ticks per sec
            //        H2CP_Sel_FileName.Text = s;
            //        H2CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
            //    }
            //}
            //else
            //{
            //    H2CP_Sel_LevelName.Text = "N/A";
            //    H2CP_Sel_DiffName.Source = null;
            //    H2CP_Sel_Time.Text = "N/A";
            //    H2CP_Sel_FileName.Text = "N/A";
            //    H2CP_Sel_LevelImage.Source = null;
            //}
        }

        private void RefreshList(object sender, RoutedEventArgs e)
        {
            var oldCSselected = CS_MainList.SelectedIndex;
            var oldCPselected = CP_MainList.SelectedIndex;
            var oldH2CPselected = H2CP_MainList.SelectedIndex;

            List<string> FilesPost = new List<string>();

            //h1 cores
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

                if(FilesPost.Count > 0)
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

            //h1 checkpoints second
            //Halo1Checkpoints.Clear();
            //if (Directory.Exists(HCMGlobal.H1CheckpointPath)) // make sure path is valid
            //{
            //    DirectoryInfo dir = new DirectoryInfo(HCMGlobal.H1CheckpointPath);
            //    FileInfo[] files = dir.GetFiles("*.bin").OrderByDescending(p => p.LastWriteTime).ToArray();
            //    FilesPost.Clear();
            //    foreach (FileInfo file in files)
            //    {
            //        while (FileHasSameTime(files, file))
            //        {
            //            file.LastWriteTime = file.LastWriteTime.AddSeconds(1);
            //        }
            //        FilesPost.Add(file.ToString());
            //    }

            //    if (FilesPost.Count > 0)
            //    {
            //        CP_MainList_Label.Content = "";
            //        foreach (string File in FilesPost)
            //        {
            //            var data = GetSaveFileMetadata(HCMGlobal.H1CheckpointPath + "/" + File, HaloGame.Halo1);
            //            data.Name = File.Substring(0, File.Length - 4);
            //            Halo1Checkpoints.Add(data);
            //        }
            //        CP_MainList.SelectedIndex = oldCPselected;
            //        GridView gv = CP_MainList.View as GridView;
            //        UpdateColumnWidths(gv);
            //    }
            //    else
            //    {
            //        CP_MainList_Label.Content = "No backup saves in local folder.";
            //    }
            //}
            //else
            //{
            //    CP_MainList_Label.Content = "Checkpoint folder path is invalid, check Settings.";
            //}

            //h2 checkpoints THIRD
            //Halo2Checkpoints.Clear();
            //if (Directory.Exists(HCMGlobal.H2CheckpointPath)) // make sure path is valid
            //{
            //    DirectoryInfo dir = new DirectoryInfo(HCMGlobal.H2CheckpointPath);
            //    FileInfo[] files = dir.GetFiles("*.bin").OrderByDescending(p => p.LastWriteTime).ToArray();
            //    FilesPost.Clear();
            //    foreach (FileInfo file in files)
            //    {
            //        while (FileHasSameTime(files, file))
            //        {
            //            file.LastWriteTime = file.LastWriteTime.AddSeconds(1);
            //        }
            //        FilesPost.Add(file.ToString());
            //    }

            //    if (FilesPost.Count > 0)
            //    {
            //        H2CP_MainList_Label.Content = "";
            //        foreach (string File in FilesPost)
            //        {
            //            var data = GetSaveFileMetadata(HCMGlobal.H2CheckpointPath + "/" + File, HaloGame.Halo2);
            //            data.Name = File.Substring(0, File.Length - 4);
            //            Halo2Checkpoints.Add(data);
            //        }

            //        H2CP_MainList.SelectedIndex = oldH2CPselected;
            //        GridView gv = H2CP_MainList.View as GridView;
            //        UpdateColumnWidths(gv);
            //    }
            //    else
            //    {
            //        H2CP_MainList_Label.Content = "No backup saves in local folder.";
            //    }
            //}
            //else
            //{
            //    H2CP_MainList_Label.Content = "Checkpoint folder path is invalid, check Settings.";
            //}
        }

        public class HaloSaveFileMetadata
        {
            public HaloGame Game { get; set; }
            public string LevelCode { get; set; } = "xxx";
            public uint StartTick { get; set; } = 0;
            public Difficulty Difficulty { get; set; } = Difficulty.Easy;
            public string Name { get; set; }

            public string DifficultyImage => $"images/diff_{(int)Difficulty}.png";
            public string TimeString => TickToTimeString(StartTick);
        }

        private HaloSaveFileMetadata GetSaveFileMetadata(string saveFilePath, HaloGame game)
        {
            int offsetLevelCode;
            int offsetStartTick;
            int offsetDifficulty;

            switch (game)
            {
                case HaloGame.Halo1:
                    offsetLevelCode = 11;
                    offsetStartTick = 756;
                    offsetDifficulty = 294;
                    break;
                case HaloGame.Halo2:
                    offsetLevelCode = 23;
                    offsetStartTick = 10808;
                    offsetDifficulty = 974;
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
                        metadata.LevelCode = new string(readBinary.ReadChars(3));

                        //get time
                        readBinary.BaseStream.Seek(offsetStartTick, SeekOrigin.Begin);
                        UInt32 holdme = readBinary.ReadUInt32();
                        if (game == HaloGame.Halo2)
                            metadata.StartTick = holdme / 2;
                        else
                            metadata.StartTick = holdme;

                        //get difficulty
                        readBinary.BaseStream.Seek(offsetDifficulty, SeekOrigin.Begin);
                        metadata.Difficulty = (Difficulty)readBinary.ReadByte();
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

        public static string TickToTimeString(uint ticks)
        {
            int seconds = (int)ticks / 30; 
            TimeSpan ts = new TimeSpan(seconds / (60 * 60), seconds / (60), seconds % 60);
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


        public static string ReadLevelFromMemory()
        {
            string test = "empty lol";

            /*Process process = Process.GetProcessesByName("MCC-Win64-Shipping")[0];
            IntPtr processHandle = OpenProcess(PROCESS_WM_READ, false, process.Id);

            int bytesRead = 0;
            byte[] buffer = new byte[3]; //3 bytes for levelcode string
            ReadProcessMemory((int)processHandle, 0x7FF7A5A6004E, buffer, buffer.Length, ref bytesRead);
            Debug(Encoding.ASCII.GetString(buffer) + " (" + bytesRead.ToString() + "bytes)");
*/

            return test;
        }

        private void DumpButton_Click(object sender, RoutedEventArgs e)
        {
            //figure out which game this was for
            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            switch (parent_name)
            {
                case "HRCP":
                    //do dumpy things
                    break;

                default:
                    break;
            }

        }

        private void InjectButton_Click(object sender, RoutedEventArgs e)
        {
            //figure out which game this was for
            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            switch (parent_name)
            {
                case "HRCP":
                    //do inject things
                    break;

                default:
                    break;
            }

        }

        private void InjectRevertButton_Click(object sender, RoutedEventArgs e)
        {
            //inject
            InjectButton_Click(sender, e);
            //then force revert
            //TBD
        
        }

        private void ForceCPDumpButton_Click(object sender, RoutedEventArgs e)
        {
            //do force cp things, set a timer to wait for checkpoint to complete, then..
            //dump
            DumpButton_Click(sender, e);

        }




        private static void Debug(string text,
                        [CallerFilePath] string file = "",
                        [CallerMemberName] string member = "",
                        [CallerLineNumber] int line = 0)
        {
            Console.WriteLine("{0}_{1}({2}): {3}", System.IO.Path.GetFileName(file), member, line, text);
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
            if (HCMGlobal.BusyFlag)
                return;


            //first, we'll have a global process ID that we check to see if we're already attached

            //string AttachedGame = "No"; //temp var, will modify this as we go through the checks
            IntPtr processHandle;
            Process myProcess;

            try
            {
                myProcess = Process.GetProcessById(HCMGlobal.ProcessID);
                if (myProcess.ProcessName == "MCC-Win64-Shipping")
                {
                    Debug("MCC already attached at " + (Convert.ToString(HCMGlobal.ProcessID, 16)).ToUpper());
                    HCMGlobal.SteamFlag = true;
                }
                else if (myProcess.ProcessName == "MCC-Win64-Shipping-Winstore")
                {
                    Debug("MCC already attached at " + (Convert.ToString(HCMGlobal.ProcessID, 16)).ToUpper());
                    HCMGlobal.SteamFlag = false;
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
                    myProcess = Process.GetProcessesByName("MCC-Win64-Shipping")[0];
                    processHandle = OpenProcess(PROCESS_WM_READ, false, myProcess.Id);
                    HCMGlobal.ProcessID = myProcess.Id;
                    HCMGlobal.GlobalProcessHandle = processHandle;
                    Debug("MCC found with ID " + (Convert.ToString(myProcess.Id, 16)).ToUpper());
                    HCMGlobal.SteamFlag = true;
                }
                catch
                {
                    try
                    {
                        myProcess = Process.GetProcessesByName("MCC-Win64-Shipping-WinStore")[0];
                        processHandle = OpenProcess(PROCESS_WM_READ, false, myProcess.Id);
                        HCMGlobal.ProcessID = myProcess.Id;
                        HCMGlobal.GlobalProcessHandle = processHandle;
                        Debug("MCC found with ID " + (Convert.ToString(myProcess.Id, 16)).ToUpper());
                        HCMGlobal.SteamFlag = false;
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
                    Debug("MCC version: " + HCMGlobal.MCCversion + ", steamflag: " + HCMGlobal.SteamFlag.ToString());
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

            //now need to check if we have the offsets for this version in our local /offsets/ folder
            if (File.Exists(HCMGlobal.OffsetsPath + HCMGlobal.MCCversion + ".json"))
            {
                //need to unpack the json and setup our offsets..
                //gotta learn how json works lmao
                Debug("file exists! unpacking json");
                using (StreamReader r = new StreamReader(HCMGlobal.OffsetsPath + HCMGlobal.MCCversion + ".json"))
                {
                    string json = r.ReadToEnd();
                    HCMGlobal.LoadedOffsets = JsonConvert.DeserializeObject<Offsets>(json);
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
            //if no offsets found, let's check the github and try to download them!
            else
            {
            /*    String url = "https://www.ah.nl/service/rest/delegate?url=%2Fproducten%2Fproduct%2Fwi224735%2Fdoritos-nacho-cheese&_=1513938720642";
                System.Net.WebClient client = new System.Net.WebClient();
                String json = client.DownloadString(url);
                System.IO.File.WriteAllText("fileName.json", json);
                Console.WriteLine(json);*/



                //nah
            }
            

            //still no offsets found; popup warning about unsupported version, set attached to no and return


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
                case "Me":
                case "No":

                    break;
                case "HR":
                    break;

                case "H1":
                break;

                case "H2":
                    break;

                case "H3":
                    break;

                case "OD":
                    break;

                case "H4":
                    break;

            }
        }
    }
}
