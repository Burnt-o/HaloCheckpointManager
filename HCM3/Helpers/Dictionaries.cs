using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
namespace HCM3.Helpers
{
    internal static class Dictionaries
    {

        public enum HaloStateEnum
        {
            Unattached = -2,
            Menu = -1,
            Halo1 = 0,
            Halo2 = 1,
            Halo2MP = 1,
            Halo3 = 2,
            Halo3ODST = 3,
            HaloReach = 4,
            Halo4 = 5,

        }

        public static readonly Dictionary<int, string> GameToRootFolderPath = new()
        {
            { 0, @"Halo 1" },
            { 1, @"Halo 2" },
            { 2, @"Halo 3" },
            { 3, @"Halo 3 ODST" },
            { 4, @"Halo Reach" },
            { 5, @"Halo 4" },
        };

        public static readonly Dictionary<int, string> GameTo2LetterGameCode = new()
        {
            { 0, @"H1" },
            { 1, @"H2" },
            { 2, @"H3" },
            { 3, @"OD" },
            { 4, @"HR" },
            { 5, @"H4" },
            { -2, @"UN"},
            { -1, @"MN"},
        };

        public static readonly Dictionary<int, string> GameToDLLname = new()
        {
            { 0, @"halo1.dll" },
            { 1, @"halo2.dll" },
            { 2, @"halo3.dll" },
            { 3, @"halo3odst.dll" },
            { 4, @"haloreach.dll" },
            { 5, @"halo4.dll" },
        };




        public static readonly Dictionary<string, string> LevelCodeToNameH1 = new Dictionary<string, string>()
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

        public static readonly Dictionary<string, string> LevelCodeToNameH2 = new Dictionary<string, string>()
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

        public static readonly Dictionary<string, string> LevelCodeToNameH3 = new Dictionary<string, string>()
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

        public static readonly Dictionary<string, string> LevelCodeToNameOD = new Dictionary<string, string>()
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

        public static readonly Dictionary<string, string> LevelCodeToNameHR = new Dictionary<string, string>()
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


        public static readonly Dictionary<string, string> LevelCodeToNameH4 = new Dictionary<string, string>()
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

        public static Dictionary<string, string>[] GameToLevelCodeDictionary =
        {
        LevelCodeToNameH1,
        LevelCodeToNameH2,
        LevelCodeToNameH3,
        LevelCodeToNameOD,
        LevelCodeToNameHR,
        LevelCodeToNameH4,
        };


        public static readonly Dictionary<string, int> KeyStringToKeyCode = new()
        { { "A", 97 }, { "B", 98 }, { "C", 99 }, { "D", 100 }, { "E", 101 }, { "F", 102 }, { "G", 103 }, { "H", 104 }, { "I", 105 }, { "J", 106 }, { "K", 107 }, { "L", 108 }, { "M", 109 }, { "N", 110 }, { "O", 111 }, { "P", 112 }, { "Q", 113 }, { "R", 114 }, { "S", 115 }, { "T", 116 }, { "U", 117 }, { "V", 118 }, { "W", 119 }, { "X", 120 }, { "Y", 121 }, { "Z", 122 }, { "LEFTARROW", 143 }, { "DOWNARROW", 144 }, { "RIGHTARROW", 145 }, { "UPARROW", 146 }, { "F1", 162 }, { "F2", 163 }, { "F3", 164 }, { "F4", 165 }, { "F5", 166 }, { "F6", 167 }, { "F7", 168 }, { "F8", 169 }, { "F9", 170 }, { "F10", 171 }, { "F11", 182 }, { "F12", 183 }, { "ZERO", 48 }, { "ONE", 49 }, { "TWO", 50 }, { "THREE", 51 }, { "FOUR", 52 }, { "FIVE", 53 }, { "SIX", 54 }, { "SEVEN", 55 }, { "EIGHT", 56 }, { "NINE", 57 }, { "PAD0", 158 }, { "PAD1", 151 }, { "PAD2", 147 }, { "PAD3", 152 }, { "PAD4", 148 }, { "PAD5", 153 }, { "PAD6", 149 }, { "PAD7", 154 }, { "PAD8", 150 }, { "PAD9", 155 }, { "PADPERIOD", 156 }, { "PADSLASH", 157 }, { "PADASTER", 42 }, { "PADMI", 159 }, { "PADPLUS", 161 }, { "PADEN", 160 }, { "ACCENTGRAVE", 137 }, { "BACKSLASH", 139 }, { "BACKSPACE", 133 }, { "COMMA", 44 }, { "DEL", 134 }, { "END", 186 }, { "EQUAL", 140 }, { "ESC", 130 }, { "HOME", 176 }, { "INSERT", 175 }, { "LEFTBRACKET", 141 }, { "LINEFEED", 132 }, { "MINUS", 45 }, { "PAGEDOWN", 178 }, { "PAGEUP", 177 }, { "PAUSE", 174 }, { "PERIOD", 46 }, { "QUOTE", 136 }, { "RIGHTBRACKET", 142 }, { "RET", 13 }, { "SEMICOLON", 135 }, { "SLASH", 138 }, { "TAB", 131 }, { "LEFTCTRL", 124 }, { "LEFTALT", 125 }, { "RIGHTALT", 126 }, { "RIGHTCTRL", 127 }, { "RIGHTSHIFT", 128 }, { "LEFTSHIFT", 129 }, { "SPACE", 32 } };
        
