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

        public struct LevelInfo
        {
            public string FullName;
            public int LevelPosition;

            public LevelInfo(string fullName, int levelPosition)
            { 
            this.FullName = fullName;
                this.LevelPosition = levelPosition;
            }
        }


        public static readonly Dictionary<string, LevelInfo> LevelCodeToNameH1 = new Dictionary<string, LevelInfo>()
        { 
         // Halo 1
            //SP
            { "a10", new LevelInfo("Pillar of Autumn", 0) },
            { "a30", new LevelInfo("Halo", 1) },
            { "a50", new LevelInfo("Truth and Rec", 2) },
            { "b30", new LevelInfo("Silent Cartographer", 3) },
            { "b40", new LevelInfo("AotCR", 4) },
            { "c10", new LevelInfo("343 Guilty Spark", 5) },
            { "c20", new LevelInfo("The Library", 6) },
            { "c40", new LevelInfo("Two Betrayals", 7) },
            { "d20", new LevelInfo("Keyes", 8) },
            { "d40", new LevelInfo("The Maw", 9) },
            //MP
            { "beavercreek", new LevelInfo("Battle Creek", -1) },
            { "boardingaction", new LevelInfo("Boarding Action", -1) },
            { "bloodgulch", new LevelInfo("Blood Gulch", -1) },
            { "carousel", new LevelInfo("Derelict", -1) },
            { "chillout", new LevelInfo("Chill Out", -1) },
            { "damnation", new LevelInfo("Damnation", -1) },
            { "dangercanyon", new LevelInfo("Danger Canyon", -1) },
            { "deathisland", new LevelInfo("Death Island", -1) },
            { "gephyrophobia", new LevelInfo("Gephyrophobia", -1) },
            { "hangemhigh", new LevelInfo("Hang 'Em High", -1) },
            { "icefields", new LevelInfo("Ice Fields", -1) },
            { "infinity", new LevelInfo("Infinity", -1) },
            { "longest", new LevelInfo("Longest", -1) },
            { "prisoner", new LevelInfo("Prisoner", -1) },
            { "putput", new LevelInfo("Chiron TL-34", -1) },
            { "ratrace", new LevelInfo("Rat Race", -1) },
            { "sidewinder", new LevelInfo("Sidewinder", -1) },
            { "timberland", new LevelInfo("Timberland", -1) },
            { "wizard", new LevelInfo("Wizard", -1) },

        };

        public static readonly Dictionary<string, LevelInfo> LevelCodeToNameH2 = new Dictionary<string, LevelInfo>()
        { 

            // Halo 2
            //SP
            { "00a_introduction", new LevelInfo("The Heretic", 0) },
            { "01a_tutorial", new LevelInfo("The Armory" , 1)},
            { "01b_spacestation", new LevelInfo("Cairo Station" , 2)},
            { "03a_oldmombasa", new LevelInfo("Outskirts" , 3)},
            { "03b_newmombasa", new LevelInfo("Metropolis" , 4)},
            { "04a_gasgiant", new LevelInfo("The Arbiter" , 5)},
            { "04b_floodlab", new LevelInfo("The Oracle" , 6)},
            { "05a_deltaapproach", new LevelInfo("Delta Halo" , 7)},
            { "05b_deltatowers", new LevelInfo("Regret" , 8)},
            { "06a_sentinelwalls", new LevelInfo("Sacred Icon" , 9)},
            { "06b_floodzone", new LevelInfo("Quarantine Zone" ,10)},
            { "07a_highcharity", new LevelInfo("Gravemind" , 11)},
            { "07b_forerunnership", new LevelInfo("High Charity" , 12)},
            { "08a_deltacliffs", new LevelInfo("Uprising" , 13)},
            { "08b_deltacontrol", new LevelInfo("The Great Journey" , 14)},
            //MP
            { "ascension", new LevelInfo("Ascension" , -1)},
            { "backwash", new LevelInfo( "Backwash" , -1)},
            { "beavercreek", new LevelInfo( "placeholder" , -1)},
            { "burial_mounds", new LevelInfo( "Burial Mounds" , -1)},
            { "coagulation", new LevelInfo( "Coagulation" , -1)},
            { "colossus", new LevelInfo( "Colossus" , -1)},
            { "containment", new LevelInfo( "Containment" , -1)},
            { "cyclotron", new LevelInfo( "Ivory Tower" , -1)},
            { "deltatap", new LevelInfo( "Sanctuary" , -1)},
            { "derelict", new LevelInfo( "Desolation" , -1)},
            { "dune", new LevelInfo( "Relic" , -1)},
            { "elongation", new LevelInfo( "Elongation" , -1)},
            { "foundation", new LevelInfo( "Foundation" , -1)},
            { "gemini", new LevelInfo( "Gemini" , -1)},
            { "headlong", new LevelInfo( "Headlong" , -1)},
            { "highplains", new LevelInfo( "Tombstone" , -1)},
            { "lockout", new LevelInfo( "Lockout" , -1)},
            { "midship", new LevelInfo( "Midship" , -1)},
            { "needle", new LevelInfo( "Uplift" , -1)},
            { "street_sweeper", new LevelInfo( "District" , -1)},
            { "triplicate", new LevelInfo( "Terminal" , -1)},
            { "turf", new LevelInfo( "Turf" , -1)},
            { "warlock", new LevelInfo( "Warlock" , -1)},
            { "waterworks", new LevelInfo( "Waterworks" , -1)},
            { "zanzibar", new LevelInfo( "Zanzibar" , -1)},

        };

        public static readonly Dictionary<string, LevelInfo> LevelCodeToNameH3 = new Dictionary<string, LevelInfo>()
        { 


             // Halo 3
            //SP
            { "005_intro", new LevelInfo( "Arrival" , 0)},
            { "010_jungle", new LevelInfo( "Sierra 117" , 1)},
            { "020_base", new LevelInfo( "Crow's Nest" , 2)},
            { "030_outskirts", new LevelInfo( "Tsavo Highway" , 3)},
            { "040_voi", new LevelInfo( "The Storm" , 4)},
            { "050_floodvoi", new LevelInfo( "Floodgate" , 5)},
            { "070_waste", new LevelInfo( "The Ark" , 6)},
            { "100_citadel", new LevelInfo( "The Covenant" , 7)},
            { "110_hc", new LevelInfo( "Cortana" , 8)},
            { "120_halo", new LevelInfo( "Halo" , 9)},
            { "130_epilogue", new LevelInfo( "Epilogue" , 10)},
            //MP
            { "armory", new LevelInfo( "Rat's Nest" , -1)},
            { "bunkerworld", new LevelInfo( "Standoff" , -1)},
            { "chill", new LevelInfo( "Narrows" , -1)},
            { "chillout", new LevelInfo( "Cold Storage" , -1)},
            { "construct", new LevelInfo( "Construct" , -1)},
            { "cyberdyne", new LevelInfo( "The Pit" , -1)},
            { "deadlock", new LevelInfo( "Highground" , -1)},
            { "descent", new LevelInfo( "Assembly" , -1)},
            { "docks", new LevelInfo( "Longshore" , -1)},
            { "fortress", new LevelInfo( "Citadel" , -1)},
            { "ghosttown", new LevelInfo( "Ghost Town" , -1)},
            { "guardian", new LevelInfo( "Guardian" , -1)},
            { "isolation", new LevelInfo( "Isolation" , -1)},
            { "lockout", new LevelInfo( "Blackout" , -1)},
            { "midship", new LevelInfo( "Heretic" , -1)},
            { "riverworld", new LevelInfo( "Valhalla" , -1)},
            { "salvation", new LevelInfo( "Epitaph" , -1)},
            { "sandbox", new LevelInfo( "Sandbox" , -1)},
            { "shrine", new LevelInfo( "Sandtrap" , -1)},
            { "sidewinder", new LevelInfo( "Avalanche" , -1)},
            { "snowbound", new LevelInfo( "Snowbound" , -1)},
            { "spacecamp", new LevelInfo( "Orbital" , -1)},
            { "warehouse", new LevelInfo( "Foundry" , -1)},
            { "zanzibar", new LevelInfo( "Last Resort" , -1)},

        };

        public static readonly Dictionary<string, LevelInfo> LevelCodeToNameOD = new Dictionary<string, LevelInfo>()
        { 


             // ODST
            //SP -- need to double check these. ODST is weird. also double check cases
            { "c100", new LevelInfo( "Prepare to Drop" , 0)}, //aka the cutscene, not the MS1 level
            { "c200", new LevelInfo( "Epilogue" , 10)},
            { "h100", new LevelInfo( "Mombasa Streets" , 1)},
            { "l200", new LevelInfo( "Data Hive" , 8)},
            { "l300", new LevelInfo( "Coastal Highway" , 9)},
            { "sc100", new LevelInfo( "Tayari Plaza" , 2)},
            { "sc110", new LevelInfo( "Uplift Reserve" , 3)},
            { "sc120", new LevelInfo( "Kinzingo Boulevard" , 4)},
            { "sc130", new LevelInfo( "ONI Alpha Site" , 5)},
            { "sc140", new LevelInfo( "NMPD HQ" , 6)},
            { "sc150", new LevelInfo( "Kikowani Station" , 7)},
            //MP
            //imagine

        };

        public static readonly Dictionary<string, LevelInfo> LevelCodeToNameHR = new Dictionary<string, LevelInfo>()
        {

            // Halo Reach
            //SP
            { "m05", new LevelInfo( "Noble Actual" , 0)},
            { "m10", new LevelInfo( "Winter Contingency" , 1)},
            { "m20", new LevelInfo( "ONI: Sword Base" , 2)},
            { "m30", new LevelInfo( "Nightfall" , 3)},
            { "m35", new LevelInfo( "Tip of the Spear" , 4)},
            { "m45", new LevelInfo( "Long Night of Solace" , 5)},
            { "m50", new LevelInfo( "Exodus" , 6)},
            { "m52", new LevelInfo( "New Alexandria" , 7)},
            { "m60", new LevelInfo( "The Package" , 8)},
            { "m70", new LevelInfo( "The Pillar of Autumn" , 9)},
            { "m70_a", new LevelInfo( "Credits" , 10)},
            { "m70_bonus", new LevelInfo( "Lone Wolf" , 11)},
            //MP
            { "20_sword_slayer", new LevelInfo( "Sword Base" , -1)},
            { "30_settlement", new LevelInfo( "Powerhouse" , -1)},
            { "35_island", new LevelInfo( "Spire" , -1)},
            { "45_aftship", new LevelInfo( "Zealot" , -1)},
            { "45_launch_station", new LevelInfo( "Countdown" , -1)},
            { "50_panopticon", new LevelInfo( "Boardwalk" , -1)},
            { "52_ivory_tower", new LevelInfo( "Reflection" , -1)},
            { "70_boneyard", new LevelInfo( "Boneyard" , -1)},
            { "forge_halo", new LevelInfo( "Forge World" , -1)},
            { "dlc_invasion ", new LevelInfo( "Breakpoint" , -1)},
            { "dlc_medium ", new LevelInfo( "Tempest" , -1)},
            { "dlc_slayer ", new LevelInfo( "Anchor 9" , -1)},
            { "cex_beaver_creek ", new LevelInfo( "Battle Canyon" , -1)},
            { "cex_damnation ", new LevelInfo( "Penance" , -1)},
            { "cex_ff_halo ", new LevelInfo( "Installation 04" , -1)},
            { "cex_hangemhigh ", new LevelInfo( "High Noon" , -1)},
            { "cex_headlong ", new LevelInfo( "Breakneck" , -1)},
            { "cex_prisoner ", new LevelInfo( "Solitary" , -1)},
            { "cex_timberland ", new LevelInfo( "Ridgeline" , -1)},
            { "condemned ", new LevelInfo( "Condemned" , -1)},
            { "ff_unearthed ", new LevelInfo( "Unearthed" , -1)},
            { "trainingpreserve ", new LevelInfo( "Highlands" , -1)},
            { "ff10_prototype ", new LevelInfo( "Overlook" , -1)},
            { "ff20_courtyard ", new LevelInfo( "Courtyard" , -1)},
            { "ff30_waterfront ", new LevelInfo( "Waterfront" , -1)},
            { "ff45_corvette ", new LevelInfo( "Corvette" , -1)},
            { "ff50_park ", new LevelInfo( "Beachhead" , -1)},
            { "ff60_airview ", new LevelInfo( "Outpost" , -1)},
            { "ff60 icecave ", new LevelInfo( "Glacier" , -1)},
            { "ff70_holdout ", new LevelInfo( "Holdout" , -1)},

        };


        public static readonly Dictionary<string, LevelInfo> LevelCodeToNameH4 = new Dictionary<string, LevelInfo>()
        { 


             // Halo 4
            //SP
            { "m05_prologue", new LevelInfo( "Prologue" , 0)},
            { "m10_crash", new LevelInfo( "Dawn" , 1)},
            { "m020", new LevelInfo( "Requiem" , 2)},
            { "m30_cryptum", new LevelInfo( "Forerunner" , 3)},
            { "m40_invasion", new LevelInfo( "Reclaimer" , 4)},
            { "m60_rescue", new LevelInfo( "Infinity" , 5)},
            { "m70_liftoff", new LevelInfo( "Shutdown" , 6)},
            { "m80_delta", new LevelInfo( "Composer" , 7)},
            { "m90_sacrifice", new LevelInfo( "Midnight" , 8)},
            { "m95_epilogue", new LevelInfo( "Epilogue" , 9)},
            //Spartan Ops
            { "ff87_chopperbowl", new LevelInfo( "Quarry" , -1)},
            { "ff86_sniperalley", new LevelInfo( "Sniper Alley" , -1)},
            { "ff90_fortsw", new LevelInfo( "Fortress" , -1)},
            { "ff84_temple", new LevelInfo( "The Refuge" , -1)},
            { "ff82_scurve", new LevelInfo( "The Cauldron" , -1)},
            { "ff81_courtyard", new LevelInfo( "The Gate" , -1)},
            { "ff91_complex", new LevelInfo( "The Galileo" , -1)},
            { "ff92_valhalla", new LevelInfo( "Two Giants" , -1)},
            { "dlc01_factory", new LevelInfo( "Lockup" , -1)},
            { "ff151_mezzanine", new LevelInfo( "Control" , -1)},
            { "ff153_caverns", new LevelInfo( "Warrens" , -1)},
            { "ff152_vortex", new LevelInfo( "Cyclone" , -1)},
            { "ff155_breach", new LevelInfo( "Harvester" , -1)},
            { "ff154_hillside", new LevelInfo( "Apex" , -1)},
            { "dlc01_engine", new LevelInfo( "Infinity" , -1)},
            //MP -- I haven't double checked that all of these are correct
            { "ca_blood_cavern", new LevelInfo( "Abandon" , -1)},
            { "ca_blood_crash", new LevelInfo( "Exile" , -1)},
            { "ca_canyon", new LevelInfo( "Meltdown" , -1)},
            { "ca_forge_bonzanza", new LevelInfo( "Impact" , -1)},
            { "ca_forge_erosion", new LevelInfo( "Erosion" , -1)},
            { "ca_forge_ravine", new LevelInfo( "Ravine" , -1)},
            { "ca_gore_valley", new LevelInfo( "Longbow" , -1)},
            { "ca_redoubt", new LevelInfo( "Vortex" , -1)},
            { "ca_tower", new LevelInfo( "Solace" , -1)},
            { "ca_warhouse", new LevelInfo( "Adrift" , -1)},
            { "ca_wraparound", new LevelInfo( "Haven" , -1)},
            { "dlc_forge_island", new LevelInfo( "Forge Island" , -1)},
            { "dlc dejewel", new LevelInfo( "Shatter" , -1)},
            { "dlc dejunkyard", new LevelInfo( "Wreckage" , -1)},
            { "z05_cliffside", new LevelInfo( "Complex" , -1)},
            { "zd_02_grind", new LevelInfo( "Harvest" , -1)},
            { "ca deadlycrossing", new LevelInfo( "Monolith" , -1)},
            { "ca_port", new LevelInfo( "Landfall" , -1)},
            { "ca_rattler", new LevelInfo( "Skyline" , -1)},
            { "ca_basin", new LevelInfo( "Outcast" , -1)},
            { "ca_highrise", new LevelInfo( "Perdition" , -1)},
            { "ca_spiderweb", new LevelInfo( "Daybreak" , -1)},
            { "ca_creeper", new LevelInfo( "Pitfall" , -1)},
            { "ca_dropoff", new LevelInfo( "Daybreak" , -1)}, //nfi why there's two daybreaks

        };

        public static Dictionary<string, LevelInfo>[] GameToLevelCodeDictionary =
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
