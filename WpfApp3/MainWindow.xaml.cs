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
using System.Windows.Threading;
using System.Security.Principal;
using System.Threading;
using System.Text.RegularExpressions;
using System.Security.Cryptography;
using Newtonsoft.Json.Serialization;
using HCMHotkeys;

//TODO LIST

/*
//ERROR HANDLING STUFF

DONE --- Make returns more informational. Instead of just returning, return true or false (success) + error message that calling function can popup a dialog about. Will have to divide functions up a bit but it's for the best. 

DONE --- Add way more try catches to anywhere that reads memory, or handles files. Add file length check too.

DONE --- Add many user dialogs for when errors popup. 

DONE --- Verify online json file vs off-line one? Uh no just delete offline json file on first run (after update). 
DONE --- //also delete config I guess? jic

DONE --- Add mandatory update online checks. 

DONE --- Add state indicator checks/lockout so no funny business on loading screen/pgcr

NOT GONNA DO --- Implement busy flag in maintick

Clean up debug/logging stuff (inc; remove debugs from maintick except when Vals change). 

    //FEATURE STUFF

DONE --- Implement profiles.

DONE --- Add level check/lockout. Or way to load arbitary level when injecting

DO THIS - Add options for list level name - code (current), acronym, thumbnail. 

Change settings to have per-game options (clean up h1 checkpoints stuff) 

Clean up about page,stop it autoshowing on first run. 

DONE --- Add last selected tab and save to config and autoload on start. Also mainlist column widths. And vertical scrollbar position. 

DONE --- Add sorting functionality. 

Get all mp level splashes (clas and anni). 

Implement custom level splash images. With auto resizing. 

DO THIS - Add tool tip to double revert or just make icon better. Actually add tool tips to EVERYTHING

DONE --- Add open in Explorer button. 

DONE --- make injecting a cp print "Custom Checkpoint.. Injected" to the screen

CP thumbnails via screenshots.. not sure on this. would have to grab framebuffer so it doesn't accidentally screenshot not-video-game

Add hotkeys for dump/inject

Checkpoint Descriptions

Auto-dump checkpoints feature


    ////////BUG SHIT
    ///h2: fix cp injection totally breaking level restarts.
    ///h3: crash on loading cp's from different rally point - probs sim issue with reach, odst, h4
    ///




    BUG LIST:
    Multiplayer:
    HR: Multiplayer checkpoints don't work between game sessions.
    H2: Multiplayer will restart the match if you inject a checkpoint on a map when you haven't made at least one checkpoint in that match.
    H3: Multiplayer will completely crash if you inject a checkpoint on a map when you haven't made at least one checkpoint in that match.

    Campaign:
    OD: sometimes crashes after injecting then finishing a level
    OD: sometimes can't force checkpoints/reverts when on non-alpha rally points
    H3: injected checkpoints don't work from between different rallypoints (game crashes)
    HR: "Force checkpoint & dump" doesn't work on non-alpha rally points (tho they work individually)





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

        public static class HCMGlobal
        {
            public static readonly string HCMversion = "0.9.7b";

            public static readonly string LocalDir = System.IO.Path.GetDirectoryName(System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName);
            public static string H1CoreSavePath = LocalDir + @"\saves\h1cs";
            public static string H1CheckpointPath = LocalDir + @"\saves\h1cp";
            public static string H2CheckpointPath = LocalDir + @"\saves\h2cp";
            public static string H3CheckpointPath = LocalDir + @"\saves\h3cp";
            public static string HRCheckpointPath = LocalDir + @"\saves\hrcp";
            public static string ODCheckpointPath = LocalDir + @"\saves\odcp";
            public static string H4CheckpointPath = LocalDir + @"\saves\h4cp";


            public static readonly string RootH1CoreSavePath = LocalDir + @"\saves\h1cs";
            public static readonly string RootH1CheckpointPath = LocalDir + @"\saves\h1cp";
            public static readonly string RootH2CheckpointPath = LocalDir + @"\saves\h2cp";
            public static readonly string RootH3CheckpointPath = LocalDir + @"\saves\h3cp";
            public static readonly string RootHRCheckpointPath = LocalDir + @"\saves\hrcp";
            public static readonly string RootODCheckpointPath = LocalDir + @"\saves\odcp";
            public static readonly string RootH4CheckpointPath = LocalDir + @"\saves\h4cp";

            public static readonly string ConfigPath = LocalDir + @"\config.json";
            public static readonly string LogPath = LocalDir + @"\log.txt";
            public static readonly string OffsetsPath = LocalDir + @"\offsets\";

            public static HCMConfig SavedConfig;
            public static Offsets LoadedOffsets;
            public static MandatoryUpdates MandatoryUpdates;

            public static string ImageModeSuffix => (SavedConfig?.ClassicMode ?? false) ? "clas" : "anni";

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

            public static string CoreFolderPath;
        }

        public class Offsets
        {
            //offsets are gonna be stored as 2-unit arrays, first position is winstore, second is steam
            //that way when we're calling them from elsewhere we can just call Offsets.WhateverOffset[HCMGlobal.WinFlag] and it'll give us the one we want
            //the units will themselves be arbitary length arrays (each position for each offset in a multi-level pointer)

            //the actual values will be populated from the json file corrosponding to the attached mcc version

            //general
            public int[][] gameindicatormagic; //renamed from gameindicator > gameindicatormagicmagic. This is to 
                                               //deliberately break HCM versions before 0.9.3 since if they recieve
                                               //the 2028 offsets, they'll write junk data to cpmessage calls and crash MCC
            public int[][] menuindicator;
            public int[][] stateindicator;

            //h1
            public int[][] H1_LevelName; //45 33 c0 48 8b 50 28 8b 12 non writeable, third/last result, scroll up to mov that writes 01. that writes to revert
            public int[][] H1_CoreSave;
            public int[][] H1_CoreLoad;
            public int[][] H1_CheckString;
            public int[][] H1_TickCounter; //b0 01 4c 8b 7c 24 20 4c 8b 74 24 28. inc right above.
            public int[][] H1_Message; //8b 48 0c 89 0b. 89 0b writes to it (only when getting a message)

            public byte[] H1_MessageCode; //
            public int H1_CPData_LevelCode;
            public int H1_CPData_StartTick;
            public int H1_CPData_Difficulty;
            public int H1_CPData_Size;




            //hr
            public int[][] HR_LevelName; //0x3FB3D9 ahead of checkpoint
            public int[][] HR_CheckString;
            public int[][] HR_Checkpoint; //4a 8b 14 3b b8 a8 00 00 00, mov right above
            public int[][] HR_Revert; //rel
            public int[][] HR_DRflag; //b8 50 00 00 00 ** ** ** ** ** ** 48 8b 04 18, it's the ** when you get a cp
            public int[][] HR_CPLocation; //just scan lol
            public int[][] HR_LoadedSeed; //45 33 ff 84 db, take first. the mov above writes to a byte that's 0x2B before the loaded seed
            public int[][] HR_TickCounter; //8b c7 48 8b 0c 18 44 01 61 0c. the add writes to it.
            public int[][] HR_Message; //4B 8b 04 2e 8b 48 0c 89 4b 04. last mov writes to it (messageTC) when getting message.
            public int[][] HR_MessageCall; //48 89 6c 24 20 41 b9 01 00 00 00. should have "checkpoint save" next to it in disassembly. the call 7 instructions above the cp save is the cp message call ; E8 67 46 28 00


            public byte[] HR_MessageCode;
            public int HR_CPData_LevelCode;
            public int HR_CPData_StartTick;
            public int HR_CPData_Difficulty;
            public int HR_CPData_Seed;
            public int HR_CPData_DROffset1;
            public int HR_CPData_DROffset2;
            public int[] HR_CPData_SHA;
            public int HR_CPData_Size;
            public int[][] HR_CPData_PreserveLocations;



            public int[][] H2_LevelName; //take second last oldmombassa from; load outskirts then scan for, WRITEABLE: 30 33 61 5F 6F 6C 64 6D 6F 6D 62 61 73 61 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 73 63 65 6E 61 72 69 6F 73 5C 73 6F 6C 6F 5C 30 33 61 5F 6F 6C 64 6D 6F 6D 62 61 73
            public int[][] H2_CheckString;
            public int[][] H2_Checkpoint; //rel
            public int[][] H2_Revert; //bf ff ff ff ff ** ** 33 c9, the cmp immediately above accesses it
            public int[][] H2_DRflag; //c1 e0 05 8b d8 d1 ef 83 cb 40. movzx below accesses it on revert.
            public int[][] H2_CPLocation; //just scan lol
            public int[][] H2_TickCounter; //f3 0f 5d c6 0f 28 74 24 40 40 84 f6, it's a mov like 10 instr below
            public int[][] H2_Message; // the 89 03 (mov [rbx], eax), above c6 83 82 00 00 00 01 66 c7
            public int[][] H2_MessageCall; //48 8b 10 ff 52 58 48 8b c8 48 8b 10, go a little down to the call that immd precedes a mov and cmp. E8 57 CA 18 00
            public int[][] H2_LoadedBSP1; //scan for writable, unicode, case sens "halo2\h2_m". two of the results will have "untracked version" a little above". 0x1F8 before the halo2 string is the bsp bytes.
            public int[][] H2_LoadedBSP2;


            public byte[] H2_MessageCode;
            public int H2_CPData_LevelCode;
            public int H2_CPData_StartTick;
            public int H2_CPData_Difficulty;
            public int[] H2_CPData_BSP;
            public int H2_CPData_DROffset1;
            public int H2_CPData_DROffset2;
            public int H2_CPData_Size;
            public int[][] H2_CPData_PreserveLocations;


            public int[][] H3_LevelName; //load up ark easy, scan for writeable 64 61 65 68 0B 00 00 00 00 E0 D4 22 00 00 00 00 84 D6 F5 8A 01 00 00 00 00 E0 9E 1C 00 00 36 06
            public int[][] H3_CheckString;
            public int[][] H3_Checkpoint; //rel
            public int[][] H3_Revert; //first halo3 result of 4c 8d 5c 24 60 49 8b 5b 10 49 8b 73 18 49 8b 7b 20 4d 8b 73 28, it's the mov below it
            public int[][] H3_DRflag; //ff c0 ** ** ** ** ** ** 99 b9 00 02 00 00. it's the mov edx when getting a checkpoint.
            public int[][] H3_CPLocation; //just scan lol
            public int[][] H3_TickCounter; //48 8b 04 c8 48 8b 0c 10 8b 59 0c,, movs below point to it
            public int[][] H3_Message; //4a 8b 0c c0 48 8b 04 31 8b 48 0c, first result, mov ecx writes to messageTC when getting cp
            public int[][] H3_MessageCall; //48 83 ec 68 33 c9  call immediately below. E8 31 DF 1F 00
            public int[][] H3_LoadedBSP1; //load into ark. two last results of scan for writeable 01 00 00 00 00 00 00 00 C0 09 68 14 01 10 00 00 01 10 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
            public int[][] H3_LoadedBSP2;


            public byte[] H3_MessageCode;
            public int H3_CPData_LevelCode;
            public int H3_CPData_StartTick;
            public int H3_CPData_Difficulty;
            public int[] H3_CPData_BSP;
            public int[] H3_CPData_SHA;
            public int H3_CPData_DROffset1;
            public int H3_CPData_DROffset2;
            public int H3_CPData_Size;
            public int[][] H3_CPData_PreserveLocations;


            public int[][] OD_LevelName;
            public int[][] OD_CheckString;
            public int[][] OD_Checkpoint;
            public int[][] OD_Revert;
            public int[][] OD_DRflag;
            public int[][] OD_CPLocation;
            public int[][] OD_TickCounter;
            public int[][] OD_Message;
            public int[][] OD_MessageCall;
            public int[][] OD_LoadedBSP1;
            public int[][] OD_LoadedBSP2;

            public byte[] OD_MessageCode;
            public int OD_CPData_LevelCode;
            public int OD_CPData_StartTick;
            public int OD_CPData_Difficulty;
            public int[] OD_CPData_BSP;
            public int[] OD_CPData_SHA;
            public int OD_CPData_DROffset1;
            public int OD_CPData_DROffset2;
            public int OD_CPData_Size;
            public int[][] OD_CPData_PreserveLocations;

            public int[][] H4_LevelName; 
            public int[][] H4_CheckString;
            public int[][] H4_Checkpoint; 
            public int[][] H4_Revert; 
            public int[][] H4_DRflag; 
            public int[][] H4_CPLocation; 
            public int[][] H4_TickCounter; 
            public int[][] H4_Message; 
            public int[][] H4_MessageCall; 


            public byte[] H4_MessageCode;
            public int H4_CPData_LevelCode;
            public int H4_CPData_StartTick;
            public int H4_CPData_Difficulty;
            public int H4_CPData_DROffset1;
            public int H4_CPData_DROffset2;
            public int H4_CPData_Size;
            public int[][] H4_CPData_PreserveLocations;


            //public static int[][] HR_StartSeed = new int[2][]; //seed of the level start - you get a different seed in reach every time you start the level from the main menu

        }

        public class MandatoryUpdates
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
            @"saves\h3cp",
            @"saves\hrcp",
            @"saves\odcp",
            @"saves\h4cp",
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
        public ObservableCollection<HaloSaveFileMetadata> Halo3Checkpoints { get; set; } = new ObservableCollection<HaloSaveFileMetadata>();
        public ObservableCollection<HaloSaveFileMetadata> HaloODSTCheckpoints { get; set; } = new ObservableCollection<HaloSaveFileMetadata>();

        public ObservableCollection<HaloSaveFileMetadata> Halo4Checkpoints { get; set; } = new ObservableCollection<HaloSaveFileMetadata>();

        public ObservableCollection<HaloSaveFileMetadata> HaloReachCheckpoints { get; set; } = new ObservableCollection<HaloSaveFileMetadata>();

        public static KeyboardHook CPHotkeyHook = new KeyboardHook();
        public static KeyboardHook RevertHotkeyHook = new KeyboardHook();
        public static KeyboardHook DoubleRevertHotkeyHook = new KeyboardHook();
        
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


            //end testing stuff

            CS_MainList.SelectionChanged += List_SelectionChanged;
            CP_MainList.SelectionChanged += List_SelectionChanged;
            H2CP_MainList.SelectionChanged += List_SelectionChanged;
            H3CP_MainList.SelectionChanged += List_SelectionChanged;
            HRCP_MainList.SelectionChanged += List_SelectionChanged;
            ODCP_MainList.SelectionChanged += List_SelectionChanged;
            H4CP_MainList.SelectionChanged += List_SelectionChanged;
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

            SetupProfiles();
            

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
            if (HCMGlobal.SavedConfig.RanVersions != null)
            {
                foreach (string i in HCMGlobal.SavedConfig.RanVersions)
                {
                    if (i == HCMGlobal.HCMversion)
                        needtorefresh = false;
                }
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
                if (oldranversions != null)
                {
                    oldranversions = oldranversions.Append(HCMGlobal.HCMversion).ToArray();
                }
                else
                {
                    oldranversions = new string[] { HCMGlobal.HCMversion };
                }
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








            if (firstrun == true)
            {
                AboutButton.RaiseEvent(new RoutedEventArgs(Button.ClickEvent)); //raise aboutwindow
                firstrun = false;
            }

            RefreshButton.RaiseEvent(new RoutedEventArgs(Button.ClickEvent));


            //next let's load the reset ui values 

            try
            {
                TabList.SelectedIndex = HCMGlobal.SavedConfig.Reset_Tab;
                Debug("SURELY I'M BEING RUN BEFORE THE OTHER THING");

                H1CSProfile.SelectedIndex = HCMGlobal.SavedConfig.Reset_Folder[0];
                H1CPProfile.SelectedIndex = HCMGlobal.SavedConfig.Reset_Folder[1];
                H2Profile.SelectedIndex = HCMGlobal.SavedConfig.Reset_Folder[2];
                H3Profile.SelectedIndex = HCMGlobal.SavedConfig.Reset_Folder[3];
                HRProfile.SelectedIndex = HCMGlobal.SavedConfig.Reset_Folder[4];
                ODProfile.SelectedIndex = HCMGlobal.SavedConfig.Reset_Folder[5];
                H4Profile.SelectedIndex = HCMGlobal.SavedConfig.Reset_Folder[6];

                SetupProfiles();
                Debug("eh why aren't I getting here");
                ProfileChangedNull();
                Debug("after pcn");

                CS_MainList.SelectedIndex = HCMGlobal.SavedConfig.Reset_SelCP[0];
                CS_MainList.ScrollIntoView(CS_MainList.SelectedItem);
                CP_MainList.SelectedIndex = HCMGlobal.SavedConfig.Reset_SelCP[1];
                CP_MainList.ScrollIntoView(CP_MainList.SelectedItem);
                H2CP_MainList.SelectedIndex = HCMGlobal.SavedConfig.Reset_SelCP[2];
                H2CP_MainList.ScrollIntoView(H2CP_MainList.SelectedItem);
                H3CP_MainList.SelectedIndex = HCMGlobal.SavedConfig.Reset_SelCP[3];
                H3CP_MainList.ScrollIntoView(H3CP_MainList.SelectedItem);
                HRCP_MainList.SelectedIndex = HCMGlobal.SavedConfig.Reset_SelCP[4];
                HRCP_MainList.ScrollIntoView(HRCP_MainList.SelectedItem);
                ODCP_MainList.SelectedIndex = HCMGlobal.SavedConfig.Reset_SelCP[5];
                ODCP_MainList.ScrollIntoView(ODCP_MainList.SelectedItem);
                H4CP_MainList.SelectedIndex = HCMGlobal.SavedConfig.Reset_SelCP[6];
                H4CP_MainList.ScrollIntoView(H4CP_MainList.SelectedItem);

                List<GridView> grids = new List<GridView>();
                grids.Add(CS_MainList.View as GridView);
                grids.Add(CP_MainList.View as GridView);
                grids.Add(H2CP_MainList.View as GridView);
                grids.Add(H3CP_MainList.View as GridView);
                grids.Add(HRCP_MainList.View as GridView);
                grids.Add(ODCP_MainList.View as GridView);
                grids.Add(H4CP_MainList.View as GridView);


                //this is hell jank
                //basically we're just applying to column widths from the config file to the actual gridviews


                Debug("eh: " + grids.Count);
                Debug("EH: " + HCMGlobal.SavedConfig.Reset_Col.Length);

                int j = 0;
                for (int i = 0; i < grids.Count; i++)
                {
                    
                    foreach (var column in grids[i].Columns)
                    {
                        Debug("this needs to count from 0 up: " + (j));
                        column.Width = HCMGlobal.SavedConfig.Reset_Col[j]; //uh
                        j++; 
                    }

                }




            }
            catch (Exception ex)
            {
                Debug("failed somewhere resetting the ui values");

                // Get stack trace for the exception with source file information
                var st = new StackTrace(ex, true);
                // Get the top stack frame
                var frame = st.GetFrame(0);
                // Get the line number from the stack frame
                var line = frame.GetFileLineNumber();
                Debug(line.ToString());
            }


            /* HCMGlobal.SavedConfig.Reset_Tab = TabList.SelectedIndex;
             HCMGlobal.SavedConfig.Reset_Folder = new int[] { H1CSProfile.SelectedIndex, H1CPProfile.SelectedIndex, H2Profile.SelectedIndex, H3Profile.SelectedIndex, HRProfile.SelectedIndex, ODProfile.SelectedIndex };
             HCMGlobal.SavedConfig.Reset_SelCP = new int[] { CS_MainList.SelectedIndex, CP_MainList.SelectedIndex, H2CP_MainList.SelectedIndex, H3CP_MainList.SelectedIndex, HRCP_MainList.SelectedIndex, ODCP_MainList.SelectedIndex };
             HCMGlobal.SavedConfig.Reset_Col = new double[] { };

             List<GridView> grids = new List<GridView>();
             grids.Add(CS_MainList.View as GridView);
             grids.Add(CS_MainList.View as GridView);


             foreach (var grid in grids)
             {
                 foreach (var column in grid.Columns)
                 {
                     HCMGlobal.SavedConfig.Reset_Col.Append(column.Width);
                 }
             }*/


            CPHotkeyHook.KeyPressed += new EventHandler<KeyPressedEventArgs>(ForceCP);
            CPHotkeyHook.RegisterHotKey(HCMHotkeys.ModifierKeys.None, (System.Windows.Forms.Keys)KeyInterop.VirtualKeyFromKey(HCMGlobal.SavedConfig.CPHotkey));
            RevertHotkeyHook.KeyPressed += new EventHandler<KeyPressedEventArgs>(ForceRevert);
            RevertHotkeyHook.RegisterHotKey(HCMHotkeys.ModifierKeys.None, (System.Windows.Forms.Keys)KeyInterop.VirtualKeyFromKey(HCMGlobal.SavedConfig.RevertHotkey));
            DoubleRevertHotkeyHook.KeyPressed += new EventHandler<KeyPressedEventArgs>(ForceDoubleRevert);
            DoubleRevertHotkeyHook.RegisterHotKey(HCMHotkeys.ModifierKeys.None, (System.Windows.Forms.Keys)KeyInterop.VirtualKeyFromKey(HCMGlobal.SavedConfig.DoubleRevertHotkey));
        }

        private void Window_Closing(object sender, CancelEventArgs e)
        {
            //main thing we do here is record the windows current tab, folder, selected save, and column widths (for each tab). 
            //save them to the config file
            //then next time we do window loaded we can try to load them
            HCMGlobal.SavedConfig.Reset_Tab = TabList.SelectedIndex;
            HCMGlobal.SavedConfig.Reset_Folder = new int[] { H1CSProfile.SelectedIndex, H1CPProfile.SelectedIndex, H2Profile.SelectedIndex, H3Profile.SelectedIndex, HRProfile.SelectedIndex, ODProfile.SelectedIndex, H4Profile.SelectedIndex };
            HCMGlobal.SavedConfig.Reset_SelCP = new int[] { CS_MainList.SelectedIndex, CP_MainList.SelectedIndex, H2CP_MainList.SelectedIndex, H3CP_MainList.SelectedIndex, HRCP_MainList.SelectedIndex, ODCP_MainList.SelectedIndex, H4CP_MainList.SelectedIndex };
            

            List<GridView> grids = new List<GridView>();
            grids.Add(CS_MainList.View as GridView);
            grids.Add(CP_MainList.View as GridView);
            grids.Add(H2CP_MainList.View as GridView);
            grids.Add(H3CP_MainList.View as GridView);
            grids.Add(HRCP_MainList.View as GridView);
            grids.Add(ODCP_MainList.View as GridView);
            grids.Add(H4CP_MainList.View as GridView);

            List<double> temparray = new List<double>();

            foreach (var grid in grids)
            {
                foreach (var column in grid.Columns)
                {
                    //Debug("hmmm: " + column.Width);
                    temparray.Add(column.Width);
                }
            }
            Debug("this should be a big number: " + temparray.Count);
            HCMGlobal.SavedConfig.Reset_Col = temparray.ToArray();
            Debug("this should be a big number: " + HCMGlobal.SavedConfig.Reset_Col.Length);


            WriteConfig();
        }


        void GridViewColumnHeaderClickedHandler(object sender, RoutedEventArgs e)
        {
            //basically, if we click on a header, return to a default/autosized column width
            var headerClicked = e.OriginalSource as GridViewColumnHeader;
            if (headerClicked != null)
            {
                DependencyProperty dp = WidthProperty;
                headerClicked.Column.ClearValue(dp);
            }

        }


        readonly Dictionary<string, string> ProfileTypeToPath = new Dictionary<string, string>()
            {
                { "H1CSProfile", HCMGlobal.H1CoreSavePath},
                { "H1CPProfile", HCMGlobal.H1CheckpointPath},
                { "H2Profile", HCMGlobal.H2CheckpointPath},
                { "H3Profile", HCMGlobal.H3CheckpointPath},
                { "HRProfile", HCMGlobal.HRCheckpointPath},
                { "ODProfile", HCMGlobal.ODCheckpointPath},
                { "H4Profile", HCMGlobal.H4CheckpointPath},
            };

        private string ProfileTypeToPathGet(string code)
        {
            string Name;
            if (ProfileTypeToPath.TryGetValue(code, out Name))
            {
                return Name;
            }

            return code;
        }


        private void SetupProfiles()
       {
            try
            {
                var list = new List<ComboBox> { H1CSProfile, H1CPProfile, H2Profile, H3Profile, HRProfile, ODProfile, H4Profile };
                var oldselected = H1CSProfile.SelectedItem;

                foreach (ComboBox cb in list)
                {
                    oldselected = cb.SelectedItem;
                    //Debug("bbbb: " + cb.SelectedItem);
                    cb.Items.Clear();
                    cb.Items.Add("*Root");



                    DirectoryInfo dir = new DirectoryInfo(ProfileTypeToPathGet(cb.Name.ToString()));
                    DirectoryInfo[] folderlist = dir.GetDirectories().OrderByDescending(p => p.LastWriteTime).ToArray();

                    foreach (DirectoryInfo folder in folderlist)
                    {
                        cb.Items.Add(folder.Name);
                    }

                    //Debug("aaaa: " + cb.Items.IndexOf(oldselected));
                    //Debug("cccc: " + cb.SelectedIndex);

                    cb.Items.Add("++ New ++");
                    if (cb.Items.IndexOf(oldselected) == -1)
                        cb.SelectedIndex = 0;
                    else
                        cb.SelectedIndex = cb.Items.IndexOf(oldselected);


                }
            }
            catch (Exception ex)
            {
                Debug("setup profile exception: " + ex.ToString());
            }
        }

        private void ProfileChangedNull()

        {

                bool backtoroot = true;

                Debug(H1CSProfile.SelectedIndex.ToString());
                Debug("REEEEEEEEEEEEEEEEEEEEEEE");
            Debug(H1CSProfile.SelectedIndex.ToString());
            Debug(H1CSProfile.SelectedItem.ToString());
            switch (TabList.SelectedIndex)
                {


                    case 0:
                    if (H1CSProfile.SelectedIndex > 0)
                    {
                        backtoroot = false;
                    }
                        HCMGlobal.H1CoreSavePath = HCMGlobal.RootH1CoreSavePath + (backtoroot ? "" : ("\\" + H1CSProfile.SelectedItem.ToString()));
                        break;

                    case 1:
                    if (H1CPProfile.SelectedIndex > 0)
                    {
                        backtoroot = false;
                    }
                    HCMGlobal.H1CheckpointPath = HCMGlobal.RootH1CheckpointPath + (backtoroot ? "" : ("\\" + H1CPProfile.SelectedItem.ToString()));
                        break;

                    case 2:
                    if (H2Profile.SelectedIndex > 0)
                    {
                        backtoroot = false;
                    }
                    HCMGlobal.H2CheckpointPath = HCMGlobal.RootH2CheckpointPath + (backtoroot ? "" : ("\\" + H2Profile.SelectedItem.ToString()));
                        break;

                    case 3:
                    if (H3Profile.SelectedIndex > 0)
                    {
                        backtoroot = false;
                    }
                    HCMGlobal.H3CheckpointPath = HCMGlobal.RootH3CheckpointPath + (backtoroot ? "" : ("\\" + H3Profile.SelectedItem.ToString()));
                        break;

                    case 4:
                    if (HRProfile.SelectedIndex > 0)
                    {
                        backtoroot = false;
                    }
                    HCMGlobal.HRCheckpointPath = HCMGlobal.RootHRCheckpointPath + (backtoroot ? "" : ("\\" + HRProfile.SelectedItem.ToString()));
                    break;

                    case 5:
                    if (ODProfile.SelectedIndex > 0)
                    {
                        backtoroot = false;
                    }
                    HCMGlobal.ODCheckpointPath = HCMGlobal.RootODCheckpointPath + (backtoroot ? "" : ("\\" + ODProfile.SelectedItem.ToString()));
                        break;

                    case 6:
                    if (H4Profile.SelectedIndex > 0)
                    {
                        backtoroot = false;
                    }
                    HCMGlobal.H4CheckpointPath = HCMGlobal.RootH4CheckpointPath + (backtoroot ? "" : ("\\" + H4Profile.SelectedItem.ToString()));
                        break;



                    default:
                        break;

                }

          
        }


        private void ProfileChanged(object sender, SelectionChangedEventArgs e)
        {


            if (H1CSProfile.SelectedItem != null)
            Debug("GJIJG: " + H1CSProfile.SelectedItem.ToString());

            //figure out which game this was for

               ComboBox cb = (ComboBox)sender;
            string parent_name = cb.Name;

            bool skipcondition = false;

            if (cb.IsDropDownOpen)
            {
                if (cb.SelectedIndex == cb.Items.Count - 1) //the "new folder" option
                {
                    var userinput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                         "Create a new Profile (folder) with the following name",
                                 "",
                                     -1, -1);
                    if (userinput != "") //this would indicate cancel click
                    {
                        string proposedfolder = ProfileTypeToPathGet(cb.Name.ToString()) + "\\" + userinput;
                        try
                        {
                            if (userinput.Contains("\\"))
                            {
                                throw new Exception("someone put the funny slashes in their folder name");
                            }

                            Debug("HMMMMM" + proposedfolder);
                            Directory.CreateDirectory(proposedfolder);
                            SetupProfiles();
                            cb.SelectedIndex = 1;
                        }
                        catch
                        {
                            MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(cb.Name + ": Failed to create new profile (folder) at: " + proposedfolder + " \n Possible invalid characters?", "Error", System.Windows.MessageBoxButton.OK);

                            cb.SelectedIndex = cb.Items.IndexOf(e.RemovedItems[0]);
                            skipcondition = true;
                        }


                    }
                    else
                    {
                        cb.SelectedIndex = cb.Items.IndexOf(e.RemovedItems[0]);
                        skipcondition = true;
                    }

                }

                if (!skipcondition)
                {
                    bool backtoroot = true;
                    Debug("EEEEEEEEEE" + cb.SelectedItem.ToString());
                    if (cb.SelectedItem.ToString() != "*Root")
                        backtoroot = false;

                    // Debug("this is where I would change the profile, I think: " + cb.SelectedItem.ToString());
                    switch (cb.Name.ToString())
                    {
                        case "H1CSProfile":
                            HCMGlobal.H1CoreSavePath = HCMGlobal.RootH1CoreSavePath + (backtoroot ? "" : ("\\" + cb.SelectedItem.ToString()));
                            break;
                        case "H1CPProfile":
                            HCMGlobal.H1CheckpointPath = HCMGlobal.RootH1CheckpointPath + (backtoroot ? "" : ("\\" + cb.SelectedItem.ToString()));
                            break;
                        case "H2Profile":
                            HCMGlobal.H2CheckpointPath = HCMGlobal.RootH2CheckpointPath + (backtoroot ? "" : ("\\" + cb.SelectedItem.ToString()));
                            break;
                        case "H3Profile":
                            HCMGlobal.H3CheckpointPath = HCMGlobal.RootH3CheckpointPath + (backtoroot ? "" : ("\\" + cb.SelectedItem.ToString()));
                            break;
                        case "HRProfile":
                            HCMGlobal.HRCheckpointPath = HCMGlobal.RootHRCheckpointPath + (backtoroot ? "" : ("\\" + cb.SelectedItem.ToString()));
                            break;
                        case "ODProfile":
                            HCMGlobal.ODCheckpointPath = HCMGlobal.RootODCheckpointPath + (backtoroot ? "" : ("\\" + cb.SelectedItem.ToString()));
                            break;
                        case "H4Profile":
                            HCMGlobal.H4CheckpointPath = HCMGlobal.RootH4CheckpointPath + (backtoroot ? "" : ("\\" + cb.SelectedItem.ToString()));
                            break;



                        default:
                            break;


                    }
                }
                
                
                cb.IsDropDownOpen = false;
            }

           

            RefreshList(sender, e);
            RefreshSel(sender, e);
        }

        private void OpenExplorer(object sender, RoutedEventArgs e)
        {

            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;
            string pathtoopen = "";

            switch (parent_name)
            {
                case "H1CS":
                    pathtoopen = HCMGlobal.H1CoreSavePath;
                    break;

                case "H1CP":
                    pathtoopen = HCMGlobal.H1CheckpointPath;
                    break;

                case "H2CP":
                    pathtoopen = HCMGlobal.H2CheckpointPath;
                    break;

                case "H3CP":
                    pathtoopen = HCMGlobal.H3CheckpointPath;
                    break;

                case "HRCP":
                    pathtoopen = HCMGlobal.HRCheckpointPath;
                    break;

                case "ODCP":
                    pathtoopen = HCMGlobal.ODCheckpointPath;
                    break;

                case "H4CP":
                    pathtoopen = HCMGlobal.H4CheckpointPath;
                    break;

                default:
                    break;
            }

            if (Directory.Exists(pathtoopen))
            {
                Process.Start(pathtoopen);
            }

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



        private static class PhysicalSaveFiles
        {
            public static HaloSaveFileMetadata SaveFileMetaData;
           
        }

        private void SortSavesClick(object sender, RoutedEventArgs e)
        {
            //thinks I want to be able to sort by: name alphabetical, time into level, difficulty, lastwritetime. all reversible.


            //alphabetical, timeinto level, difficulty, level, lastwritetime
            //hmm
            //let's get data context then open up a window where user can choose what they want to sort by
            ListView mainlist = null;
            string savepath = null;

            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            switch (parent_name)
            {
                case "H1CS":
                    mainlist = CS_MainList;
                    savepath = HCMGlobal.H1CoreSavePath;
                    break;

                case "H1CP":
                    mainlist = CP_MainList;
                    savepath = HCMGlobal.H1CheckpointPath;
                    break;

                case "H2CP":
                    mainlist = H2CP_MainList;
                    savepath = HCMGlobal.H2CheckpointPath;
                    break;

                case "H3CP":
                    mainlist = H3CP_MainList;
                    savepath = HCMGlobal.H3CheckpointPath;
                    break;

                case "HRCP":
                    mainlist = HRCP_MainList;
                    savepath = HCMGlobal.HRCheckpointPath;
                    break;

                case "ODCP":
                    mainlist = ODCP_MainList;
                    savepath = HCMGlobal.ODCheckpointPath;
                    break;

                case "H4CP":
                    mainlist = H4CP_MainList;
                    savepath = HCMGlobal.H4CheckpointPath;
                    break;

                default:
                    break;
            }

            if (mainlist == null || savepath == null || !Directory.Exists(savepath))
            {
                //popup error message and cancel
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show("Something went horribly wrong trying to setup save sorting window", "Error", System.Windows.MessageBoxButton.OK);
                return;
            }

            Debug("hmm: " + savepath);
            SortSavesWindow SortSavesWindow = new SortSavesWindow(savepath.Substring(savepath.LastIndexOf("saves") + 5), ref HCMGlobal.SavedConfig);
            bool? whatbutton = SortSavesWindow.ShowDialog();

            //need to check for cancel click in sortsaveswindow
            if (whatbutton != true)
                return;

            //get return values from sort saves
            //0 = none
            //1 = difficulty
            //2 = level order (just sort alphabetically by levelname, maybe seperate multi out first)
            //3 = Alphabetically
            //4 = Time into level

            (int, bool)[] criteria = SortSavesWindow.Criteria();
            HCMGlobal.SavedConfig.Sort.SetCriteria(criteria);
            HCMGlobal.SavedConfig.Sort.ReversePreviousPosition = SortSavesWindow.ReversePreviousPosition();



            List<DateTime> arrayoftimes = new List<DateTime>();
            try
            {
                //make a list of lastwrite times and save it to the end so we can reapply in the new order
                for (int i = 0; i < mainlist.Items.Count; i++)
                {
                    var filedata = mainlist.Items.GetItemAt(i) as HaloSaveFileMetadata;
                    arrayoftimes.Add(File.GetLastWriteTime($@"{savepath}\{filedata.Name}.bin"));
                }

            }
            catch (Exception ex)
            {
                Log("unknown error occured: " + ex.ToString());
            }

            Debug("length of array of times: " + arrayoftimes.Count);
            if (arrayoftimes.Count == 0)
                return; //don't want to try sorting an empty list

            List<HaloSaveFileMetadata> SortList = new List<HaloSaveFileMetadata>();
            try
            {
                //now need a list of the saves with all our relavent properties
                for (int i = 0; i < mainlist.Items.Count; i++)
                {
                    var filedata = mainlist.Items.GetItemAt(i) as HaloSaveFileMetadata;
                    filedata.LastWriteTime = (File.GetLastWriteTime($@"{savepath}\{filedata.Name}.bin")); //have to add this property
                    filedata.IsMultiplayer = LevelCodeToGameType(filedata.LevelCode);
                    SortList.Add(filedata);
                }

            }
            catch (Exception ex)
            {
                Log("unknown error occured: " + ex.ToString());
                //need to add a popup error message here
                return;
            }

            //okay, sorting time. #

            //let's do shit in reverse order basically

            if (SortSavesWindow.ReversePreviousPosition())
            {
                Debug("Yes, REVERSING");
                SortList = SortList.OrderBy(x => x.LastWriteTime)
                                          .ToList();
            }


            //0 = none
            //1 = difficulty
            //2 = level order (just sort alphabetically by levelname, maybe seperate multi out first)
            //3 = Alphabetically
            //4 = Time into level

            //just gonna declare this boy here cos it makes more sense
            void SortThisGuyOut (int sortoption, bool reverseoption)
                {
                switch (sortoption)
                {
                    case 0: //none
                    default:
                        break;

                    case 1: //difficulty
                        if (!reverseoption) //reversing cos I feel like it
                        {
                            SortList = SortList.OrderBy(x => x.Difficulty)
                                  .ToList();
                        }
                        else
                        {
                            SortList = SortList.OrderByDescending(x => x.Difficulty)
                                      .ToList();
                        }
                        break;

                    case 2: //level order (aka level code alphabetical) -- needs extra check for multi
                        if (!reverseoption) //reversing cos I feel like it
                        {
                            SortList = SortList.OrderBy(x => x.IsMultiplayer)
                                               .ThenBy(y => y.LevelCode)
                                  .ToList();
                        }
                        else
                        {
                            SortList = SortList.OrderByDescending(x => x.IsMultiplayer)
                                               .ThenByDescending(y => y.LevelCode)
                                      .ToList();
                        }
                        break;

                    case 3: //alphabetical by cp name
                        if (!reverseoption) //reversing cos I feel like it
                        {
                            SortList = SortList.OrderBy(x => x.Name)
                                  .ToList();
                        }
                        else
                        {
                            SortList = SortList.OrderByDescending(x => x.Name)
                                      .ToList();
                        }
                        break;

                    case 4: //time into level
                        if (!reverseoption) //reversing this cos reasons
                        {
                            SortList = SortList.OrderBy(x => x.StartTick)
                                  .ToList();
                        }
                        else
                        {
                            SortList = SortList.OrderByDescending(x => x.StartTick)
                                      .ToList();
                        }
                        break;


                }
            }

            //now let's call it
            foreach (var (key, reverse) in criteria.Reverse())
                SortThisGuyOut(key, reverse);



            Debug("Sortlist first pos: " + SortList[0].Name);
            var testdata = mainlist.Items.GetItemAt(0) as HaloSaveFileMetadata;
            Debug("mainlist first pos: " + testdata.Name);

            //then reapply the writetimes we got at the start, but in the new order
            for (int i = 0; i < SortList.Count; i++)
            {
                //Debug("difficulty: " + SortList[i].Difficulty);
                File.SetLastWriteTime(savepath + "\\" + SortList[i].Name + ".bin", arrayoftimes[i]);
            }

            RefreshList(sender, e);


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
            var mainlist = CS_MainList;
            var savepath = HCMGlobal.H1CoreSavePath;

            switch (parent_name)
            {
                case "H1CS":
                    mainlist = CS_MainList;
                    savepath = HCMGlobal.H1CoreSavePath;
                    break;

                case "H1CP":
                    mainlist = CP_MainList;
                    savepath = HCMGlobal.H1CheckpointPath;
                    break;

                case "H2CP":
                    mainlist = H2CP_MainList;
                    savepath = HCMGlobal.H2CheckpointPath;
                    break;

                case "H3CP":
                    mainlist = H3CP_MainList;
                    savepath = HCMGlobal.H3CheckpointPath;
                    break;

                case "ODCP":
                    mainlist = ODCP_MainList;
                    savepath = HCMGlobal.ODCheckpointPath;
                    break;

                case "HRCP":
                    mainlist = HRCP_MainList;
                    savepath = HCMGlobal.HRCheckpointPath;
                    break;

                case "H4CP":
                    mainlist = H4CP_MainList;
                    savepath = HCMGlobal.H4CheckpointPath;
                    break;

                default:
                    break;

            }

                    if (mainlist.SelectedIndex >= 1)
                    {
                        var fileBelow = mainlist.Items.GetItemAt(mainlist.SelectedIndex) as HaloSaveFileMetadata;
                        var fileAbove = mainlist.Items.GetItemAt(mainlist.SelectedIndex - 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string movethis = $@"{savepath}\{fileBelow.Name}.bin";
                            string abovefile = $@"{savepath}\{fileAbove.Name}.bin";
                            SwapFileTimes(movethis, abovefile);
                             mainlist.SelectedIndex--;
                        }
                    }
                   

            RefreshList(sender, e);
        }

        private void MoveUpButton_All_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);

            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            parent = (FrameworkElement)((FrameworkElement)parent).Parent;
            string parent_name = parent.Name;

            var mainlist = CS_MainList;
            var savepath = HCMGlobal.H1CoreSavePath;

            switch (parent_name)
            {
                case "H1CS":
                    mainlist = CS_MainList;
                    savepath = HCMGlobal.H1CoreSavePath;
                    break;

                case "H1CP":
                    mainlist = CP_MainList;
                    savepath = HCMGlobal.H1CheckpointPath;
                    break;

                case "H2CP":
                    mainlist = H2CP_MainList;
                    savepath = HCMGlobal.H2CheckpointPath;
                    break;

                case "H3CP":
                    mainlist = H3CP_MainList;
                    savepath = HCMGlobal.H3CheckpointPath;
                    break;

                case "ODCP":
                    mainlist = ODCP_MainList;
                    savepath = HCMGlobal.ODCheckpointPath;
                    break;

                case "HRCP":
                    mainlist = HRCP_MainList;
                    savepath = HCMGlobal.HRCheckpointPath;
                    break;

                case "H4CP":
                    mainlist = H4CP_MainList;
                    savepath = HCMGlobal.H4CheckpointPath;
                    break;

                default:
                    break;

            }


                    if (mainlist.SelectedIndex >= 0)
                    {
                        var fileBelow = mainlist.Items.GetItemAt(mainlist.SelectedIndex) as HaloSaveFileMetadata;
                        var fileAbove = mainlist.Items.GetItemAt(0) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {

                            string movethis = $@"{savepath}\{fileBelow.Name}.bin";
                            string abovefile = $@"{savepath}\{fileAbove.Name}.bin";
                            ArbitaryFileTimeMove(mainlist.SelectedIndex, 0, mainlist, savepath);
                    mainlist.SelectedIndex = 0;
                        }
                    }
                   

            RefreshList(sender, e);

        }

        private void MoveDownButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);

            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            var mainlist = CS_MainList;
            var savepath = HCMGlobal.H1CoreSavePath;

            switch (parent_name)
            {
                case "H1CS":
                    mainlist = CS_MainList;
                    savepath = HCMGlobal.H1CoreSavePath;
                    break;

                case "H1CP":
                    mainlist = CP_MainList;
                    savepath = HCMGlobal.H1CheckpointPath;
                    break;

                case "H2CP":
                    mainlist = H2CP_MainList;
                    savepath = HCMGlobal.H2CheckpointPath;
                    break;

                case "H3CP":
                    mainlist = H3CP_MainList;
                    savepath = HCMGlobal.H3CheckpointPath;
                    break;

                case "ODCP":
                    mainlist = ODCP_MainList;
                    savepath = HCMGlobal.ODCheckpointPath;
                    break;

                case "HRCP":
                    mainlist = HRCP_MainList;
                    savepath = HCMGlobal.HRCheckpointPath;
                    break;

                case "H4CP":
                    mainlist = H4CP_MainList;
                    savepath = HCMGlobal.H4CheckpointPath;
                    break;

                default:
                    break;

            }

                    if (mainlist.SelectedItem != null && mainlist.SelectedIndex != mainlist.Items.Count - 1)
                    {
                        var fileAbove = mainlist.Items.GetItemAt(mainlist.SelectedIndex) as HaloSaveFileMetadata;
                        var fileBelow = mainlist.Items.GetItemAt(mainlist.SelectedIndex + 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string pathAbove = $@"{savepath}\{fileAbove.Name}.bin";
                            string pathBelow = $@"{savepath}\{fileBelow.Name}.bin";
                            SwapFileTimes(pathAbove, pathBelow);
                    mainlist.SelectedIndex++;
                        }
                    }
                    

            RefreshList(sender, e);
        }

        private void MoveDownButton_All_Click(object sender, RoutedEventArgs e)
        {
            RefreshSel(sender, e);

            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            parent = (FrameworkElement)((FrameworkElement)parent).Parent;
            string parent_name = parent.Name;

            var mainlist = CS_MainList;
            var savepath = HCMGlobal.H1CoreSavePath;

            switch (parent_name)
            {
                case "H1CS":
                    mainlist = CS_MainList;
                    savepath = HCMGlobal.H1CoreSavePath;
                    break;

                case "H1CP":
                    mainlist = CP_MainList;
                    savepath = HCMGlobal.H1CheckpointPath;
                    break;

                case "H2CP":
                    mainlist = H2CP_MainList;
                    savepath = HCMGlobal.H2CheckpointPath;
                    break;

                case "H3CP":
                    mainlist = H3CP_MainList;
                    savepath = HCMGlobal.H3CheckpointPath;
                    break;

                case "ODCP":
                    mainlist = ODCP_MainList;
                    savepath = HCMGlobal.ODCheckpointPath;
                    break;

                case "HRCP":
                    mainlist = HRCP_MainList;
                    savepath = HCMGlobal.HRCheckpointPath;
                    break;

                case "H4CP":
                    mainlist = H4CP_MainList;
                    savepath = HCMGlobal.H4CheckpointPath;
                    break;

                default:
                    break;

            }

                    if (mainlist != null && mainlist.SelectedIndex != mainlist.Items.Count - 1)
                    {
                        var fileAbove = mainlist.Items.GetItemAt(mainlist.SelectedIndex) as HaloSaveFileMetadata;
                        var fileBelow = mainlist.Items.GetItemAt(mainlist.Items.Count - 1) as HaloSaveFileMetadata;
                        if (fileBelow != null && fileAbove != null)
                        {
                            string pathAbove = $@"{savepath}\{fileAbove.Name}.bin";
                            string pathBelow = $@"{savepath}\{fileBelow.Name}.bin";
                            ArbitaryFileTimeMove(mainlist.SelectedIndex, mainlist.Items.Count - 1, mainlist, savepath);
                    mainlist.SelectedIndex = mainlist.Items.Count - 1;
                        }
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
            var mainlist = CS_MainList;
            var savepath = HCMGlobal.H1CoreSavePath;

            switch(btn.Name)
            {
                case "CS_Sel_RenameButton":
                    mainlist = CS_MainList;
                    savepath = HCMGlobal.H1CoreSavePath;
                 break;
                case "CP_Sel_RenameButton":
                    mainlist = CP_MainList;
                    savepath = HCMGlobal.H1CheckpointPath;
                    break;
                case "H2CP_Sel_RenameButton":
                    mainlist = H2CP_MainList;
                    savepath = HCMGlobal.H2CheckpointPath;
                    break;
                case "H3CP_Sel_RenameButton":
                    mainlist = H3CP_MainList;
                    savepath = HCMGlobal.H3CheckpointPath;
                    break;

                case "ODCP_Sel_RenameButton":
                    mainlist = ODCP_MainList;
                    savepath = HCMGlobal.ODCheckpointPath;
                    break;

                case "HRCP_Sel_RenameButton":
                    mainlist = HRCP_MainList;
                    savepath = HCMGlobal.HRCheckpointPath;
                    break;

                case "H4CP_Sel_RenameButton":
                    mainlist = H4CP_MainList;
                    savepath = HCMGlobal.H4CheckpointPath;
                    break;


                default:
                    break;

            }

            string input = "";
                    if (mainlist.SelectedItem != null)
                    {
                        var item = mainlist.Items.GetItemAt(mainlist.SelectedIndex);
                        System.Type type2 = item.GetType();
                        s2 = (string)type2.GetProperty("Name").GetValue(item, null);
                        backup = savepath + @"\" + s2 + @".bin";
                        backuploc = savepath;
                        var userinput = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                 "Rename your backup save",
                         s2,
                             -1, -1);
                        proposedsave = (backuploc + @"\" + userinput + @".bin");
                input = userinput;
                    }
                    
        

            if (File.Exists(backup) && !File.Exists(proposedsave) && input.Length > 0) 
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
            var mainlist = CS_MainList;
            var savepath = HCMGlobal.H1CoreSavePath;

            RefreshSel(sender, e);
            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            switch (parent_name)
            {
                case "H1CS":
                    oldselected = CS_MainList.SelectedIndex;
                    mainlist = CS_MainList;
                    savepath = HCMGlobal.H1CoreSavePath;
                    break;

                case "H2CP":
                    oldselected = H2CP_MainList.SelectedIndex;
                    mainlist = H2CP_MainList;
                    savepath = HCMGlobal.H2CheckpointPath;
                    break;

                case "H3CP":
                    oldselected = H3CP_MainList.SelectedIndex;
                    mainlist = H3CP_MainList;
                    savepath = HCMGlobal.H3CheckpointPath;
                    break;

                case "ODCP":
                    oldselected = ODCP_MainList.SelectedIndex;
                    mainlist = ODCP_MainList;
                    savepath = HCMGlobal.ODCheckpointPath;
                    break;

                case "HRCP":
                    oldselected = HRCP_MainList.SelectedIndex;
                    mainlist = HRCP_MainList;
                    savepath = HCMGlobal.HRCheckpointPath;
                    break;

                case "H4CP":
                    oldselected = H4CP_MainList.SelectedIndex;
                    mainlist = H4CP_MainList;
                    savepath = HCMGlobal.H4CheckpointPath;
                    break;

            }

            if (mainlist.SelectedItem != null)
            {
                var item = mainlist.Items.GetItemAt(mainlist.SelectedIndex) as HaloSaveFileMetadata;
                string path = $@"{savepath}\{item.Name}.bin";

                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show("Delete " + item.Name + "?", "Delete Confirmation", System.Windows.MessageBoxButton.YesNo);
                if (messageBoxResult == MessageBoxResult.Yes)
                {
                    DeleteSaveFile(path);
                }
            }
            RefreshList(sender, e);
            mainlist.SelectedIndex = oldselected;
            RefreshSel(sender, e);

        }

        private void RefreshButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshLoa(sender, e);
            RefreshList(sender, e);
            RefreshSel(sender, e);
            SetupProfiles();
        }

        private void SettingsButton_Click(object sender, RoutedEventArgs e)
        {
            SettingsWindow settingswindow = new SettingsWindow();

            
            settingswindow.modeanni.IsChecked = !HCMGlobal.SavedConfig?.ClassicMode ?? true;
            settingswindow.modeclas.IsChecked = HCMGlobal.SavedConfig?.ClassicMode ?? false;
            settingswindow.LevelLockoutCheckbox.IsChecked = HCMGlobal.SavedConfig?.LockoutLevels ?? false;

            string[] levellistoptions = new string[] { "Level ID", "Acronym", "Full Name", "Icon" };

            foreach (string option in levellistoptions)
            {
                settingswindow.LevelListBox.Items.Add(option);
            }
            settingswindow.LevelListBox.SelectedIndex = HCMGlobal.SavedConfig?.LevelListOption ?? 0;


            settingswindow.ShowDialog();


          

            HCMGlobal.SavedConfig.ClassicMode = settingswindow.modeclas.IsChecked ?? false;
            HCMGlobal.SavedConfig.LockoutLevels = settingswindow.LevelLockoutCheckbox.IsChecked ?? false;
            HCMGlobal.SavedConfig.LevelListOption = settingswindow.LevelListBox.SelectedIndex;


            WriteConfig();
            RefreshLoa(sender, e);
            RefreshSel(sender, e);
            RefreshList(sender, e);
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

                    if (HCMGlobal.SavedConfig != null && File.Exists(HCMGlobal.CoreFolderPath + @"\core.bin") && HCMGlobal.CoreFolderPath != null)
                    {
                        var data = GetSaveFileMetadata(HCMGlobal.CoreFolderPath + @"\core.bin", HaloGame.Halo1);
                        H1CS_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode, HaloGame.Halo1);

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


                case 2: //h2
                    if (HCMGlobal.AttachedGame == "H2" && ValidCheck_H2())
                    {
                        //data to get; level code, diff, time

                        //first check double revert flag
                        int bytesWritten;
                        bool DRflag;
                        byte[] DRbuffer = new byte[1];
                        var data = new HaloSaveFileMetadata();

                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                        {
                            DRflag = Convert.ToBoolean(DRbuffer[0]);
                        }
                        else { Debug("oh no"); NullH2(); return; }

                        int offset;
                        if (!DRflag)
                        {
                            offset = HCMGlobal.LoadedOffsets.H2_CPData_DROffset1; //first cp
                        }
                        else
                        {
                            offset = HCMGlobal.LoadedOffsets.H2_CPData_DROffset2; //second cp
                        }

                        int[] addy = HCMGlobal.LoadedOffsets.H2_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                        addy[3] = offset;
                        //setup done, let's get our data
                        //levelcode
                        byte[] buffer = new byte[64];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.H2_CPData_LevelCode, buffer, buffer.Length, out bytesWritten))
                        {
                            char[] exceptions = new char[] { '_' };
                            data.LevelCode = Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                            data.LevelCode = data.LevelCode.Substring(data.LevelCode.LastIndexOf("\\") + 1);
                            data.LevelCode = data.LevelCode.Substring(data.LevelCode.LastIndexOf("\\") + 1);
                            data.LevelCode = String.Concat(data.LevelCode.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                        }
                        else { Debug("oh no"); NullH2(); return; }

                        //difficulty
                        if (LevelCodeToGameType(data.LevelCode))
                        {
                            data.Difficulty = (Difficulty)4;
                        }
                        else
                        {
                            buffer = new byte[1];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.H2_CPData_Difficulty, buffer, buffer.Length, out bytesWritten))
                            {
                                data.Difficulty = (Difficulty)buffer[0];
                            }
                            else { Debug("oh no"); NullH2(); return; }
                        }
                        //tickcount
                        buffer = new byte[4];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.H2_CPData_StartTick, buffer, buffer.Length, out bytesWritten))
                        {
                            data.StartTick = BitConverter.ToUInt32(buffer, 0);
                        }
                        else { Debug("oh no"); NullH2(); return; }

                       

                        //now assign to ui
                        H2CP_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode, HaloGame.Halo2);

                        H2CP_Loa_Time.Text = TickToTimeString(data.StartTick, true);

                        if (data.Difficulty != Difficulty.Invalid)
                            H2CP_Loa_DiffName.Source = new BitmapImage(new Uri($"images/H2/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                        if (LevelCodeToGameType(data.LevelCode))
                        {
                            H2CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/H2/mp.png", UriKind.Relative));
                        }
                        else
                        {
                            H2CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/H2/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                        }

                    }
                    else { Debug("oh no"); NullH2(); }
                    break;

                case 3: //H3
                    if (HCMGlobal.AttachedGame == "H3" && ValidCheck_H3())
                    {
                        //data to get; level code, diff, time

                        //first check double revert flag
                        int bytesWritten;
                        bool DRflag;
                        byte[] DRbuffer = new byte[1];
                        var data = new HaloSaveFileMetadata();

                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                        {
                            DRflag = Convert.ToBoolean(DRbuffer[0]);
                        }
                        else { Debug("oh no"); NullH3(); return; }

                        int offset;
                        if (!DRflag)
                        {
                            offset = HCMGlobal.LoadedOffsets.H3_CPData_DROffset1; //first cp
                        }
                        else
                        {
                            offset = HCMGlobal.LoadedOffsets.H3_CPData_DROffset2; //second cp
                        }

                        int[] addy = HCMGlobal.LoadedOffsets.H3_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                        addy[3] = offset;
                        //setup done, let's get our data
                        //levelcode
                        byte[] buffer = new byte[64];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.H3_CPData_LevelCode, buffer, buffer.Length, out bytesWritten))
                        {
                            char[] exceptions = new char[] { '_' };
                            data.LevelCode = Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                            data.LevelCode = data.LevelCode.Substring(data.LevelCode.LastIndexOf("\\") + 1);
                            data.LevelCode = data.LevelCode.Substring(data.LevelCode.LastIndexOf("\\") + 1);
                            data.LevelCode = String.Concat(data.LevelCode.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                        }
                        else { Debug("oh no"); NullH3(); return; }

                        //difficulty
                        if (LevelCodeToGameType(data.LevelCode))
                        {
                            data.Difficulty = (Difficulty)4;
                        }
                        else
                        {
                            buffer = new byte[1];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.H3_CPData_Difficulty, buffer, buffer.Length, out bytesWritten))
                            {
                                data.Difficulty = (Difficulty)buffer[0];
                            }
                            else { Debug("oh no"); NullH3(); return; }
                        }
                        //tickcount
                        buffer = new byte[4];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.H3_CPData_StartTick, buffer, buffer.Length, out bytesWritten))
                        {
                            data.StartTick = BitConverter.ToUInt32(buffer, 0);
                        }
                        else { Debug("oh no"); NullH3(); return; }



                        //now assign to ui
                        H3CP_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode, HaloGame.Halo3);

                        H3CP_Loa_Time.Text = TickToTimeString(data.StartTick, true);

                        if (data.Difficulty != Difficulty.Invalid)
                            H3CP_Loa_DiffName.Source = new BitmapImage(new Uri($"images/H3/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                        if (LevelCodeToGameType(data.LevelCode))
                        {
                            H3CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/H3/mp.png", UriKind.Relative));
                        }
                        else
                        {
                            H3CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/H3/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                        }

                    }
                    else { Debug("oh no"); NullH3(); }
                    break;


                case 4: //OD
                    if (HCMGlobal.AttachedGame == "OD" && ValidCheck_OD())
                    {
                        //data to get; level code, diff, time

                        //first check double revert flag
                        int bytesWritten;
                        bool DRflag;
                        byte[] DRbuffer = new byte[1];
                        var data = new HaloSaveFileMetadata();

                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                        {
                            DRflag = Convert.ToBoolean(DRbuffer[0]);
                        }
                        else { Debug("oh no"); NullOD(); return; }

                        int offset;
                        if (!DRflag)
                        {
                            offset = HCMGlobal.LoadedOffsets.OD_CPData_DROffset1; //first cp
                        }
                        else
                        {
                            offset = HCMGlobal.LoadedOffsets.OD_CPData_DROffset2; //second cp
                        }

                        int[] addy = HCMGlobal.LoadedOffsets.OD_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                        addy[3] = offset;
                        //setup done, let's get our data
                        //levelcode
                        byte[] buffer = new byte[64];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.OD_CPData_LevelCode, buffer, buffer.Length, out bytesWritten))
                        {
                            char[] exceptions = new char[] { '_' };
                            data.LevelCode = Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                            data.LevelCode = data.LevelCode.Substring(data.LevelCode.LastIndexOf("\\") + 1);
                            data.LevelCode = data.LevelCode.Substring(data.LevelCode.LastIndexOf("\\") + 1);
                            data.LevelCode = String.Concat(data.LevelCode.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                        }
                        else { Debug("oh no"); NullOD(); return; }

                        //difficulty
                        if (LevelCodeToGameType(data.LevelCode))
                        {
                            data.Difficulty = (Difficulty)4;
                        }
                        else
                        {
                            buffer = new byte[1];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.OD_CPData_Difficulty, buffer, buffer.Length, out bytesWritten))
                            {
                                data.Difficulty = (Difficulty)buffer[0];
                            }
                            else { Debug("oh no"); NullOD(); return; }
                        }
                        //tickcount
                        buffer = new byte[4];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.OD_CPData_StartTick, buffer, buffer.Length, out bytesWritten))
                        {
                            data.StartTick = BitConverter.ToUInt32(buffer, 0);
                        }
                        else { Debug("oh no"); NullOD(); return; }



                        //now assign to ui
                        ODCP_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode, HaloGame.HaloODST);

                        ODCP_Loa_Time.Text = TickToTimeString(data.StartTick, true);

                        if (data.Difficulty != Difficulty.Invalid)
                            ODCP_Loa_DiffName.Source = new BitmapImage(new Uri($"images/OD/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                        if (LevelCodeToGameType(data.LevelCode))
                        {
                            ODCP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/OD/mp.png", UriKind.Relative));
                        }
                        else
                        {
                            ODCP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/OD/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                        }

                    }
                    else { Debug("oh no"); NullOD(); }
                    break;


                case 5: //reach
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
                            offset = HCMGlobal.LoadedOffsets.HR_CPData_DROffset1; //first cp
                        }
                        else
                        {
                            offset = HCMGlobal.LoadedOffsets.HR_CPData_DROffset2; //second cp
                        }

                        int[] addy = HCMGlobal.LoadedOffsets.HR_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                        addy[3] = offset;
                        //setup done, let's get our data
                        //levelcode
                        byte[] buffer = new byte[64];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.HR_CPData_LevelCode, buffer, buffer.Length, out bytesWritten))
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
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.HR_CPData_Difficulty, buffer, buffer.Length, out bytesWritten))
                            {
                                data.Difficulty = (Difficulty)buffer[0];
                            }
                            else { Debug("oh no"); NullReach(); return; }
                        }
                        //tickcount
                        buffer = new byte[4];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.HR_CPData_StartTick, buffer, buffer.Length, out bytesWritten))
                        {
                            data.StartTick = BitConverter.ToUInt32(buffer, 0);
                        }
                        else { Debug("oh no"); NullReach(); return; }

                        //level start seed
                        buffer = new byte[4];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.HR_CPData_Seed, buffer, buffer.Length, out bytesWritten))
                        {
                            data.Seed = (Convert.ToString(BitConverter.ToUInt32(buffer, 0), 16)).ToUpper();
                        }
                        else { Debug("oh no"); NullReach(); return; }

                        //now assign to ui
                        HRCP_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode, HaloGame.HaloReach);

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


                case 6: //h4
                    if (HCMGlobal.AttachedGame == "H4" && ValidCheck_H4())
                    {
                        //data to get; level code, diff, time

                        //first check double revert flag
                        int bytesWritten;
                        bool DRflag;
                        byte[] DRbuffer = new byte[1];
                        var data = new HaloSaveFileMetadata();

                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                        {
                            DRflag = Convert.ToBoolean(DRbuffer[0]);
                        }
                        else { Debug("oh no"); NullH4(); return; }

                        int offset;
                        if (!DRflag)
                        {
                            offset = HCMGlobal.LoadedOffsets.H4_CPData_DROffset1; //first cp
                        }
                        else
                        {
                            offset = HCMGlobal.LoadedOffsets.H4_CPData_DROffset2; //second cp
                        }

                        int[] addy = HCMGlobal.LoadedOffsets.H4_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                        addy[3] = offset;
                        //setup done, let's get our data
                        //levelcode
                        byte[] buffer = new byte[64];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.H4_CPData_LevelCode, buffer, buffer.Length, out bytesWritten))
                        {
                            char[] exceptions = new char[] { '_' };
                            data.LevelCode = String.Concat(Encoding.UTF8.GetString(buffer, 0, buffer.Length).Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                        }
                        else { Debug("oh no"); NullH4(); return; }

                        //difficulty
                        if (LevelCodeToGameType(data.LevelCode))
                        {
                            data.Difficulty = (Difficulty)4;
                        }
                        else
                        {
                            buffer = new byte[1];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.H4_CPData_Difficulty, buffer, buffer.Length, out bytesWritten))
                            {
                                data.Difficulty = (Difficulty)buffer[0];
                            }
                            else { Debug("oh no"); NullH4(); return; }
                        }
                        //tickcount
                        buffer = new byte[4];
                        if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy) + HCMGlobal.LoadedOffsets.H4_CPData_StartTick, buffer, buffer.Length, out bytesWritten))
                        {
                            data.StartTick = BitConverter.ToUInt32(buffer, 0);
                        }
                        else { Debug("oh no"); NullH4(); return; }

                        //now assign to ui
                        H4CP_Loa_LevelName.Text = LevelCodeToFullName(data.LevelCode, HaloGame.Halo4);

                        H4CP_Loa_Time.Text = TickToTimeString(data.StartTick, true);

                        if (data.Difficulty != Difficulty.Invalid)
                            H4CP_Loa_DiffName.Source = new BitmapImage(new Uri($"images/H4/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                        if (LevelCodeToGameType(data.LevelCode))
                        {
                            H4CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/H4/mp.png", UriKind.Relative));
                        }
                        else
                        {
                            H4CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/H4/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                        }

                    }
                    else { Debug("oh no"); NullH4(); }



                    break;



                    void NullReach()
                    {
                        HRCP_Loa_LevelName.Text = "N/A";
                        HRCP_Loa_DiffName.Source = null;
                        HRCP_Loa_Time.Text = "N/A";
                        HRCP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
                        HRCP_Loa_Seed.Content = "Seed: N/A";
                    }

                    void NullH2()
                    {
                        H2CP_Loa_LevelName.Text = "N/A";
                        H2CP_Loa_DiffName.Source = null;
                        H2CP_Loa_Time.Text = "N/A";
                        H2CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
                    }

                    void NullH3()
                    {
                        H3CP_Loa_LevelName.Text = "N/A";
                        H3CP_Loa_DiffName.Source = null;
                        H3CP_Loa_Time.Text = "N/A";
                        H3CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
                    }

                    void NullOD()
                    {
                        ODCP_Loa_LevelName.Text = "N/A";
                        ODCP_Loa_DiffName.Source = null;
                        ODCP_Loa_Time.Text = "N/A";
                        ODCP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
                    }

                    void NullH4()
                    {
                        H4CP_Loa_LevelName.Text = "N/A";
                        H4CP_Loa_DiffName.Source = null;
                        H4CP_Loa_Time.Text = "N/A";
                        H4CP_Loa_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
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
                            H1CS_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode, HaloGame.Halo1);

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


                    //no lockout for h1cs

                    break;

                case 2: //h2
                    if (H2CP_MainList.SelectedItem != null)
                    {
                        var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex);
                        System.Type type = item.GetType();
                        string s = (string)type.GetProperty("Name").GetValue(item, null);
                        var pathtotest = HCMGlobal.H2CheckpointPath + @"\" + s + @".bin";

                        if (File.Exists(pathtotest))
                        {
                            var data = GetSaveFileMetadata(pathtotest, HaloGame.Halo2);
                            H2CP_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode, HaloGame.Halo2);

                            if (data.Difficulty != Difficulty.Invalid)
                                H2CP_Sel_DiffName.Source = new BitmapImage(new Uri($"images/H2/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                            H2CP_Sel_Time.Text = TickToTimeString(data.StartTick, true);
                            H2CP_Sel_FileName.Text = s;

                            if (LevelCodeToGameType(data.LevelCode))
                            {
                                H2CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/H2/mp.png", UriKind.Relative));
                            }
                            else
                            {
                                H2CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/H2/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                            }

                           
                        }
                    }
                    else
                    {
                        H2CP_Sel_LevelName.Text = "N/A";
                        H2CP_Sel_DiffName.Source = null;
                        H2CP_Sel_Time.Text = "N/A";
                        H2CP_Sel_FileName.Text = "N/A";
                        H2CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
                    }


                  
                    //so the bug is that SetEnabledUI is undoing our work here
                    if (HCMGlobal.SavedConfig == null || HCMGlobal.SavedConfig.LockoutLevels == true && (H2CP_MainList.SelectedItem == null || H2CP_MainList.SelectedItem.GetType().GetProperty("LevelCode").GetValue(H2CP_MainList.SelectedItem, null) as string
                   != HCMGlobal.AttachedLevel))
                    {
                        Debug("lockout the thing");
                        H2CP_Sel_InjectButton.IsEnabled = false;
                        H2CP_Sel_InjectRevertButton.IsEnabled = false;
                    }
                    else
                    {
                        Debug("unlockout the thingy");
                        H2CP_Sel_InjectButton.IsEnabled = true;
                        H2CP_Sel_InjectRevertButton.IsEnabled = true;
                    }
                    break;

                case 3: //H3
                    if (H3CP_MainList.SelectedItem != null)
                    {
                        var item = H3CP_MainList.Items.GetItemAt(H3CP_MainList.SelectedIndex);
                        System.Type type = item.GetType();
                        string s = (string)type.GetProperty("Name").GetValue(item, null);
                        var pathtotest = HCMGlobal.H3CheckpointPath + @"\" + s + @".bin";

                        if (File.Exists(pathtotest))
                        {
                            var data = GetSaveFileMetadata(pathtotest, HaloGame.Halo3);
                            H3CP_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode, HaloGame.Halo3);

                            if (data.Difficulty != Difficulty.Invalid)
                                H3CP_Sel_DiffName.Source = new BitmapImage(new Uri($"images/H3/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                            H3CP_Sel_Time.Text = TickToTimeString(data.StartTick, true);
                            Debug("AHHH" + TickToTimeString(data.StartTick, true));
                            H3CP_Sel_FileName.Text = s;

                            if (LevelCodeToGameType(data.LevelCode))
                            {
                                H3CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/H3/mp.png", UriKind.Relative));
                            }
                            else
                            {
                                H3CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/H3/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                            }


                        }
                    }
                    else
                    {
                        H3CP_Sel_LevelName.Text = "N/A";
                        H3CP_Sel_DiffName.Source = null;
                        H3CP_Sel_Time.Text = "N/A";
                        H3CP_Sel_FileName.Text = "N/A";
                        H3CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
                    }


                    if (HCMGlobal.SavedConfig == null || HCMGlobal.SavedConfig.LockoutLevels == true && (H3CP_MainList.SelectedItem == null || H3CP_MainList.SelectedItem.GetType().GetProperty("LevelCode").GetValue(H3CP_MainList.SelectedItem, null) as string
                   != HCMGlobal.AttachedLevel))
                    {
                        Debug("lockout the thing");
                        H3CP_Sel_InjectButton.IsEnabled = false;
                        H3CP_Sel_InjectRevertButton.IsEnabled = false;
                    }
                    else
                    {
                        Debug("unlockout the thingy");
                        H3CP_Sel_InjectButton.IsEnabled = true;
                        H3CP_Sel_InjectRevertButton.IsEnabled = true;
                    }
                    break;

                case 4: //OD
                    if (ODCP_MainList.SelectedItem != null)
                    {
                        var item = ODCP_MainList.Items.GetItemAt(ODCP_MainList.SelectedIndex);
                        System.Type type = item.GetType();
                        string s = (string)type.GetProperty("Name").GetValue(item, null);
                        var pathtotest = HCMGlobal.ODCheckpointPath + @"\" + s + @".bin";

                        if (File.Exists(pathtotest))
                        {
                            var data = GetSaveFileMetadata(pathtotest, HaloGame.HaloODST);
                            ODCP_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode, HaloGame.HaloODST);

                            if (data.Difficulty != Difficulty.Invalid)
                                ODCP_Sel_DiffName.Source = new BitmapImage(new Uri($"images/OD/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                            ODCP_Sel_Time.Text = TickToTimeString(data.StartTick, true);
                            Debug("AHHH" + TickToTimeString(data.StartTick, true));
                            ODCP_Sel_FileName.Text = s;

                            if (LevelCodeToGameType(data.LevelCode))
                            {
                                ODCP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/OD/mp.png", UriKind.Relative));
                            }
                            else
                            {
                                ODCP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/OD/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                            }


                        }
                    }
                    else
                    {
                        ODCP_Sel_LevelName.Text = "N/A";
                        ODCP_Sel_DiffName.Source = null;
                        ODCP_Sel_Time.Text = "N/A";
                        ODCP_Sel_FileName.Text = "N/A";
                        ODCP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
                    }


                    if (HCMGlobal.SavedConfig == null || HCMGlobal.SavedConfig.LockoutLevels == true && (ODCP_MainList.SelectedItem == null || ODCP_MainList.SelectedItem.GetType().GetProperty("LevelCode").GetValue(ODCP_MainList.SelectedItem, null) as string
                   != HCMGlobal.AttachedLevel))
                    {
                        Debug("lockout the thing");
                        if (ODCP_MainList.SelectedItem != null)
                        {
                            Debug("obv: " + (ODCP_MainList.SelectedItem.GetType().GetProperty("LevelCode").GetValue(ODCP_MainList.SelectedItem, null) as string));
                        }
                        Debug("exp: " + HCMGlobal.AttachedLevel);
                        ODCP_Sel_InjectButton.IsEnabled = false;
                        ODCP_Sel_InjectRevertButton.IsEnabled = false;
                    }
                    else
                    {
                        Debug("unlockout the thingy");
                        ODCP_Sel_InjectButton.IsEnabled = true;
                        ODCP_Sel_InjectRevertButton.IsEnabled = true;
                    }
                    break;

                case 5: //reach
                    if (HRCP_MainList.SelectedItem != null)
                    {
                        var item = HRCP_MainList.Items.GetItemAt(HRCP_MainList.SelectedIndex);
                        System.Type type = item.GetType();
                        string s = (string)type.GetProperty("Name").GetValue(item, null);
                        var pathtotest = HCMGlobal.HRCheckpointPath + @"\" + s + @".bin";

                        if (File.Exists(pathtotest))
                        {
                            var data = GetSaveFileMetadata(pathtotest, HaloGame.HaloReach);
                            HRCP_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode, HaloGame.HaloReach);

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


                    if (HCMGlobal.SavedConfig == null || HCMGlobal.SavedConfig.LockoutLevels == true && (HRCP_MainList.SelectedItem == null || HRCP_MainList.SelectedItem.GetType().GetProperty("LevelCode").GetValue(HRCP_MainList.SelectedItem, null) as string
                   != HCMGlobal.AttachedLevel))
                    {
                        Debug("lockout the thing");
                        HRCP_Sel_InjectButton.IsEnabled = false;
                        HRCP_Sel_InjectRevertButton.IsEnabled = false;
                    }
                    else
                    {
                        Debug("unlockout the thingy");
                        HRCP_Sel_InjectButton.IsEnabled = true;
                        HRCP_Sel_InjectRevertButton.IsEnabled = true;
                    }
                    break;

                case 6: //h4
                    if (H4CP_MainList.SelectedItem != null)
                    {
                        var item = H4CP_MainList.Items.GetItemAt(H4CP_MainList.SelectedIndex);
                        System.Type type = item.GetType();
                        string s = (string)type.GetProperty("Name").GetValue(item, null);
                        var pathtotest = HCMGlobal.H4CheckpointPath + @"\" + s + @".bin";

                        if (File.Exists(pathtotest))
                        {
                            var data = GetSaveFileMetadata(pathtotest, HaloGame.Halo4);
                            H4CP_Sel_LevelName.Text = LevelCodeToFullName(data.LevelCode, HaloGame.Halo4);

                            if (data.Difficulty != Difficulty.Invalid)
                                H4CP_Sel_DiffName.Source = new BitmapImage(new Uri($"images/H4/diff_{(int)data.Difficulty}.png", UriKind.Relative));

                            H4CP_Sel_Time.Text = TickToTimeString(data.StartTick, true);
                            H4CP_Sel_FileName.Text = s;

                            if (LevelCodeToGameType(data.LevelCode))
                            {
                                H4CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/H4/mp.png", UriKind.Relative));
                            }
                            else
                            {
                                H4CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/H4/{data.LevelCode}_{HCMGlobal.ImageModeSuffix}.png", UriKind.Relative));
                            }

                        }
                    }
                    else
                    {
                        H4CP_Sel_LevelName.Text = "N/A";
                        H4CP_Sel_DiffName.Source = null;
                        H4CP_Sel_Time.Text = "N/A";
                        H4CP_Sel_FileName.Text = "N/A";
                        H4CP_Sel_LevelImage.Source = new BitmapImage(new Uri($"images/nofile.png", UriKind.Relative));
                    }


                    if (HCMGlobal.SavedConfig == null || HCMGlobal.SavedConfig.LockoutLevels == true && (H4CP_MainList.SelectedItem == null || H4CP_MainList.SelectedItem.GetType().GetProperty("LevelCode").GetValue(H4CP_MainList.SelectedItem, null) as string
                   != HCMGlobal.AttachedLevel))
                    {
                        string teststring = "null";
                        if (H4CP_MainList.SelectedItem != null)
                        {
                            teststring = H4CP_MainList.SelectedItem.GetType().GetProperty("LevelCode").GetValue(H4CP_MainList.SelectedItem, null) as string;
                        }
                        Debug("lockout the thing");
                        Debug("why lockout? attached level: " + HCMGlobal.AttachedLevel.ToString() + ", sel cp level: " + teststring);
                        H4CP_Sel_InjectButton.IsEnabled = false;
                        H4CP_Sel_InjectRevertButton.IsEnabled = false;
                    }
                    else
                    {
                        Debug("unlockout the thingy");
                        H4CP_Sel_InjectButton.IsEnabled = true;
                        H4CP_Sel_InjectRevertButton.IsEnabled = true;
                    }
                    break;

                default:
                    break;

            }

        }

        private void HRCP_Sel_LevelImage_ImageFailed(object sender, ExceptionRoutedEventArgs e) //christ I can't even remember what this is for
        {
            throw new NotImplementedException();
        }

        private void RefreshList(object sender, RoutedEventArgs e)
        {

            

            List<string> FilesPost = new List<string>();

            //let's only update the active tab (we'll later also add a RefreshList whenever the tab is changed)
            Debug(TabList.SelectedIndex.ToString());
            string savepath = HCMGlobal.H1CoreSavePath;
            var game = HaloGame.Halo1;
            var list = CS_MainList;
            var listlabel = H1CS_MainList_Label;
            var oldselected = CS_MainList.SelectedIndex;
            var collection = Halo1CoreSaves;
            switch (TabList.SelectedIndex)
            {
                case 0: //h1 cores
                    
                    savepath = HCMGlobal.H1CoreSavePath;
                    game = HaloGame.Halo1;
                    list = CS_MainList;
                    listlabel = H1CS_MainList_Label;
                    oldselected = CS_MainList.SelectedIndex;
                    collection = Halo1CoreSaves;
                    Halo1CoreSaves.Clear();
                    break;

                case 2: //halo 2
                   
                    savepath = HCMGlobal.H2CheckpointPath;
                    game = HaloGame.Halo2;
                    list = H2CP_MainList;
                    listlabel = H2CP_MainList_Label;
                    oldselected = H2CP_MainList.SelectedIndex;
                    collection = Halo2Checkpoints;
                    Halo2Checkpoints.Clear();
                    break;

                case 3: //halo 3

                    savepath = HCMGlobal.H3CheckpointPath;
                    game = HaloGame.Halo3;
                    list = H3CP_MainList;
                    listlabel = H3CP_MainList_Label;
                    oldselected = H3CP_MainList.SelectedIndex;
                    collection = Halo3Checkpoints;
                    Halo3Checkpoints.Clear();
                    break;

                case 4: //odst

                    savepath = HCMGlobal.ODCheckpointPath;
                    game = HaloGame.HaloODST;
                    list = ODCP_MainList;
                    listlabel = ODCP_MainList_Label;
                    oldselected = ODCP_MainList.SelectedIndex;
                    collection = HaloODSTCheckpoints;
                    HaloODSTCheckpoints.Clear();
                    break;

                case 5: //halo reach
                    
                    savepath = HCMGlobal.HRCheckpointPath;
                    game = HaloGame.HaloReach;
                    list = HRCP_MainList;
                    listlabel = HRCP_MainList_Label;
                    oldselected = HRCP_MainList.SelectedIndex;
                    collection = HaloReachCheckpoints;
                    HaloReachCheckpoints.Clear();
                    break;

                case 6: //halo 4

                    savepath = HCMGlobal.H4CheckpointPath;
                    game = HaloGame.Halo4;
                    list = H4CP_MainList;
                    listlabel = H4CP_MainList_Label;
                    oldselected = H4CP_MainList.SelectedIndex;
                    collection = Halo4Checkpoints;
                    Halo4Checkpoints.Clear();
                    break;

                default:
                    return;

            }

            if (Directory.Exists(savepath)) // make sure path is valid
            {
                DirectoryInfo dir = new DirectoryInfo(savepath);
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
                    listlabel.Content = "";
                    foreach (string File in FilesPost)
                    {
                        var data = GetSaveFileMetadata(savepath + "/" + File, game);
                        data.Name = File.Substring(0, File.Length - 4);
                        //could add a switch here for levellistoption
                        //but how to add image as option hm.
                        //data.LevelCode = null;

                        //data.TextVisibility = Visibility.Visible;
                        //data.ImageVisibility = Visibility.Hidden;

                        if (LevelCodeToGameType(data.LevelCode))
                        {
                            data.LevelNameDisplay = data.LevelCode;
                        }
                        else
                        {
                            switch (HCMGlobal.SavedConfig?.LevelListOption ?? 0)
                            {

                                default:
                                case 0: //levelcode
                                    data.LevelNameDisplay = data.LevelCode;
                                    break;

                                case 1: //anronym
                                        //TODO: make a dictionary for this
                                    data.LevelNameDisplay = "NAH";
                                    break;

                                case 2: //fullname
                                        //TODO: make a dictionary for this
                                    data.LevelNameDisplay = "Indubitably Not Ready Yet";
                                    break;

                                case 3: //icon //levelnamedisplay won't be used 
                                    data.LevelListImageURL = data.LevelListImage;
                                    data.TextVisibility = Visibility.Hidden;
                                    data.ImageVisibility = Visibility.Visible;
                                    break;
                            }
                        }



                        collection.Add(data);
                    }



                    list.SelectedIndex = oldselected;
                    GridView gv = list.View as GridView;
                    UpdateColumnWidths(gv);

                }
                else
                {
                    listlabel.Content = "No backup saves in local folder.";
                }



            }
            else
            {
                listlabel.Content = "save folder path is invalid, check Settings.";
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
            public string DifficultyImageH2 => $"images/H2/diff_{(int)Difficulty}.png";
            public string DifficultyImageH3 => $"images/H3/diff_{(int)Difficulty}.png";
            public string DifficultyImageHR => $"images/HR/diff_{(int)Difficulty}.png";

            public string DifficultyImageOD => $"images/OD/diff_{(int)Difficulty}.png";

            public string DifficultyImageH4 => $"images/H4/diff_{(int)Difficulty}.png";

            //public string LevelListImage => $"images/H1/a10_clas.png";
            public string LevelListImage => $"images/H1/{LevelCode}_{HCMGlobal.ImageModeSuffix}.png";
            public string LevelListImageURL { get; set; }

            public string LevelNameDisplay { get; set; }
            public string TimeString => TickToTimeString(StartTick, false);

            //added this for sortsaves manip
            public DateTime LastWriteTime { get; set; }
            public bool IsMultiplayer { get; set; }

            public Visibility TextVisibility { get; set; } = Visibility.Visible;
            public Visibility ImageVisibility { get; set; } = Visibility.Hidden;
        }

        private HaloSaveFileMetadata GetSaveFileMetadata(string saveFilePath, HaloGame game)
        {
            HaloSaveFileMetadata metadata = new HaloSaveFileMetadata() { Game = game };

            int offsetLevelCode;
            int offsetStartTick;
            int offsetDifficulty;
            int offsetSeed = 0;


            if (HCMGlobal.LoadedOffsets == null) //use some default values if we haven't loaded offsets json yet
            {
                switch (game)
                {
                    case HaloGame.Halo1:
                        offsetLevelCode = 11;
                        offsetStartTick = 756;
                        offsetDifficulty = 294;
                        break;
                    case HaloGame.Halo2:
                        offsetLevelCode = 23;
                        offsetStartTick = 10824;
                        offsetDifficulty = 974;
                        break;
                    case HaloGame.Halo3:
                        offsetLevelCode = 28;
                        offsetStartTick = 0x3E0220;
                        offsetDifficulty = 0x274;
                        break;
                    case HaloGame.HaloODST:
                        offsetLevelCode = 0x15;
                        offsetStartTick = 0x004002E0;
                        offsetDifficulty = 0x26C;
                        break;
                    case HaloGame.HaloReach:
                        offsetLevelCode = 0xFD73;
                        offsetStartTick = 0x10FD54;
                        offsetDifficulty = 0xFEFC;
                        offsetSeed = 0x148;
                        break;
                    case HaloGame.Halo4:
                        offsetLevelCode = 0x1C96F;
                        offsetStartTick = 0x2E424;
                        offsetDifficulty = 0x1CB00;
                        break;
                    default:
                        throw new NotSupportedException();
                }
            }
            else
            {
                switch (game)
                {
                    case HaloGame.Halo1:
                        offsetLevelCode = HCMGlobal.LoadedOffsets.H1_CPData_LevelCode;
                        offsetStartTick = HCMGlobal.LoadedOffsets.H1_CPData_StartTick;
                        offsetDifficulty = HCMGlobal.LoadedOffsets.H1_CPData_Difficulty;
                        break;
                    case HaloGame.Halo2:
                        offsetLevelCode = HCMGlobal.LoadedOffsets.H2_CPData_LevelCode;
                        offsetStartTick = HCMGlobal.LoadedOffsets.H2_CPData_StartTick;
                        offsetDifficulty = HCMGlobal.LoadedOffsets.H2_CPData_Difficulty;
                        break;
                    case HaloGame.Halo3:
                        offsetLevelCode = HCMGlobal.LoadedOffsets.H3_CPData_LevelCode;
                        offsetStartTick = HCMGlobal.LoadedOffsets.H3_CPData_StartTick;
                        offsetDifficulty = HCMGlobal.LoadedOffsets.H3_CPData_Difficulty;
                        break;
                    case HaloGame.HaloODST:
                        offsetLevelCode = HCMGlobal.LoadedOffsets.OD_CPData_LevelCode;
                        offsetStartTick = HCMGlobal.LoadedOffsets.OD_CPData_StartTick;
                        offsetDifficulty = HCMGlobal.LoadedOffsets.OD_CPData_Difficulty;
                        break;
                    case HaloGame.HaloReach:
                        offsetLevelCode = HCMGlobal.LoadedOffsets.HR_CPData_LevelCode;
                        offsetStartTick = HCMGlobal.LoadedOffsets.HR_CPData_StartTick;
                        offsetDifficulty = HCMGlobal.LoadedOffsets.HR_CPData_Difficulty;
                        offsetSeed = HCMGlobal.LoadedOffsets.HR_CPData_Seed;
                        break;
                    case HaloGame.Halo4:
                        offsetLevelCode = HCMGlobal.LoadedOffsets.H4_CPData_LevelCode;
                        offsetStartTick = HCMGlobal.LoadedOffsets.H4_CPData_StartTick;
                        offsetDifficulty = HCMGlobal.LoadedOffsets.H4_CPData_Difficulty;
                        break;
                    default:
                        throw new NotSupportedException();
                }
            }
            

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
                            metadata.LevelCode = new string(readBinary.ReadChars(64));
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
                        if (HCMGlobal.AttachedGame != "H1") //shit constantly pops up for h1 and doesn't matter
                        {
                            MessageBox.Show(ex.ToString());
                        }
                        else
                        {
                            Debug("EXCEPTION: " + ex.ToString());
                        }


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

        readonly Dictionary<string, string> LevelCodeToNameH1 = new Dictionary<string, string>()
        { 
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

        };

        readonly Dictionary<string, string> LevelCodeToNameH2 = new Dictionary<string, string>()
        { 

            // Halo 2
            //SP
            { "00a_introduction", "The Heretic" },
            { "01a_tutorial", "The Armory" },
            { "01b_spacestation", "Cairo Station" },
            { "03a_oldmombasa", "Outskirts" },
            { "03b_newmombasa", "Metropolis" },
            { "04a_gasgiant", "The Arbiter" },
            { "04b_floodlab", "The Oracle" },
            { "05a_deltaapproach", "Delta Halo" },
            { "05b_deltatowers", "Regret" },
            { "06a_sentinelwalls", "Sacred Icon" },
            { "06b_floodzone", "Quarantine Zone" },
            { "07a_highcharity", "Gravemind" },
            { "07b_forerunnership", "High Charity" },
            { "08a_deltacliffs", "Uprising" },
            { "08b_deltacontrol", "The Great Journey" },
            //MP
            { "ascension", "Ascension" },
            { "backwash", "Backwash" },
            { "beavercreek", "placeholder" },
            { "burial_mounds", "Burial Mounds" },
            { "coagulation", "Coagulation" },
            { "colossus", "Colossus" },
            { "containment", "Containment" },
            { "cyclotron", "Ivory Tower" },
            { "deltatap", "Sanctuary" },
            { "derelict", "Desolation" },
            { "dune", "Relic" },
            { "elongation", "Elongation" },
            { "foundation", "Foundation" },
            { "gemini", "Gemini" },
            { "headlong", "Headlong" },
            { "highplains", "Tombstone" },
            { "lockout", "Lockout" },
            { "midship", "Midship" },
            { "needle", "Uplift" },
            { "street_sweeper", "District" },
            { "triplicate", "Terminal" },
            { "turf", "Turf" },
            { "warlock", "Warlock" },
            { "waterworks", "Waterworks" },
            { "zanzibar", "Zanzibar" },

        };

        readonly Dictionary<string, string> LevelCodeToNameH3 = new Dictionary<string, string>()
        { 


             // Halo 3
            //SP
            { "005_intro", "Arrival" },
            { "010_jungle", "Sierra 117" },
            { "020_base", "Crow's Nest" },
            { "030_outskirts", "Tsavo Highway" },
            { "040_voi", "The Storm" },
            { "050_floodvoi", "Floodgate" },
            { "070_waste", "The Ark" },
            { "100_citadel", "The Covenant" },
            { "110_hc", "Cortana" },
            { "120_halo", "Halo" },
            { "130_epilogue", "Epilogue" },
            //MP
            { "armory", "Rat's Nest" },
            { "bunkerworld", "Standoff" },
            { "chill", "Narrows" },
            { "chillout", "Cold Storage" },
            { "construct", "Construct" },
            { "cyberdyne", "The Pit" },
            { "deadlock", "Highground" },
            { "descent", "Assembly" },
            { "docks", "Longshore" },
            { "fortress", "Citadel" },
            { "ghosttown", "Ghost Town" },
            { "guardian", "Guardian" },
            { "isolation", "Isolation" },
            { "lockout", "Blackout" },
            { "midship", "Heretic" },
            { "riverworld", "Valhalla" },
            { "salvation", "Epitaph" },
            { "sandbox", "Sandbox" },
            { "shrine", "Sandtrap" },
            { "sidewinder", "Avalanche" },
            { "snowbound", "Snowbound" },
            { "spacecamp", "Orbital" },
            { "warehouse", "Foundry" },
            { "zanzibar", "Last Resort" },

        };

        readonly Dictionary<string, string> LevelCodeToNameOD = new Dictionary<string, string>()
        { 


             // ODST
            //SP -- need to double check these. ODST is weird. also double check cases
            { "c100", "Prepare to Drop" }, //aka the cutscene, not the MS1 level
            { "c200", "Epilogue" },
            { "h100", "Mombasa Streets" },
            { "l200", "Data Hive" },
            { "l300", "Coastal Highway" },
            { "sc100", "Tayari Plaza" },
            { "sc110", "Uplift Reserve" },
            { "sc120", "Kinzingo Boulevard" },
            { "sc130", "ONI Alpha Site" },
            { "sc140", "NMPD HQ" },
            { "sc150", "Kikowani Station" },
            //MP
            //imagine

        };

        readonly Dictionary<string, string> LevelCodeToNameHR = new Dictionary<string, string>()
        {

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


        readonly Dictionary<string, string> LevelCodeToNameH4 = new Dictionary<string, string>()
        { 


             // Halo 4
            //SP
            { "m05_prologue", "Prologue" },
            { "m10_crash", "Dawn" },
            { "m020", "Requiem" },
            { "m30_cryptum", "Forerunner" },
            { "m40_invasion", "Reclaimer" },
            { "m60_rescue", "Infinity" },
            { "m70_liftoff", "Shutdown" },
            { "m80_delta", "Composer" },
            { "m90_sacrifice", "Midnight" },
            { "m95_epilogue", "Epilogue" },
            //Spartan Ops
            { "ff87_chopperbowl", "Quarry" },
            { "ff86_sniperalley", "Sniper Alley" },
            { "ff90_fortsw", "Fortress" },
            { "ff84_temple", "The Refuge" },
            { "ff82_scurve", "The Cauldron" },
            { "ff81_courtyard", "The Gate" },
            { "ff91_complex", "The Galileo" },
            { "ff92_valhalla", "Two Giants" },
            { "dlc01_factory", "Lockup" },
            { "ff151_mezzanine", "Control" },
            { "ff153_caverns", "Warrens" },
            { "ff152_vortex", "Cyclone" },
            { "ff155_breach", "Harvester" },
            { "ff154_hillside", "Apex" },
            { "dlc01_engine", "Infinity" },
            //MP -- I haven't double checked that all of these are correct
            { "ca_blood_cavern", "Abandon" },
            { "ca_blood_crash", "Exile" },
            { "ca_canyon", "Meltdown" },
            { "ca_forge_bonzanza", "Impact" },
            { "ca_forge_erosion", "Erosion" },
            { "ca_forge_ravine", "Ravine" },
            { "ca_gore_valley", "Longbow" },
            { "ca_redoubt", "Vortex" },
            { "ca_tower", "Solace" },
            { "ca_warhouse", "Adrift" },
            { "ca_wraparound", "Haven" },
            { "dlc_forge_island", "Forge Island" },
            { "dlc dejewel", "Shatter" },
            { "dlc dejunkyard", "Wreckage" },
            { "z05_cliffside", "Complex" },
            { "zd_02_grind", "Harvest" },
            { "ca deadlycrossing", "Monolith" },
            { "ca_port", "Landfall" },
            { "ca_rattler", "Skyline" },
            { "ca_basin", "Outcast" },
            { "ca_highrise", "Perdition" },
            { "ca_spiderweb", "Daybreak" },
            { "ca_creeper", "Pitfall" },
            { "ca_dropoff", "Daybreak" }, //nfi why there's two daybreaks

        };

        readonly static Dictionary<string, bool> LevelCodeToType = new Dictionary<string, bool>()
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
            { "00a_introduction", false },
            { "01a_tutorial", false },
            { "01b_spacestation", false },
            { "03a_oldmombasa", false },
            { "03b_newmombasa", false },
            { "04a_gasgiant", false },
            { "04b_floodlab", false },
            { "05a_deltaapproach", false },
            { "05b_deltatowers", false },
            { "06a_sentinelwalls", false },
            { "06b_floodzone", false },
            { "07a_highcharity", false },
            { "07b_forerunnership", false },
            { "08a_deltacliffs", false },
            { "08b_deltacontrol", false },
            //MP
            { "ascension", true },
            { "backwash", true },
            //{ "beavercreek", true },
            { "burial_mounds", true },
            { "coagulation", true },
            { "colossus", true },
            { "containment", true },
            { "cyclotron", true },
            { "deltatap", true },
            { "derelict ", true },
            { "dune", true },
            { "elongation", true },
            { "foundation", true },
            { "gemini", true },
            { "headlong", true },
            { "highplains", true },
            { "lockout", true },
            { "midship", true },
            { "needle", true },
            { "street_sweeper", true },
            { "triplicate", true },
            { "turf", true },
            { "warlock", true },
            { "waterworks", true },
            { "zanzibar", true },
            //tbd

                         // Halo 3
            //SP
            { "005_intro", false },
            { "010_jungle",  false },
            { "020_base",  false },
            { "030_outskirts",  false },
            { "040_voi",  false },
            { "050_floodvoi",  false},
            { "070_waste",  false },
            { "100_citadel", false },
            { "110_hc", false },
            { "120_halo", false },
            { "130_epilogue",  false },
            //MP
            { "armory", true },
            { "bunkerworld", true },
            { "chill",true },
            //{ "chillout", true },
            { "construct", true },
            { "cyberdyne", true },
            { "deadlock", true },
            { "descent", true },
            { "docks", true },
            { "fortress", true },
            { "ghosttown",true },
            { "guardian", true },
            { "isolation", true },
            //{ "lockout", true },
            //{ "midship", true },
            { "riverworld", true },
            { "salvation", true },
            { "sandbox", true },
            { "shrine", true },
            //{ "sidewinder", true },
            { "snowbound", true },
            { "spacecamp", true },
            { "warehouse", true },
            //{ "zanzibar", true },

            //ODST
                        { "c100", false },
            { "c200", false },
            { "h100", false },
            { "l200", false },
            { "l300", false },
            { "sc100", false },
            { "sc110", false },
            { "sc120", false },
            { "sc130", false },
            { "sc140", false },
            { "sc150", false },

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


                         // Halo 4
            //SP
            { "m05_prologue", false },
            { "m10_crash", false },
            { "m020", false },
            { "m30_cryptum", false },
            { "m60_rescue", false },
            { "m40_invasion", false },
            { "m70_liftoff", false },
            { "m80_delta", false },
            { "m90_sacrifice", false },
            { "m95_epilogue", false },
            //Spartan Ops -- gonna define these as MP so I don't have to do level icons for em all
            { "ff87_chopperbowl", true },
            { "ff86_sniperalley", true },
            { "ff90_fortsw", true },
            { "ff84_temple", true },
            { "ff82_scurve", true },
            { "ff81_courtyard", true },
            { "ff91_complex", true },
            { "ff92_valhalla", true },
            { "dlc01_factory", true },
            { "ff151_mezzanine", true },
            { "ff153_caverns", true },
            { "ff152_vortex", true },
            { "ff155_breach", true },
            { "ff154_hillside", true },
            { "dlc01_engine",true },
            //MP
            { "ca_blood_cavern", true },
            { "ca_blood_crash", true },
            { "ca_canyon", true },
            { "ca_forge_bonzanza", true },
            { "ca_forge_erosion", true },
            { "ca_forge_ravine", true },
            { "ca_gore_valley", true },
            { "ca_redoubt", true },
            { "ca_tower", true },
            { "ca_warhouse", true },
            { "ca_wraparound", true },
            { "dlc_forge_island", true },
            { "dlc dejewel", true },
            { "dlc dejunkyard", true },
            { "z05_cliffside", true },
            { "zd_02_grind", true },
            { "ca deadlycrossing", true },
            { "ca_port", true },
            { "ca_rattler", true },
            { "ca_basin", true },
            { "ca_highrise", true },
            { "ca_spiderweb", true },
            { "ca_creeper", true },
            { "ca_dropoff", true }, 

        };

        public string LevelCodeToFullName(string code, HaloGame game)
        {
            string Name;
            switch (game)
            {
                case HaloGame.Halo1:
                    if (LevelCodeToNameH1.TryGetValue(code, out Name))
                    {
                        return Name;
                    }
                    return code;

                case HaloGame.Halo2:
                    if (LevelCodeToNameH2.TryGetValue(code, out Name))
                    {
                        return Name;
                    }
                    return code;

                case HaloGame.Halo3:
                    if (LevelCodeToNameH3.TryGetValue(code, out Name))
                    {
                        return Name;
                    }
                    return code;

                case HaloGame.HaloODST:
                    if (LevelCodeToNameOD.TryGetValue(code, out Name))
                    {
                        return Name;
                    }
                    return code;

                case HaloGame.HaloReach:
                    if (LevelCodeToNameHR.TryGetValue(code, out Name))
                    {
                        return Name;
                    }
                    return code;

                case HaloGame.Halo4:
                    if (LevelCodeToNameH4.TryGetValue(code, out Name))
                    {
                        return Name;
                    }
                    return code;

                default:
                    return code;
            }



        }

        public static bool LevelCodeToGameType(string code)
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

        

        private void ForceCPButton_Click(object sender, RoutedEventArgs e)
        {
            //figure out which game this was for
            FrameworkElement parent = (FrameworkElement)((Button)sender).Parent;
            string parent_name = parent.Name;

            var (success, error) = ForceCPFunction(parent_name);

            if (success == false)
            {
                Debug("Failed to force save, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to force save, " + error, "Error", System.Windows.MessageBoxButton.OK);
                return;
            }
            RefreshLoa(sender, e);

        }

        private static (bool success, string error) ForceCPFunction(string game)
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

                case "H2CP":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "H2" && ValidCheck_H2())
                        {

                            //next, the custom message stuff
                            //null the cp messagecall
                            byte[] buffer = new byte[5] { 0x90, 0x90, 0x90, 0x90, 0x90 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h2: custom message 0: success");
                            }
                            else
                            {
                                return (false, "message 0 failure");
                            }


                            //acquire the current tickcount
                            buffer = new byte[4];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_TickCounter[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h2: custom message 1: success");
                            }
                            else
                            {
                                FixMessageCall();
                                return (false, "message 1 failure");
                            }

                            //buffer will be equal to the tickcounter so just paste it into the new spot
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_Message[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h2: custom message 2: success");
                            }
                            else
                            {
                                FixMessageCall();
                                return (false, "message 2 failure");
                            }

                            buffer = new byte["Custom Checkpoint... done".Length * 2]; //halo uses widechar for it's message strings, so double the length needed.
                            buffer = Encoding.Unicode.GetBytes("Custom Checkpoint... done");
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_Message[Convert.ToInt32(HCMGlobal.WinFlag)]) + 0x4, buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h2: custom message 3: success");
                            }
                            else
                            {
                                FixMessageCall();
                                return (false, "message 3 failure");
                            }

                            buffer = new byte[4] { 0, 0, 1, 0 }; //setting showmessage flag to true
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_Message[Convert.ToInt32(HCMGlobal.WinFlag)]) + 0x80, buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h2: custom message 4: success");
                            }
                            else
                            {
                                FixMessageCall();
                                return (false, "message 4 failure");
                            }

                            buffer = new byte[1] { 1 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_Checkpoint[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h2: made cp");

                            }
                            else
                            {
                                FixMessageCall();
                                Debug("h2: failed to make cp");
                                return (false, "failed to write cp byte");
                            }

                            Thread.Sleep(50);

                            buffer = new byte[5];
                            buffer = HCMGlobal.LoadedOffsets.H2_MessageCode;
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h2: custom message 5: success");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "message 5 failure");
                            }

                            void FixMessageCall()
                            {

                                try
                                {
                                    buffer = new byte[5];
                                    buffer = HCMGlobal.LoadedOffsets.H2_MessageCode;
                                    WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten);
                                }
                                catch
                                {
                                }
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

                case "H3CP":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "H3" && ValidCheck_H3())
                        {

                            //looks like doing custom message in MP breaks things, so we need to skip that stuff if we're in MP
                            byte[] buffer = new byte[64];
                            string holdstring;
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_LevelName[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                
                                holdstring = Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                                holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                                holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                                char[] exceptions = new char[] { '_' };
                                holdstring = String.Concat(holdstring.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                                Debug("read h3 level: " + holdstring);

                            }
                            else {
                                return (false, "failed to read levelname for mp check");
                            }

                            if (!LevelCodeToGameType(holdstring))
                            {

                                //next, the custom message stuff
                                //null the cp messagecall
                                buffer = new byte[5] { 0x90, 0x90, 0x90, 0x90, 0x90 };
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("H3: custom message 0: success");
                                }
                                else
                                {
                                    return (false, "message 0 failure");
                                }


                                //acquire the current tickcount
                                buffer = new byte[4];
                                if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_TickCounter[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("H3: custom message 1: success");
                                }
                                else
                                {
                                    FixMessageCall();
                                    return (false, "message 1 failure");
                                }

                                //buffer will be equal to the tickcounter so just paste it into the new spot
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_Message[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("H3: custom message 2: success");
                                }
                                else
                                {
                                    FixMessageCall();
                                    return (false, "message 2 failure");
                                }

                                buffer = new byte["Custom Checkpoint... done".Length * 2]; //halo uses widechar for it's message strings, so double the length needed.
                                buffer = Encoding.Unicode.GetBytes("Custom Checkpoint... done");
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_Message[Convert.ToInt32(HCMGlobal.WinFlag)]) - 0xC8, buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("H3: custom message 3: success");
                                }
                                else
                                {
                                    FixMessageCall();
                                    return (false, "message 3 failure");
                                }

                                buffer = new byte[1] { 1 }; //setting showmessage flag to true
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_Message[Convert.ToInt32(HCMGlobal.WinFlag)]) - 7, buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("H3: custom message 4: success");
                                }
                                else
                                {
                                    FixMessageCall();
                                    return (false, "message 4 failure");
                                }

                            }

                            buffer = new byte[1] { 1 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_Checkpoint[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("H3: made cp");

                            }
                            else
                            {
                                FixMessageCall();
                                Debug("H3: failed to make cp");
                                return (false, "failed to write cp byte");
                            }

                            if (!LevelCodeToGameType(holdstring))
                            {
                                Thread.Sleep(50);

                                buffer = new byte[5];
                                buffer = HCMGlobal.LoadedOffsets.H3_MessageCode;
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("H3: custom message 5: success");
                                    return (true, "success!");
                                }
                                else
                                {
                                    return (false, "message 5 failure");
                                }
                            }
                            else
                            {
                                return (true, "success! (mp cp)");
                            }


                            void FixMessageCall()
                            {

                                try
                                {
                                    buffer = new byte[5];
                                    buffer = HCMGlobal.LoadedOffsets.H3_MessageCode;
                                    WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten);
                                }
                                catch
                                {
                                }
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


                ///start inserting code for ODST here, then revert, dr, dump, and inject
                ///
                case "ODCP":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "OD" && ValidCheck_OD())
                        {

                            //next, the custom message stuff
                            //null the cp messagecall
                            byte[] buffer = new byte[5] { 0x90, 0x90, 0x90, 0x90, 0x90 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("OD: custom message 0: success");
                            }
                            else
                            {
                                return (false, "message 0 failure");
                            }


                            //acquire the current tickcount
                            buffer = new byte[4];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_TickCounter[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("OD: custom message 1: success");
                            }
                            else
                            {
                                FixMessageCall();
                                return (false, "message 1 failure");
                            }

                            //buffer will be equal to the tickcounter so just paste it into the new spot
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_Message[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("OD: custom message 2: success");
                            }
                            else
                            {
                                FixMessageCall();
                                return (false, "message 2 failure");
                            }

                            buffer = new byte["Custom Checkpoint... done".Length * 2]; //halo uses widechar for it's message strings, so double the length needed.
                            buffer = Encoding.Unicode.GetBytes("Custom Checkpoint... done");
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_Message[Convert.ToInt32(HCMGlobal.WinFlag)]) - 0xC8, buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("OD: custom message 3: success");
                            }
                            else
                            {
                                FixMessageCall();
                                return (false, "message 3 failure");
                            }

                            buffer = new byte[1] { 1 }; //setting showmessage flag to true
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_Message[Convert.ToInt32(HCMGlobal.WinFlag)]) - 7, buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("OD: custom message 4: success");
                            }
                            else
                            {
                                FixMessageCall();
                                return (false, "message 4 failure");
                            }

                            buffer = new byte[1] { 1 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_Checkpoint[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("OD: made cp");

                            }
                            else
                            {
                                FixMessageCall();
                                Debug("OD: failed to make cp");
                                return (false, "failed to write cp byte");
                            }

                            Thread.Sleep(50);

                            buffer = new byte[5];
                            buffer = HCMGlobal.LoadedOffsets.OD_MessageCode;
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("OD: custom message 5: success");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "message 5 failure");
                            }

                            void FixMessageCall()
                            {

                                try
                                {
                                    buffer = new byte[5];
                                    buffer = HCMGlobal.LoadedOffsets.OD_MessageCode;
                                    WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten);
                                }
                                catch
                                {
                                }
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


                case "HRCP":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "HR" && ValidCheck_HR())
                        {

                            //looks like doing custom message in MP breaks things, so we need to skip that stuff if we're in MP
                            byte[] buffer = new byte[64];
                            string holdstring;
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_LevelName[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {

                                holdstring = Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                                holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                                holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                                char[] exceptions = new char[] { '_' };
                                holdstring = String.Concat(holdstring.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                                Debug("read h3 level: " + holdstring);

                            }
                            else
                            {
                                return (false, "failed to read levelname for mp check");
                            }

                            if (!LevelCodeToGameType(holdstring))
                            {



                                //next, the custom message stuff
                                //null the cp messagecall
                                buffer = new byte[5] { 0x90, 0x90, 0x90, 0x90, 0x90 };
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("hr: custom message 0: success");
                                }
                                else
                                {
                                    return (false, "message 0 failure");
                                }


                                //acquire the current tickcount
                                buffer = new byte[4];
                                if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_TickCounter[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("hr: custom message 1: success");
                                }
                                else
                                {
                                    FixMessageCall();
                                    return (false, "message 1 failure");
                                }

                                //buffer will be equal to the tickcounter so just paste it into the new spot
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_Message[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("hr: custom message 2: success");
                                }
                                else
                                {
                                    FixMessageCall();
                                    return (false, "message 2 failure");
                                }

                                buffer = new byte["Custom Checkpoint... done".Length * 2]; //halo uses widechar for it's message strings, so double the length needed.
                                buffer = Encoding.Unicode.GetBytes("Custom Checkpoint... done");
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_Message[Convert.ToInt32(HCMGlobal.WinFlag)]) + 0x84, buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("hr: custom message 3: success");
                                }
                                else
                                {
                                    FixMessageCall();
                                    return (false, "message 3 failure");
                                }

                                buffer = new byte[1] { 1 }; //setting showmessage flag to true
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_Message[Convert.ToInt32(HCMGlobal.WinFlag)]) - 0x10, buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("hr: custom message 4: success");
                                }
                                else
                                {
                                    FixMessageCall();
                                    return (false, "message 4 failure");
                                }

                            }

                            buffer = new byte[1] { 1 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_Checkpoint[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("hr: made cp");

                            }
                            else
                            {
                                FixMessageCall();
                                Debug("hr: failed to make cp");
                                return (false, "failed to write cp byte");
                            }


                            if (!LevelCodeToGameType(holdstring))
                            {
                                Thread.Sleep(50);

                                buffer = new byte[5];
                                buffer = HCMGlobal.LoadedOffsets.HR_MessageCode;
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("hr: custom message 5: success");
                                    return (true, "success!");
                                }
                                else
                                {
                                    return (false, "message 5 failure");
                                }
                            }
                            else
                            {
                                return (true, "success! (mp cp)");
                            }



                            void FixMessageCall()
                            {

                                try
                                {
                                    buffer = new byte[5];
                                    buffer = HCMGlobal.LoadedOffsets.HR_MessageCode;
                                    WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.HR_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten);
                                }
                                catch
                                {
                                }
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


                case "H4CP":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "H4" && ValidCheck_H4())
                        {

                            //looks like doing custom message in MP breaks things, so we need to skip that stuff if we're in MP
                            byte[] buffer = new byte[64];
                            string holdstring;
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_LevelName[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {

                                holdstring = Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                                holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                                holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                                char[] exceptions = new char[] { '_' };
                                holdstring = String.Concat(holdstring.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                                Debug("read H4 level: " + holdstring);

                            }
                            else
                            {
                                return (false, "failed to read levelname for mp check");
                            }

                            if (!LevelCodeToGameType(holdstring))
                            {

                                //next, the custom message stuff
                                //null the cp messagecall
                                buffer = new byte[5] { 0x90, 0x90, 0x90, 0x90, 0x90 };
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("H4: custom message 0: success");
                                }
                                else
                                {
                                    return (false, "message 0 failure");
                                }


                                //acquire the current tickcount
                                buffer = new byte[4];
                                if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_TickCounter[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("H4: custom message 1: success");
                                }
                                else
                                {
                                    FixMessageCall();
                                    return (false, "message 1 failure");
                                }

                                //buffer will be equal to the tickcounter so just paste it into the new spot
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_Message[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("H4: custom message 2: success");
                                }
                                else
                                {
                                    FixMessageCall();
                                    return (false, "message 2 failure");
                                }

                                buffer = new byte["Custom Checkpoint... done  ".Length * 2]; //halo uses widechar for it's message strings, so double the length needed.
                                buffer = Encoding.Unicode.GetBytes("Custom Checkpoint... done  ");
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_Message[Convert.ToInt32(HCMGlobal.WinFlag)]) + 8, buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("H4: custom message 3: success");
                                }
                                else
                                {
                                    FixMessageCall();
                                    return (false, "message 3 failure");
                                }

                                buffer = new byte[8] { 0, 0, 0, 0, 0, 0, 0, 0 }; //setting showmessage flag to true --- show is ZERO, not one. set 8 bytes, not one.
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_Message[Convert.ToInt32(HCMGlobal.WinFlag)]) - 0x10, buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("H4: custom message 4: success");
                                }
                                else
                                {
                                    FixMessageCall();
                                    return (false, "message 4 failure");
                                }

                            }

                            buffer = new byte[1] { 1 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_Checkpoint[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("H4: made cp");

                            }
                            else
                            {
                                FixMessageCall();
                                Debug("H4: failed to make cp");
                                return (false, "failed to write cp byte");
                            }

                            if (!LevelCodeToGameType(holdstring))
                            {
                                Thread.Sleep(50);

                                buffer = new byte[5];
                                buffer = HCMGlobal.LoadedOffsets.H4_MessageCode;
                                if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                                {
                                    Debug("H4: custom message 5: success");
                                    return (true, "success!");
                                }
                                else
                                {
                                    return (false, "message 5 failure");
                                }
                            }
                            else
                            {
                                return (true, "success! (mp cp)");
                            }


                            void FixMessageCall()
                            {

                                try
                                {
                                    buffer = new byte[5];
                                    buffer = HCMGlobal.LoadedOffsets.H4_MessageCode;
                                    WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_MessageCall[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten);
                                }
                                catch
                                {
                                }
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
            var (success, error) = ForceRevertFunction(parent_name);

            if (success == false)
            {
                Debug("Failed to force load, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to force load, " + error, "Error", System.Windows.MessageBoxButton.OK);
            }

        }

        private static (bool success, string error) ForceRevertFunction(string game)
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

                case "H2CP":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "H2" && ValidCheck_H2())
                        {
                            byte[] buffer = new byte[1] { 1 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_Revert[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h2: made revert");
                                return (true, "success!");
                            }
                            else
                            {
                                Debug("h2: failed to make revert");
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

                case "H3CP":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "H3" && ValidCheck_H3())
                        {
                            byte[] buffer = new byte[1] { 1 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_Revert[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("H3: made revert");
                                return (true, "success!");
                            }
                            else
                            {
                                Debug("H3: failed to make revert");
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

                case "ODCP":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "OD" && ValidCheck_OD())
                        {
                            byte[] buffer = new byte[1] { 1 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_Revert[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("OD: made revert");
                                return (true, "success!");
                            }
                            else
                            {
                                Debug("OD: failed to make revert");
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


                case "H4CP":
                    try
                    {
                        if (HCMGlobal.AttachedGame == "H4" && ValidCheck_H4())
                        {
                            byte[] buffer = new byte[1] { 1 };
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_Revert[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("H4: made revert");
                                return (true, "success!");
                            }
                            else
                            {
                                Debug("H4: failed to make revert");
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
            var (success, error) = DoubleRevertFunction(parent_name);

            if (success == false)
            {
                Debug("Failed to double revert, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to double revert, " + error, "Error", System.Windows.MessageBoxButton.OK);
                return;
            }

            RefreshLoa(sender, e);

            (success, error) = ForceRevertFunction(parent_name);

            if (success == false)
            {
                Debug("Failed to revert, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(parent_name + ": Failed to revert, " + error, "Error", System.Windows.MessageBoxButton.OK);
                return;
            }



            
        }

        private static (bool success, string error) DoubleRevertFunction(string game)
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

                case "H2CP":

                    try
                    {
                        if (HCMGlobal.AttachedGame == "H2" && ValidCheck_H2())
                        {
                            byte[] buffer = new byte[1];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h2: read double revert flag");
                            }
                            else
                            {
                                return (false, "failed to read double revert flag");
                            }

                            buffer[0] = Convert.ToByte(!(BitConverter.ToBoolean(buffer, 0))); //just flip the value lmao
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("h2: made double revert");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "h2: failed to make double revert");
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

                case "H3CP":

                    try
                    {
                        if (HCMGlobal.AttachedGame == "H3" && ValidCheck_H3())
                        {
                            byte[] buffer = new byte[1];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("H3: read double revert flag");
                            }
                            else
                            {
                                return (false, "failed to read double revert flag");
                            }

                            buffer[0] = Convert.ToByte(!(BitConverter.ToBoolean(buffer, 0))); //just flip the value lmao
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("H3: made double revert");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "H3: failed to make double revert");
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

                case "ODCP":

                    try
                    {
                        if (HCMGlobal.AttachedGame == "OD" && ValidCheck_OD())
                        {
                            byte[] buffer = new byte[1];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("OD: read double revert flag");
                            }
                            else
                            {
                                return (false, "failed to read double revert flag");
                            }

                            buffer[0] = Convert.ToByte(!(BitConverter.ToBoolean(buffer, 0))); //just flip the value lmao
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("OD: made double revert");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "OD: failed to make double revert");
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


                case "H4CP":

                    try
                    {
                        if (HCMGlobal.AttachedGame == "H4" && ValidCheck_H4())
                        {
                            byte[] buffer = new byte[1];
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("H4: read double revert flag");
                            }
                            else
                            {
                                return (false, "failed to read double revert flag");
                            }

                            buffer[0] = Convert.ToByte(!(BitConverter.ToBoolean(buffer, 0))); //just flip the value lmao
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("H4: made double revert");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "H4: failed to make double revert");
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
                        if (HCMGlobal.CoreFolderPath != null)
                        {
                            pathtotest = HCMGlobal.CoreFolderPath + @"\core.bin";
                        }
                        else
                        {
                                return (false, "core folder path not set, uhhhh that's bad! tell burnt?");
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

                case "H2CP":
                    try
                    {

                        if (HCMGlobal.AttachedGame == "H2" && ValidCheck_H2())
                        {

                            int bytesWritten;
                            bool DRflag;
                            byte[] DRbuffer = new byte[1];

                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                            {
                                DRflag = Convert.ToBoolean(DRbuffer[0]);
                            }
                            else
                            {
                                Debug("something went wrong trying to read DR flag for h2 dumping");
                                return (false, "Failed to read double-revert byte");
                            }

                            byte[] buffer = new byte[HCMGlobal.LoadedOffsets.H2_CPData_Size];
                            int offset;
                            if (!DRflag)
                            {
                                offset = HCMGlobal.LoadedOffsets.H2_CPData_DROffset1; //first cp
                            }
                            else
                            {
                                offset = HCMGlobal.LoadedOffsets.H2_CPData_DROffset2; //second cp
                            }

                            int[] addy = HCMGlobal.LoadedOffsets.H2_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                            addy[3] = offset;
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy), buffer, buffer.Length, out bytesWritten))
                            {

                                string backuploc = HCMGlobal.H2CheckpointPath;

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
                                            Debug("SUCESSFULLY DUMPED H2 CP, LENGTH: " + test.Length.ToString());
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


                case "H3CP":
                    try
                    {

                        if (HCMGlobal.AttachedGame == "H3" && ValidCheck_H3())
                        {

                            int bytesWritten;
                            bool DRflag;
                            byte[] DRbuffer = new byte[1];

                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                            {
                                DRflag = Convert.ToBoolean(DRbuffer[0]);
                            }
                            else
                            {
                                Debug("something went wrong trying to read DR flag for H3 dumping");
                                return (false, "Failed to read double-revert byte");
                            }

                            byte[] buffer = new byte[HCMGlobal.LoadedOffsets.H3_CPData_Size];
                            int offset;
                            if (!DRflag)
                            {
                                offset = HCMGlobal.LoadedOffsets.H3_CPData_DROffset1; //first cp
                            }
                            else
                            {
                                offset = HCMGlobal.LoadedOffsets.H3_CPData_DROffset2; //second cp
                            }

                            int[] addy = HCMGlobal.LoadedOffsets.H3_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                            addy[3] = offset;
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy), buffer, buffer.Length, out bytesWritten))
                            {

                                string backuploc = HCMGlobal.H3CheckpointPath;

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
                                            Debug("SUCESSFULLY DUMPED H3 CP, LENGTH: " + test.Length.ToString());
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



                case "ODCP":
                    try
                    {

                        if (HCMGlobal.AttachedGame == "OD" && ValidCheck_OD())
                        {

                            int bytesWritten;
                            bool DRflag;
                            byte[] DRbuffer = new byte[1];

                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                            {
                                DRflag = Convert.ToBoolean(DRbuffer[0]);
                            }
                            else
                            {
                                Debug("something went wrong trying to read DR flag for OD dumping");
                                return (false, "Failed to read double-revert byte");
                            }

                            byte[] buffer = new byte[HCMGlobal.LoadedOffsets.OD_CPData_Size];
                            int offset;
                            if (!DRflag)
                            {
                                offset = HCMGlobal.LoadedOffsets.OD_CPData_DROffset1; //first cp
                            }
                            else
                            {
                                offset = HCMGlobal.LoadedOffsets.OD_CPData_DROffset2; //second cp
                            }

                            int[] addy = HCMGlobal.LoadedOffsets.OD_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                            addy[3] = offset;
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy), buffer, buffer.Length, out bytesWritten))
                            {

                                string backuploc = HCMGlobal.ODCheckpointPath;

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
                                            Debug("SUCESSFULLY DUMPED OD CP, LENGTH: " + test.Length.ToString());
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

                            byte[] buffer = new byte[HCMGlobal.LoadedOffsets.HR_CPData_Size];
                            int offset;
                            if (!DRflag)
                            {
                                offset = HCMGlobal.LoadedOffsets.HR_CPData_DROffset1; //first cp
                            }
                            else
                            {
                                offset = HCMGlobal.LoadedOffsets.HR_CPData_DROffset2; //second cp
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


                case "H4CP":
                    try
                    {

                        if (HCMGlobal.AttachedGame == "H4" && ValidCheck_H4())
                        {

                            int bytesWritten;
                            bool DRflag;
                            byte[] DRbuffer = new byte[1];

                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                            {
                                DRflag = Convert.ToBoolean(DRbuffer[0]);
                            }
                            else
                            {
                                Debug("something went wrong trying to read DR flag for H4 dumping");
                                return (false, "Failed to read double-revert byte");
                            }

                            byte[] buffer = new byte[HCMGlobal.LoadedOffsets.H4_CPData_Size];
                            int offset;
                            if (!DRflag)
                            {
                                offset = HCMGlobal.LoadedOffsets.H4_CPData_DROffset1; //first cp
                            }
                            else
                            {
                                offset = HCMGlobal.LoadedOffsets.H4_CPData_DROffset2; //second cp
                            }

                            int[] addy = HCMGlobal.LoadedOffsets.H4_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                            addy[3] = offset;
                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy), buffer, buffer.Length, out bytesWritten))
                            {

                                string backuploc = HCMGlobal.H4CheckpointPath;

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
                                            Debug("SUCESSFULLY DUMPED H4 CP, LENGTH: " + test.Length.ToString());
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
                        string targetPath = HCMGlobal.CoreFolderPath + @"\core.bin";
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

                case "H2CP":
                    try
                    {
                        Debug("attempting to inject h2 checkpoint");
                        if (H2CP_MainList.SelectedItem == null)
                        {
                            return (false, "no save file selected!");
                        }

                        if (HCMGlobal.AttachedGame == "H2" && ValidCheck_H2())
                        {
                            var item = H2CP_MainList.Items.GetItemAt(H2CP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                            string sourcePath = HCMGlobal.H2CheckpointPath + @"\" + item.Name + @".bin";
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

                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                            {
                                DRflag = Convert.ToBoolean(DRbuffer[0]);
                            }
                            else
                            {
                                return (false, "something went wrong trying to read DR flag for h2 injecting");
                            }

                            int offset;
                            if (!DRflag)
                            {
                                offset = HCMGlobal.LoadedOffsets.H2_CPData_DROffset1; //first cp
                            }
                            else
                            {
                                offset = HCMGlobal.LoadedOffsets.H2_CPData_DROffset2; //second cp
                            }

                            int[] addy = HCMGlobal.LoadedOffsets.H2_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                            addy[3] = offset;

                            //setup a 2d array with the values we need to preserve (offset, length)
                            int[][] PreserveLocations = HCMGlobal.LoadedOffsets.H2_CPData_PreserveLocations; //first let's checkpoints load at all, second one fixes restart-level bug



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
                                    return (false, "failed reading current vals for h2 injection");
                                }
                            }

                            //now to inject into memory
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("successfully injected h2 cp, " + buffer.Length.ToString() + " bytes!");
                                
                            }
                            else
                            {
                                return (false, "failed injecting h2 cp");
                            }


                            //ok this is some weird shit but we also gotta inject the cp to a third location or else level restart will fail. not sure whats up with that


                            //gotta set the cp loaded bsp or we'll crash da game!
                            int[] addybsp;
                            if (!DRflag)
                            {
                                addybsp = HCMGlobal.LoadedOffsets.H2_LoadedBSP1[Convert.ToInt32(HCMGlobal.WinFlag)]; //first cp
                            }
                            else
                            {
                                addybsp = HCMGlobal.LoadedOffsets.H2_LoadedBSP2[Convert.ToInt32(HCMGlobal.WinFlag)]; //second cp
                            }

                            byte[] BSPbuffer = new byte[HCMGlobal.LoadedOffsets.H2_CPData_BSP[1]];
                            BSPbuffer[0] = buffer[HCMGlobal.LoadedOffsets.H2_CPData_BSP[0]];
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addybsp), BSPbuffer, BSPbuffer.Length, out bytesWritten))
                            {
                                Debug("successfully set loaded bsp, " + BSPbuffer.Length.ToString() + " bytes!");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "failed writing loaded bsp");
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

                case "H3CP":
                    try
                    {
                        Debug("attempting to inject H3 checkpoint");
                        if (H3CP_MainList.SelectedItem == null)
                        {
                            return (false, "no save file selected!");
                        }

                        if (HCMGlobal.AttachedGame == "H3" && ValidCheck_H3())
                        {
                            var item = H3CP_MainList.Items.GetItemAt(H3CP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                            string sourcePath = HCMGlobal.H3CheckpointPath + @"\" + item.Name + @".bin";
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

                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                            {
                                DRflag = Convert.ToBoolean(DRbuffer[0]);
                            }
                            else
                            {
                                return (false, "something went wrong trying to read DR flag for H3 injecting");
                            }

                            int offset;
                            if (!DRflag)
                            {
                                offset = HCMGlobal.LoadedOffsets.H3_CPData_DROffset1; //first cp
                            }
                            else
                            {
                                offset = HCMGlobal.LoadedOffsets.H3_CPData_DROffset2; //second cp
                            }

                            int[] addy = HCMGlobal.LoadedOffsets.H3_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                            addy[3] = offset;

                            //setup a 2d array with the values we need to preserve (offset, length)

                            ///THIS HAS CHANGED AAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
                            int[][] PreserveLocations = HCMGlobal.LoadedOffsets.H3_CPData_PreserveLocations;

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
                                    return (false, "failed reading current vals for H3 injection");
                                }
                            }

                            //now the hash stuff because for some god foresaken reason h3 checkpoints have a hash (thanks scales)

                            //NEXT DO HASH STUFF -- BOY THIS IS BEGINNING TO LOOK UNREADABLE LMAO
                            //first, store the old hash value (not really necessary but helps debugging)
                            byte[] oldhash = new byte[HCMGlobal.LoadedOffsets.H3_CPData_SHA[1]];
                            Array.Copy(buffer, HCMGlobal.LoadedOffsets.H3_CPData_SHA[0], oldhash, 0, HCMGlobal.LoadedOffsets.H3_CPData_SHA[1]);
                            Debug("oldhash: " + BitConverter.ToString(oldhash).Replace("-", ""));


                            //zero out the hash at FB18 (dec 20 bytes)
                            byte[] zeroes = new byte[HCMGlobal.LoadedOffsets.H3_CPData_SHA[1]];
                            Array.Copy(zeroes, 0, buffer, HCMGlobal.LoadedOffsets.H3_CPData_SHA[0], HCMGlobal.LoadedOffsets.H3_CPData_SHA[1]);

                            //then calculate the sha-1 hash
                            try
                            {
                                using (var cryptoProvider = new SHA1CryptoServiceProvider())
                                {
                                    byte[] newhash = cryptoProvider.ComputeHash(buffer);

                                    //write the hash at FB18 
                                    Array.Copy(newhash, 0, buffer, HCMGlobal.LoadedOffsets.H3_CPData_SHA[0], HCMGlobal.LoadedOffsets.H3_CPData_SHA[1]);
                                    Debug("newhash: " + BitConverter.ToString(newhash).Replace("-", ""));
                                }
                            }
                            catch
                            {
                                return (false, "something went wrong calculating cp hash");
                            }

                            //gotta set the cp loaded bsp or we'll crash da game!


                            //bsp manip
                            byte[] buffer6 = new byte[HCMGlobal.LoadedOffsets.H3_CPData_BSP[1]];
                            Array.Copy(buffer, HCMGlobal.LoadedOffsets.H3_CPData_BSP[0], buffer6, 0, HCMGlobal.LoadedOffsets.H3_CPData_BSP[1]);//read bsp from checkpoint file
                            int[][] bspoffsets;
                            if (!DRflag)
                            {
                                bspoffsets = HCMGlobal.LoadedOffsets.H3_LoadedBSP1;
                            }
                            else
                            {
                                bspoffsets = HCMGlobal.LoadedOffsets.H3_LoadedBSP2;
                            }

                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, bspoffsets[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer6, buffer6.Length, out int bytesWritten6)) //write it in so the game knows what bsp to load
                            {
                                Debug("Successfully pasted h3 bsp bytes: " + BitConverter.ToString(buffer6).Replace("-", ""));
                            }
                            else
                            {
                                return (false, "failed pasting h3 bsp bytes");
                            }


                            //now to inject the prepared checkpoint into memory
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("successfully injected H3 cp, " + buffer.Length.ToString() + " bytes!");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "failed injecting H3 cp");
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



                case "ODCP":
                    try
                    {
                        Debug("attempting to inject OD checkpoint");
                        if (ODCP_MainList.SelectedItem == null)
                        {
                            return (false, "no save file selected!");
                        }

                        if (HCMGlobal.AttachedGame == "OD" && ValidCheck_OD())
                        {
                            var item = ODCP_MainList.Items.GetItemAt(ODCP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                            string sourcePath = HCMGlobal.ODCheckpointPath + @"\" + item.Name + @".bin";
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

                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                            {
                                DRflag = Convert.ToBoolean(DRbuffer[0]);
                            }
                            else
                            {
                                return (false, "something went wrong trying to read DR flag for OD injecting");
                            }

                            int offset;
                            if (!DRflag)
                            {
                                offset = HCMGlobal.LoadedOffsets.OD_CPData_DROffset1; //first cp
                            }
                            else
                            {
                                offset = HCMGlobal.LoadedOffsets.OD_CPData_DROffset2; //second cp
                            }

                            int[] addy = HCMGlobal.LoadedOffsets.OD_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                            addy[3] = offset;

                            //setup a 2d array with the values we need to preserve (offset, length)

                            int[][] PreserveLocations = HCMGlobal.LoadedOffsets.OD_CPData_PreserveLocations;
                            //possibly unncessary preserves: { 0x3F458B, 0x2 }, { 0x3F05BC, 0x8 }, { 0x3F06E4, 0x8 }

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
                                    return (false, "failed reading current vals for OD injection");
                                }
                            }

                            //now the hash stuff because for some god foresaken reason OD checkpoints have a hash (thanks scales)

                            //NEXT DO HASH STUFF
                            //first, store the old hash value (not really necessary but helps debugging)
                            byte[] oldhash = new byte[HCMGlobal.LoadedOffsets.OD_CPData_SHA[1]];
                            Array.Copy(buffer, HCMGlobal.LoadedOffsets.OD_CPData_SHA[0], oldhash, 0, HCMGlobal.LoadedOffsets.OD_CPData_SHA[1]);
                            Debug("oldhash: " + BitConverter.ToString(oldhash).Replace("-", ""));


                            //zero out the hash at FB18 (dec 20 bytes)
                            byte[] zeroes = new byte[HCMGlobal.LoadedOffsets.OD_CPData_SHA[1]];
                            Array.Copy(zeroes, 0, buffer, HCMGlobal.LoadedOffsets.OD_CPData_SHA[0], HCMGlobal.LoadedOffsets.OD_CPData_SHA[1]);

                            //then calculate the sha-1 hash
                            try
                            {
                                using (var cryptoProvider = new SHA1CryptoServiceProvider())
                                {
                                    byte[] newhash = cryptoProvider.ComputeHash(buffer);

                                    //write the hash at FB18 
                                    Array.Copy(newhash, 0, buffer, HCMGlobal.LoadedOffsets.OD_CPData_SHA[0], HCMGlobal.LoadedOffsets.OD_CPData_SHA[1]);
                                    Debug("newhash: " + BitConverter.ToString(newhash).Replace("-", ""));
                                }
                            }
                            catch
                            {
                                return (false, "something went wrong calculating cp hash");
                            }

                            //gotta set the cp loaded bsp or we'll crash da game!


                            //bsp manip
                            byte[] buffer6 = new byte[HCMGlobal.LoadedOffsets.OD_CPData_BSP[1]];
                            Array.Copy(buffer, HCMGlobal.LoadedOffsets.OD_CPData_BSP[0], buffer6, 0, HCMGlobal.LoadedOffsets.OD_CPData_BSP[1]);//read bsp from checkpoint file
                            int[][] bspoffsets;
                            if (!DRflag)
                            {
                                bspoffsets = HCMGlobal.LoadedOffsets.OD_LoadedBSP1;
                            }
                            else
                            {
                                bspoffsets = HCMGlobal.LoadedOffsets.OD_LoadedBSP2;
                            }

                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, bspoffsets[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer6, buffer6.Length, out int bytesWritten6)) //write it in so the game knows what bsp to load
                            {
                                Debug("Successfully pasted OD bsp bytes: " + BitConverter.ToString(buffer6).Replace("-", ""));
                            }
                            else
                            {
                                return (false, "failed pasting OD bsp bytes");
                            }


                            //now to inject the prepared checkpoint into memory
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("successfully injected OD cp, " + buffer.Length.ToString() + " bytes!");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "failed injecting OD cp");
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
                                offset = HCMGlobal.LoadedOffsets.HR_CPData_DROffset1; //first cp
                            }
                            else
                            {
                                offset = HCMGlobal.LoadedOffsets.HR_CPData_DROffset2; //second cp
                            }

                            int[] addy = HCMGlobal.LoadedOffsets.HR_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                            addy[3] = offset;

                            //setup a 2d array with the values we need to preserve (offset, length)
                            int[][] PreserveLocations = HCMGlobal.LoadedOffsets.HR_CPData_PreserveLocations; 

                            //int[][] PreserveLocations =  new int[][] { new int[] { 5853828, 4 }, new int[] { 5831936, 16 } };

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

                            //now checksum stuff. for some godforsaken reason, 343 added a checksum check into reach cp's as of patch 2448


                          


                            byte[] oldhash = new byte[HCMGlobal.LoadedOffsets.HR_CPData_SHA[1]];
                            Array.Copy(buffer, HCMGlobal.LoadedOffsets.HR_CPData_SHA[0], oldhash, 0, HCMGlobal.LoadedOffsets.HR_CPData_SHA[1]);
                            Debug("oldhash: " + BitConverter.ToString(oldhash).Replace("-", ""));


                            //zero out the hash at FB18 (dec 20 bytes)
                            byte[] zeroes = new byte[HCMGlobal.LoadedOffsets.HR_CPData_SHA[1]];
                            Array.Copy(zeroes, 0, buffer, HCMGlobal.LoadedOffsets.HR_CPData_SHA[0], HCMGlobal.LoadedOffsets.HR_CPData_SHA[1]);

                            //then calculate the sha-1 hash
                            try
                            {
                                using (var cryptoProvider = new SHA1CryptoServiceProvider())
                                {
                                    byte[] newhash = cryptoProvider.ComputeHash(buffer);

                                    //write the hash at FB18 
                                    Array.Copy(newhash, 0, buffer, HCMGlobal.LoadedOffsets.HR_CPData_SHA[0], HCMGlobal.LoadedOffsets.HR_CPData_SHA[1]);
                                    Debug("newhash: " + BitConverter.ToString(newhash).Replace("-", ""));
                                }
                            }
                            catch
                            {
                                return (false, "something went wrong calculating cp hash");
                            }

                            //end checksum stuff


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


                case "H4CP":
                    try
                    {
                        Debug("attempting to inject H4 checkpoint");
                        if (H4CP_MainList.SelectedItem == null)
                        {
                            return (false, "no save file selected!");
                        }

                        if (HCMGlobal.AttachedGame == "H4" && ValidCheck_H4())
                        {
                            var item = H4CP_MainList.Items.GetItemAt(H4CP_MainList.SelectedIndex) as HaloSaveFileMetadata;
                            string sourcePath = HCMGlobal.H4CheckpointPath + @"\" + item.Name + @".bin";
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

                            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_DRflag[Convert.ToInt32(HCMGlobal.WinFlag)]), DRbuffer, DRbuffer.Length, out bytesWritten))
                            {
                                DRflag = Convert.ToBoolean(DRbuffer[0]);
                            }
                            else
                            {
                                return (false, "something went wrong trying to read DR flag for H4 injecting");
                            }

                            int offset;
                            if (!DRflag)
                            {
                                offset = HCMGlobal.LoadedOffsets.H4_CPData_DROffset1; //first cp
                            }
                            else
                            {
                                offset = HCMGlobal.LoadedOffsets.H4_CPData_DROffset2; //second cp
                            }

                            int[] addy = HCMGlobal.LoadedOffsets.H4_CPLocation[Convert.ToInt32(HCMGlobal.WinFlag)];
                            addy[3] = offset;

                            //setup a 2d array with the values we need to preserve (offset, length)

                            
                            int[][] PreserveLocations = HCMGlobal.LoadedOffsets.H4_CPData_PreserveLocations;

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
                                    return (false, "failed reading current vals for H4 injection");
                                }
                            }


                            //now to inject the prepared checkpoint into memory
                            if (WriteProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, addy), buffer, buffer.Length, out bytesWritten))
                            {
                                Debug("successfully injected H4 cp, " + buffer.Length.ToString() + " bytes!");
                                return (true, "success!");
                            }
                            else
                            {
                                return (false, "failed injecting H4 cp");
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
            (success, error) = ForceRevertFunction(parent_name);

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
            var (success, error) = ForceCPFunction(parent_name);

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
            RefreshList(sender, e);

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
                dtClockTime_Tick(sender, new RoutedEventArgs());
            }
            catch (Exception ex)
            {
                Log("unknown maintick error: " + ex.ToString());
            }

            RefreshLoa(sender, new RoutedEventArgs());
            if (HCMGlobal.SavedConfig?.LockoutLevels == true)
            {
                RefreshSel(sender, new RoutedEventArgs());
            }

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


        public static string Hex2Dec(string m)
        {
            return int.Parse(m, System.Globalization.NumberStyles.HexNumber).ToString();
        }

        private void dtClockTime_Tick(object sender, RoutedEventArgs e)
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
            check gameindicatormagic and menuindicator to check whether we're in game (and which game) vs menu
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
                else if (myProcess.ProcessName == "MCC-Win64-Shipping-WinStore" || myProcess.ProcessName == "MCCWinStore-Win64-Shipping.exe") 
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


                        try
                        {
                            myProcess = Process.GetProcessesByName("MCCWinStore-Win64-Shipping")[0];
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
            }




            //adding core folder check here

            if (HCMGlobal.WinFlag == false && HCMGlobal.CoreFolderPath == null)
            {
                //autodetect core folder path by just checking where the attached process is
                string CoreAutoPath = myProcess.MainModule.FileName;
                Debug("aaaaaa: " + CoreAutoPath);
                if (CoreAutoPath != null && CoreAutoPath.Contains("steamapps")) //latter ought to be true if steam version of MCC
                {
                    CoreAutoPath = Directory.GetParent(CoreAutoPath).FullName;
                    CoreAutoPath = Directory.GetParent(CoreAutoPath).FullName;
                    CoreAutoPath = Directory.GetParent(CoreAutoPath).FullName;
                    CoreAutoPath = Directory.GetParent(CoreAutoPath).FullName;
                    CoreAutoPath = System.IO.Path.Combine(CoreAutoPath, @"core");
                    Debug("bbbbbbb: " + CoreAutoPath);
                    if (Directory.Exists(CoreAutoPath))
                    {
                        Debug("autodetected Core folder: " + CoreAutoPath);
                        HCMGlobal.CoreFolderPath = System.IO.Path.GetFullPath(CoreAutoPath);
                    }
                }
            }


            //also forcing folder refresh here
            //ProfileChanged(sender, null);

            
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
                        ITraceWriter traceWriter = new MemoryTraceWriter();
                        try
                        {


                            
                            var s = json;
                            var res = Regex.Replace(s, @"(?i)\b0x([a-f0-9]+)\b", m => Hex2Dec(m.Groups[1].Value));
                            Debug(res);
                            HCMGlobal.LoadedOffsets = JsonConvert.DeserializeObject<Offsets>(res, new JsonSerializerSettings { TraceWriter = traceWriter, });
                            
                        }
                        catch
                        {
                            Debug("something went horrifically wrong trying to load local json file");
                            Debug(traceWriter.ToString());
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

                    if (HCMGlobal.LoadedOffsets.gameindicatormagic == null)
                    {
                        Debug("something went horribly wrong loading the json");
                    }
                    else
                    {
                        RefreshList(sender, e);
                        Debug("gi: " + HCMGlobal.LoadedOffsets.gameindicatormagic.ToString());
                        Debug("gi00: " + HCMGlobal.LoadedOffsets.gameindicatormagic[0][0].ToString());
                        Debug("gi01: " + HCMGlobal.LoadedOffsets.gameindicatormagic[0][1].ToString());
                        Debug("gi10: " + HCMGlobal.LoadedOffsets.gameindicatormagic[1][0].ToString());
                        Debug("gi11: " + HCMGlobal.LoadedOffsets.gameindicatormagic[1][1].ToString());
                        failedlocalcheck = false;
                        //if (HCMGlobal.SavedConfig.CoreFolderPath == null)
                    }
                }
                //if no offsets found, let's check the github and try to download them!
                if (!HCMGlobal.CheckedForOnlineOffsets) //took out failedlocalcheck check, so we always get freshest offsets
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
                                ITraceWriter traceWriter = new MemoryTraceWriter();
                                try
                                {
                                    
                                    var s = json2;
                                    var res = Regex.Replace(s, @"(?i)\b0x([a-f0-9]+)\b", m => Hex2Dec(m.Groups[1].Value));
                                    Debug(res);
                                    HCMGlobal.LoadedOffsets = JsonConvert.DeserializeObject<Offsets>(res, new JsonSerializerSettings { TraceWriter = traceWriter, });
                                }
                                catch
                                {
                                    Debug("something went horrifically wrong trying to load downloaded local json file");
                                    Debug(traceWriter.ToString());
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

                            if (HCMGlobal.LoadedOffsets.gameindicatormagic == null)
                            {
                                Debug("something went horribly wrong loading the json");
                            }
                            else
                            {
                                RefreshList(sender, e);
                                Debug("gi: " + HCMGlobal.LoadedOffsets.gameindicatormagic.ToString());
                                Debug("gi00: " + HCMGlobal.LoadedOffsets.gameindicatormagic[0][0].ToString());
                                Debug("gi01: " + HCMGlobal.LoadedOffsets.gameindicatormagic[0][1].ToString());
                                Debug("gi10: " + HCMGlobal.LoadedOffsets.gameindicatormagic[1][0].ToString());
                                Debug("gi11: " + HCMGlobal.LoadedOffsets.gameindicatormagic[1][1].ToString());
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
                    if (HCMGlobal.LoadedOffsets.gameindicatormagic[0][0] > 0)
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

                    case "MCC-Win64-Shipping-WinStore.exe":
                        HCMGlobal.BaseAddress = myProcessModule.BaseAddress;
                        break;

                    case "MCCWinStore-Win64-Shipping.exe":
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

            if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.gameindicatormagic[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out int bytesRead2))
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
                        HCMGlobal.AttachedGame = "H4"; 
                        break;
                    case 4:
                        HCMGlobal.AttachedGame = "Mn"; //nfi - h2a mp maybe?
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
            else //means reading the gameindicatormagic failed
            {
                Debug("failed to read game indicator");
                Debug("winflag?: " + HCMGlobal.WinFlag);
                Debug("a " + HCMGlobal.BaseAddress);
                Debug("b " + HCMGlobal.GlobalProcessHandle);
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

                case "H2":

                    if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_LevelName[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesRead))
                    {
                        holdstring = Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                        holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                        holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                        char[] exceptions = new char[] { '_' };
                        holdstring = String.Concat(holdstring.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                        Debug("read h2 level: " + holdstring);
                        HCMGlobal.AttachedLevel = holdstring;
                    }
                    else
                    {
                        Debug("failed to read h2 level");
                        HCMGlobal.AttachedLevel = null;
                    }
                    break;

                case "H3":

                    if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_LevelName[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesRead))
                    {
                        holdstring = Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                        holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                        holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                        char[] exceptions = new char[] { '_' };
                        holdstring = String.Concat(holdstring.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                        Debug("read h3 level: " + holdstring);
                        HCMGlobal.AttachedLevel = holdstring;
                    }
                    else
                    {
                        Debug("failed to read h3 level");
                        HCMGlobal.AttachedLevel = null;
                    }
                    break;

                case "OD":

                    if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_LevelName[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesRead))
                    {
                        //levelstring here is laid out a little differently

                        holdstring = Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                        //Debug("Holdstring 1: " + holdstring);
                        holdstring = holdstring.Substring(0, holdstring.LastIndexOf("."));
                        //Debug("Holdstring 2: " + holdstring);
                        //holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                       // Debug("Holdstring 3: " + holdstring);
                        char[] exceptions = new char[] { '_' };
                        holdstring = String.Concat(holdstring.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                        Debug("read OD level: " + holdstring);
                        HCMGlobal.AttachedLevel = holdstring;
                    }
                    else
                    {
                        Debug("failed to read OD level");
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


                case "H4":

                    if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_LevelName[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out bytesRead))
                    {
                        holdstring = Encoding.UTF8.GetString(buffer, 0, buffer.Length);
                        holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                        holdstring = holdstring.Substring(holdstring.LastIndexOf("\\") + 1);
                        char[] exceptions = new char[] { '_' };
                        holdstring = String.Concat(holdstring.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                        Debug("read H4 level: " + holdstring);
                        HCMGlobal.AttachedLevel = holdstring;
                    }
                    else
                    {
                        Debug("failed to read H4 level");
                        HCMGlobal.AttachedLevel = null;
                    }
                    break;

            }


            //if game is reach & tab is reach, then do a seed check
            if (TabList.SelectedIndex == 5 && HCMGlobal.AttachedGame == "HR" && ValidCheck_HR())
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
               

                default:
                case "Mn":
                case "No":
                    SetH1(false);
                    SetHR(false);
                    SetH2(false);
                    SetH3(false);
                    SetOD(false);
                    SetH4(false);
                    break;

                case "H1":
                    SetH1(true); 
                    SetHR(false);
                    SetH2(false);
                    SetH3(false);
                    SetOD(false);
                    SetH4(false);
                    break;

                case "H2":
                    SetH1(false);
                    SetHR(false);
                    SetH2(true);
                    SetH3(false);
                    SetOD(false);
                    SetH4(false);
                    break;

                case "HR":
                    SetH1(false);
                    SetHR(true);
                    SetH2(false);
                    SetH3(false);
                    SetOD(false);
                    SetH4(false);
                    break;

                case "H3":
                    SetH1(false);
                    SetHR(false);
                    SetH2(false);
                    SetH3(true);
                    SetOD(false);
                    SetH4(false);
                    break;

                case "OD":
                    SetH1(false);
                    SetHR(false);
                    SetH2(false);
                    SetH3(false);
                    SetOD(true);
                    SetH4(false);
                    break;

                case "H4":
                    SetH1(false);
                    SetHR(false);
                    SetH2(false);
                    SetH3(false);
                    SetOD(false);
                    SetH4(true);
                    break;



            }

           
  

            void SetH1(bool state)
            {
                H1CS_ForceCheckpoint.IsEnabled = state;
                H1CS_ForceRevert.IsEnabled = state;

                H1CS_Loa_ForceCPDump.IsEnabled = state;
                H1CS_Sel_InjectRevertButton.IsEnabled = state;
                H1CS_Loa_DumpButton.IsEnabled = state;
               H1CS_Sel_InjectButton.IsEnabled = state;

                if (HCMGlobal.CoreFolderPath == null)
                {
                    H1CS_Loa_ForceCPDump.IsEnabled = false; //something with our auto core folder detection went wrong
                    H1CS_Sel_InjectRevertButton.IsEnabled = false;
                    //H1CS_Loa_DumpButton.IsEnabled = false;
                    //H1CS_Sel_InjectButton.IsEnabled = false;
                }

                    H1CS_Loa_DumpButton.IsEnabled = true; //these will work even if we're not attached with correct offsets
                    H1CS_Sel_InjectButton.IsEnabled = true;



               

            }

            void SetHR(bool state)
            {
                HRCP_ForceCheckpoint.IsEnabled = state;
                HRCP_ForceRevert.IsEnabled = state;
                HRCP_ForceDR.IsEnabled = state;
                HRCP_Loa_DumpButton.IsEnabled = state;
                HRCP_Loa_ForceCPDump.IsEnabled = state;

                if (HCMGlobal.SavedConfig != null && HCMGlobal.SavedConfig.LockoutLevels == false)
                {
                    HRCP_Sel_InjectButton.IsEnabled = state;
                    HRCP_Sel_InjectRevertButton.IsEnabled = state;
                }


            }


            void SetH2(bool state)
            {
                H2CP_ForceCheckpoint.IsEnabled = state;
                H2CP_ForceRevert.IsEnabled = state;
                H2CP_ForceDR.IsEnabled = state;
                H2CP_Loa_DumpButton.IsEnabled = state;
                H2CP_Loa_ForceCPDump.IsEnabled = state;
                if (HCMGlobal.SavedConfig != null && HCMGlobal.SavedConfig.LockoutLevels == false)
                {
                    H2CP_Sel_InjectButton.IsEnabled = state;
                    H2CP_Sel_InjectRevertButton.IsEnabled = state;
                }
            }

            void SetH3(bool state)
            {
                H3CP_ForceCheckpoint.IsEnabled = state;
                H3CP_ForceRevert.IsEnabled = state;
                H3CP_ForceDR.IsEnabled = state;
                H3CP_Loa_DumpButton.IsEnabled = state;
                H3CP_Loa_ForceCPDump.IsEnabled = state;
                if (HCMGlobal.SavedConfig != null && HCMGlobal.SavedConfig.LockoutLevels == false)
                {
                    H3CP_Sel_InjectButton.IsEnabled = state;
                    H3CP_Sel_InjectRevertButton.IsEnabled = state;
                }
            }

            void SetOD(bool state)
            {
                ODCP_ForceCheckpoint.IsEnabled = state;
                ODCP_ForceRevert.IsEnabled = state;
                ODCP_ForceDR.IsEnabled = state;
                ODCP_Loa_DumpButton.IsEnabled = state;
                ODCP_Loa_ForceCPDump.IsEnabled = state;
                if (HCMGlobal.SavedConfig != null && HCMGlobal.SavedConfig.LockoutLevels == false)
                {
                    ODCP_Sel_InjectButton.IsEnabled = state;
                    ODCP_Sel_InjectRevertButton.IsEnabled = state;
                }
            }


            void SetH4(bool state)
            {
                H4CP_ForceCheckpoint.IsEnabled = state;
                H4CP_ForceRevert.IsEnabled = state;
                H4CP_ForceDR.IsEnabled = state;
                H4CP_Loa_DumpButton.IsEnabled = state;
                H4CP_Loa_ForceCPDump.IsEnabled = state;
                if (HCMGlobal.SavedConfig != null && HCMGlobal.SavedConfig.LockoutLevels == false)
                {
                    H4CP_Sel_InjectButton.IsEnabled = state;
                    H4CP_Sel_InjectRevertButton.IsEnabled = state;
                }
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

        private static bool ValidCheck_H2()
        {

            try
            {
                byte[] buffer = new byte[4];
                if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H2_CheckString[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out int bytesRead2))
                {
                    if (Encoding.UTF8.GetString(buffer, 0, buffer.Length) == "scen")
                    {
                        Debug("h2 check success");
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

        private static bool ValidCheck_H3()
        {

            try
            {
                byte[] buffer = new byte[4];
                if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H3_CheckString[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out int bytesRead2))
                {
                    if (Encoding.UTF8.GetString(buffer, 0, buffer.Length) == "leve")
                    {
                        Debug("h3 check success");
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

        private static bool ValidCheck_OD()
        {

            try
            {
                byte[] buffer = new byte[4];
                if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.OD_CheckString[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out int bytesRead2))
                {
                    if (Encoding.UTF8.GetString(buffer, 0, buffer.Length) == "maps")
                    {
                        Debug("OD check success");
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


        private static bool ValidCheck_H4()
        {

            try
            {
                byte[] buffer = new byte[4];
                if (ReadProcessMemory(HCMGlobal.GlobalProcessHandle, FindPointerAddy(HCMGlobal.GlobalProcessHandle, HCMGlobal.BaseAddress, HCMGlobal.LoadedOffsets.H4_CheckString[Convert.ToInt32(HCMGlobal.WinFlag)]), buffer, buffer.Length, out int bytesRead2))
                {
                    if (Encoding.UTF8.GetString(buffer, 0, buffer.Length) == "maps")
                    {
                        Debug("H4 check success");
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

        public static void ForceCP(object sender, KeyPressedEventArgs e)
        {
            string game = (HCMGlobal.AttachedGame == "H1") ? game = "H1CS" : game = HCMGlobal.AttachedGame + "CP";

            //do reverty things
            var (success, error) = ForceCPFunction(game);

            if (success == false)
            {
                Debug("Failed to force load, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(game + ": Failed to force save, " + error, "Error", System.Windows.MessageBoxButton.OK);
            }
        }
        public static void ForceRevert(object sender, KeyPressedEventArgs e)
        {
            string game = (HCMGlobal.AttachedGame == "H1") ? game = "H1CS" : game = HCMGlobal.AttachedGame + "CP";

            //do reverty things
            var (success, error) = ForceRevertFunction(game);

            if (success == false)
            {
                Debug("Failed to force load, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(game + ": Failed to force load, " + error, "Error", System.Windows.MessageBoxButton.OK);
            }
        }
        public static void ForceDoubleRevert(object sender, KeyPressedEventArgs e)
        {
            if (HCMGlobal.AttachedGame == "H1")
            {
                ForceRevert(sender, e);
                return;
            }

            string game = HCMGlobal.AttachedGame + "CP";

            //do reverty things
            var (success, error) = DoubleRevertFunction(game);

            if (success == false)
            {
                Debug("Failed to double revert, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(game + ": Failed to double revert, " + error, "Error", System.Windows.MessageBoxButton.OK);
            }

            (success, error) = ForceRevertFunction(game);

            if (success == false)
            {
                Debug("Failed to revert, " + error);
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show(game + ": Failed to revert, " + error, "Error", System.Windows.MessageBoxButton.OK);
                return;
            }
        }
    }
}