        public static readonly Dictionary<int, string> KeyCodeToKeyString = KeyStringToKeyCode.ToDictionary((x) => x.Value, (x) => x.Key);

        /// <summary>
    /// Enumeration for virtual keys.
    /// </summary>
    public enum VirtualKeys
        : ushort
    {
        LeftButton = 0x01,
        RightButton = 0x02,
        Cancel = 0x03,
        MiddleButton = 0x04,
        ExtraButton1 = 0x05,
        ExtraButton2 = 0x06,
        Back = 0x08,
        Tab = 0x09,
        Clear = 0x0C,
        Return = 0x0D,
        Shift = 0x10,
        Control = 0x11,
        Menu = 0x12,
        Pause = 0x13,
        CapsLock = 0x14,
        Kana = 0x15,
        Hangeul = 0x15,
        Hangul = 0x15,
        Junja = 0x17,
        Final = 0x18,
        Hanja = 0x19,
        Kanji = 0x19,
        Escape = 0x1B,
        Convert = 0x1C,
        NonConvert = 0x1D,
        Accept = 0x1E,
        ModeChange = 0x1F,
        Space = 0x20,
        Prior = 0x21,
        Next = 0x22,
        End = 0x23,
        Home = 0x24,
        Left = 0x25,
        Up = 0x26,
        Right = 0x27,
        Down = 0x28,
        Select = 0x29,
        Print = 0x2A,
        Execute = 0x2B,
        Snapshot = 0x2C,
        Insert = 0x2D,
        Delete = 0x2E,
        Help = 0x2F,
        N0 = 0x30,
        N1 = 0x31,
        N2 = 0x32,
        N3 = 0x33,
        N4 = 0x34,
        N5 = 0x35,
        N6 = 0x36,
        N7 = 0x37,
        N8 = 0x38,
        N9 = 0x39,
        A = 0x41,
        B = 0x42,
        C = 0x43,
        D = 0x44,
        E = 0x45,
        F = 0x46,
        G = 0x47,
        H = 0x48,
        I = 0x49,
        J = 0x4A,
        K = 0x4B,
        L = 0x4C,
        M = 0x4D,
        N = 0x4E,
        O = 0x4F,
        P = 0x50,
        Q = 0x51,
        R = 0x52,
        S = 0x53,
        T = 0x54,
        U = 0x55,
        V = 0x56,
        W = 0x57,
        X = 0x58,
        Y = 0x59,
        Z = 0x5A,
        LeftWindows = 0x5B,
        RightWindows = 0x5C,
        Application = 0x5D,
        Sleep = 0x5F,
        Numpad0 = 0x60,
        Numpad1 = 0x61,
        Numpad2 = 0x62,
        Numpad3 = 0x63,
        Numpad4 = 0x64,
        Numpad5 = 0x65,
        Numpad6 = 0x66,
        Numpad7 = 0x67,
        Numpad8 = 0x68,
        Numpad9 = 0x69,
        Multiply = 0x6A,
        Add = 0x6B,
        Separator = 0x6C,
        Subtract = 0x6D,
        Decimal = 0x6E,
        Divide = 0x6F,
        F1 = 0x70,
        F2 = 0x71,
        F3 = 0x72,
        F4 = 0x73,
        F5 = 0x74,
        F6 = 0x75,
        F7 = 0x76,
        F8 = 0x77,
        F9 = 0x78,
        F10 = 0x79,
        F11 = 0x7A,
        F12 = 0x7B,
        F13 = 0x7C,
        F14 = 0x7D,
        F15 = 0x7E,
        F16 = 0x7F,
        F17 = 0x80,
        F18 = 0x81,
        F19 = 0x82,
        F20 = 0x83,
        F21 = 0x84,
        F22 = 0x85,
        F23 = 0x86,
        F24 = 0x87,
        NumLock = 0x90,
        ScrollLock = 0x91,
        NEC_Equal = 0x92,
        Fujitsu_Jisho = 0x92,
        Fujitsu_Masshou = 0x93,
        Fujitsu_Touroku = 0x94,
        Fujitsu_Loya = 0x95,
        Fujitsu_Roya = 0x96,
        LeftShift = 0xA0,
        RightShift = 0xA1,
        LeftControl = 0xA2,
        RightControl = 0xA3,
        LeftMenu = 0xA4,
        RightMenu = 0xA5,
        BrowserBack = 0xA6,
        BrowserForward = 0xA7,
        BrowserRefresh = 0xA8,
        BrowserStop = 0xA9,
        BrowserSearch = 0xAA,
        BrowserFavorites = 0xAB,
        BrowserHome = 0xAC,
        VolumeMute = 0xAD,
        VolumeDown = 0xAE,
        VolumeUp = 0xAF,
        MediaNextTrack = 0xB0,
        MediaPrevTrack = 0xB1,
        MediaStop = 0xB2,
        MediaPlayPause = 0xB3,
        LaunchMail = 0xB4,
        LaunchMediaSelect = 0xB5,
        LaunchApplication1 = 0xB6,
        LaunchApplication2 = 0xB7,
        OEM1 = 0xBA,
        OEMPlus = 0xBB,
        OEMComma = 0xBC,
        OEMMinus = 0xBD,
        OEMPeriod = 0xBE,
        OEM2 = 0xBF,
        OEM3 = 0xC0,
        OEM4 = 0xDB,
        OEM5 = 0xDC,
        OEM6 = 0xDD,
        OEM7 = 0xDE,
        OEM8 = 0xDF,
        OEMAX = 0xE1,
        OEM102 = 0xE2,
        ICOHelp = 0xE3,
        ICO00 = 0xE4,
        ProcessKey = 0xE5,
        ICOClear = 0xE6,
        Packet = 0xE7,
        OEMReset = 0xE9,
        OEMJump = 0xEA,
        OEMPA1 = 0xEB,
        OEMPA2 = 0xEC,
        OEMPA3 = 0xED,
        OEMWSCtrl = 0xEE,
        OEMCUSel = 0xEF,
        OEMATTN = 0xF0,
        OEMFinish = 0xF1,
        OEMCopy = 0xF2,
        OEMAuto = 0xF3,
        OEMENLW = 0xF4,
        OEMBackTab = 0xF5,
        ATTN = 0xF6,
        CRSel = 0xF7,
        EXSel = 0xF8,
        EREOF = 0xF9,
        Play = 0xFA,
        Zoom = 0xFB,
        Noname = 0xFC,
        PA1 = 0xFD,
        OEMClear = 0xFE
    }
        }
}
