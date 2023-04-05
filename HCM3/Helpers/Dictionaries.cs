using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCM3.Helpers
{
    public static class Dictionaries
    {

        public static string NullableIntPtrToHexString(IntPtr? ptr)
        {
            if (ptr == null) return "Null";
            return ptr.Value.ToString("X");
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



        public static readonly Dictionary<int, string> GameToDLLname = new()
        {
            { 0, @"halo1.dll" },
            { 1, @"halo2.dll" },
            { 2, @"halo3.dll" },
            { 3, @"halo3odst.dll" },
            { 4, @"haloreach.dll" },
            { 5, @"halo4.dll" },
        };




        public static readonly Dictionary<string, LevelInfo> LevelCodeToLevelInfoH1 = new Dictionary<string, LevelInfo>()
        { 
         // Halo 1
            //SP
            { "a10", new LevelInfo("a10", "Pillar of Autumn", 0) },
            { "a30", new LevelInfo("a30", "Halo", 1) },
            { "a50", new LevelInfo("a50", "Truth and Rec", 2) },
            { "b30", new LevelInfo("b30", "Silent Cartographer", 3) },
            { "b40", new LevelInfo("b40", "AotCR", 4) },
            { "c10", new LevelInfo("c10", "343 Guilty Spark", 5) },
            { "c20", new LevelInfo("c20", "The Library", 6) },
            { "c40", new LevelInfo("c40", "Two Betrayals", 7) },
            { "d20", new LevelInfo("d20", "Keyes", 8) },
            { "d40", new LevelInfo("d40", "The Maw", 9) },
            //MP
            { "beavercreek", new LevelInfo("beavercreek", "Battle Creek", -1) },
            { "boardingaction", new LevelInfo("boardingaction", "Boarding Action", -1) },
            { "bloodgulch", new LevelInfo("bloodgulch", "Blood Gulch", -1) },
            { "carousel", new LevelInfo("carousel", "Derelict", -1) },
            { "chillout", new LevelInfo("chillout", "Chill Out", -1) },
            { "damnation", new LevelInfo("damnation", "Damnation", -1) },
            { "dangercanyon", new LevelInfo("dangercanyon", "Danger Canyon", -1) },
            { "deathisland", new LevelInfo("deathisland", "Death Island", -1) },
            { "gephyrophobia", new LevelInfo("gephyrophobia", "Gephyrophobia", -1) },
            { "hangemhigh", new LevelInfo("hangemhigh", "Hang 'Em High", -1) },
            { "icefields", new LevelInfo("icefields", "Ice Fields", -1) },
            { "infinity", new LevelInfo("infinity", "Infinity", -1) },
            { "longest", new LevelInfo("longest", "Longest", -1) },
            { "prisoner", new LevelInfo("prisoner", "Prisoner", -1) },
            { "putput", new LevelInfo("putput", "Chiron TL-34", -1) },
            { "ratrace", new LevelInfo("ratrace", "Rat Race", -1) },
            { "sidewinder", new LevelInfo("sidewinder", "Sidewinder", -1) },
            { "timberland", new LevelInfo("timberland", "Timberland", -1) },
            { "wizard", new LevelInfo("wizard", "Wizard", -1) },

        };

        public static readonly Dictionary<string, LevelInfo> LevelCodeToLevelInfoH2 = new Dictionary<string, LevelInfo>()
        { 

            // Halo 2
            //SP
            { "00a_introduction", new LevelInfo("00a_introduction", "The Heretic", 0) },
            { "01a_tutorial", new LevelInfo("01a_tutorial", "The Armory" , 1)},
            { "01b_spacestation", new LevelInfo("01b_spacestation", "Cairo Station" , 2)},
            { "03a_oldmombasa", new LevelInfo("03a_oldmombasa", "Outskirts" , 3)},
            { "03b_newmombasa", new LevelInfo("03b_newmombasa", "Metropolis" , 4)},
            { "04a_gasgiant", new LevelInfo("04a_gasgiant", "The Arbiter" , 5)},
            { "04b_floodlab", new LevelInfo("04b_floodlab", "The Oracle" , 6)},
            { "05a_deltaapproach", new LevelInfo("05a_deltaapproach", "Delta Halo" , 7)},
            { "05b_deltatowers", new LevelInfo("05b_deltatowers", "Regret" , 8)},
            { "06a_sentinelwalls", new LevelInfo("06a_sentinelwalls", "Sacred Icon" , 9)},
            { "06b_floodzone", new LevelInfo("06b_floodzone", "Quarantine Zone" ,10)},
            { "07a_highcharity", new LevelInfo("07a_highcharity", "Gravemind" , 11)},
            { "07b_forerunnership", new LevelInfo("07b_forerunnership", "High Charity" , 12)},
            { "08a_deltacliffs", new LevelInfo("08a_deltacliffs", "Uprising" , 13)},
            { "08b_deltacontrol", new LevelInfo("08b_deltacontrol", "The Great Journey" , 14)},
            //MP
            { "ascension", new LevelInfo("ascension", "Ascension" , -1)},
            { "backwash", new LevelInfo("backwash", "Backwash" , -1)},
            { "beavercreek", new LevelInfo("beavercreek", "Beaver Creek" , -1)},
            { "burial_mounds", new LevelInfo("burial_mounds", "Burial Mounds" , -1)},
            { "coagulation", new LevelInfo("coagulation", "Coagulation" , -1)},
            { "colossus", new LevelInfo("colossus", "Colossus" , -1)},
            { "containment", new LevelInfo("containment", "Containment" , -1)},
            { "cyclotron", new LevelInfo("cyclotron", "Ivory Tower" , -1)},
            { "deltatap", new LevelInfo("deltatap", "Sanctuary" , -1)},
            { "derelict", new LevelInfo("derelict", "Desolation" , -1)},
            { "dune", new LevelInfo("dune", "Relic" , -1)},
            { "elongation", new LevelInfo("elongation", "Elongation" , -1)},
            { "foundation", new LevelInfo("foundation", "Foundation" , -1)},
            { "gemini", new LevelInfo("gemini", "Gemini" , -1)},
            { "headlong", new LevelInfo("headlong", "Headlong" , -1)},
            { "highplains", new LevelInfo("highplains", "Tombstone" , -1)},
            { "lockout", new LevelInfo("lockout", "Lockout" , -1)},
            { "midship", new LevelInfo("midship", "Midship" , -1)},
            { "needle", new LevelInfo("needle", "Uplift" , -1)},
            { "street_sweeper", new LevelInfo("street_sweeper", "District" , -1)},
            { "triplicate", new LevelInfo("triplicate", "Terminal" , -1)},
            { "turf", new LevelInfo("turf", "Turf" , -1)},
            { "warlock", new LevelInfo("warlock", "Warlock" , -1)},
            { "waterworks", new LevelInfo("waterworks", "Waterworks" , -1)},
            { "zanzibar", new LevelInfo("zanzibar", "Zanzibar" , -1)},

        };

        public static readonly Dictionary<string, LevelInfo> LevelCodeToLevelInfoH3 = new Dictionary<string, LevelInfo>()
        { 


             // Halo 3
            //SP
            { "005_intro", new LevelInfo("005_intro", "Arrival" , 0)},
            { "010_jungle", new LevelInfo("010_jungle", "Sierra 117" , 1)},
            { "020_base", new LevelInfo("020_base", "Crow's Nest" , 2)},
            { "030_outskirts", new LevelInfo("030_outskirts", "Tsavo Highway" , 3)},
            { "040_voi", new LevelInfo("040_voi", "The Storm" , 4)},
            { "050_floodvoi", new LevelInfo("050_floodvoi", "Floodgate" , 5)},
            { "070_waste", new LevelInfo("070_waste", "The Ark" , 6)},
            { "100_citadel", new LevelInfo("100_citadel", "The Covenant" , 7)},
            { "110_hc", new LevelInfo("110_hc", "Cortana" , 8)},
            { "120_halo", new LevelInfo("120_halo", "Halo" , 9)},
            { "130_epilogue", new LevelInfo("130_epilogue", "Epilogue" , 10)},
            //MP
            { "armory", new LevelInfo("armory", "Rat's Nest" , -1)},
            { "bunkerworld", new LevelInfo("bunkerworld", "Standoff" , -1)},
            { "chill", new LevelInfo("chill", "Narrows" , -1)},
            { "chillout", new LevelInfo("chillout", "Cold Storage" , -1)},
            { "construct", new LevelInfo("construct", "Construct" , -1)},
            { "cyberdyne", new LevelInfo("cyberdyne", "The Pit" , -1)},
            { "deadlock", new LevelInfo("deadlock", "Highground" , -1)},
            { "descent", new LevelInfo("descent", "Assembly" , -1)},
            { "docks", new LevelInfo("docks", "Longshore" , -1)},
            { "fortress", new LevelInfo("fortress", "Citadel" , -1)},
            { "ghosttown", new LevelInfo("ghosttown", "Ghost Town" , -1)},
            { "guardian", new LevelInfo("guardian", "Guardian" , -1)},
            { "isolation", new LevelInfo("isolation", "Isolation" , -1)},
            { "lockout", new LevelInfo("lockout", "Blackout" , -1)},
            { "midship", new LevelInfo("midship", "Heretic" , -1)},
            { "riverworld", new LevelInfo("riverworld", "Valhalla" , -1)},
            { "salvation", new LevelInfo("salvation", "Epitaph" , -1)},
            { "sandbox", new LevelInfo("sandbox", "Sandbox" , -1)},
            { "shrine", new LevelInfo("shrine", "Sandtrap" , -1)},
            { "sidewinder", new LevelInfo("sidewinder", "Avalanche" , -1)},
            { "snowbound", new LevelInfo("snowbound", "Snowbound" , -1)},
            { "spacecamp", new LevelInfo("spacecamp", "Orbital" , -1)},
            { "warehouse", new LevelInfo("warehouse", "Foundry" , -1)},
            { "zanzibar", new LevelInfo("zanzibar", "Last Resort" , -1)},

        };

        public static readonly Dictionary<string, LevelInfo> LevelCodeToLevelInfoOD = new Dictionary<string, LevelInfo>()
        { 


             // ODST
            //SP -- need to double check these. ODST is weird. also double check cases
            { "c100", new LevelInfo("c100", "Prepare to Drop" , 0)}, //aka the cutscene, not the MS1 level
            { "c200", new LevelInfo("c200", "Epilogue" , 10)},
            { "h100", new LevelInfo("h100", "Mombasa Streets" , 1)},
            { "l200", new LevelInfo("l200", "Data Hive" , 8)},
            { "l300", new LevelInfo("l300", "Coastal Highway" , 9)},
            { "sc100", new LevelInfo("sc100", "Tayari Plaza" , 2)},
            { "sc110", new LevelInfo("sc110", "Uplift Reserve" , 3)},
            { "sc120", new LevelInfo("sc120", "Kinzingo Boulevard" , 4)},
            { "sc130", new LevelInfo("sc130", "ONI Alpha Site" , 5)},
            { "sc140", new LevelInfo("sc140", "NMPD HQ" , 6)},
            { "sc150", new LevelInfo("sc150", "Kikowani Station" , 7)},
            //MP
            //imagine

        };

        public static readonly Dictionary<string, LevelInfo> LevelCodeToLevelInfoHR = new Dictionary<string, LevelInfo>()
        {

            // Halo Reach
            //SP
            { "m05", new LevelInfo("m05", "Noble Actual" , 0)},
            { "m10", new LevelInfo("m10", "Winter Contingency" , 1)},
            { "m20", new LevelInfo("m20", "ONI: Sword Base" , 2)},
            { "m30", new LevelInfo("m30", "Nightfall" , 3)},
            { "m35", new LevelInfo("m35", "Tip of the Spear" , 4)},
            { "m45", new LevelInfo("m45", "Long Night of Solace" , 5)},
            { "m50", new LevelInfo("m50", "Exodus" , 6)},
            { "m52", new LevelInfo("m52", "New Alexandria" , 7)},
            { "m60", new LevelInfo("m60", "The Package" , 8)},
            { "m70", new LevelInfo( "m70","The Pillar of Autumn" , 9)},
            { "m70_a", new LevelInfo("m70_a", "Credits" , 10)},
            { "m70_bonus", new LevelInfo("m70_bonus", "Lone Wolf" , 11)},
            //MP
            { "20_sword_slayer", new LevelInfo("20_sword_slayer", "Sword Base" , -1)},
            { "30_settlement", new LevelInfo("30_settlement", "Powerhouse" , -1)},
            { "35_island", new LevelInfo("35_island", "Spire" , -1)},
            { "45_aftship", new LevelInfo("45_aftship", "Zealot" , -1)},
            { "45_launch_station", new LevelInfo("45_launch_station", "Countdown" , -1)},
            { "50_panopticon", new LevelInfo("50_panopticon", "Boardwalk" , -1)},
            { "52_ivory_tower", new LevelInfo("52_ivory_tower", "Reflection" , -1)},
            { "70_boneyard", new LevelInfo("70_boneyard", "Boneyard" , -1)},
            { "forge_halo", new LevelInfo("forge_halo", "Forge World" , -1)},
            { "dlc_invasion", new LevelInfo("dlc_invasion", "Breakpoint" , -1)},
            { "dlc_medium", new LevelInfo("dlc_medium", "Tempest" , -1)},
            { "dlc_slayer", new LevelInfo("dlc_slayer", "Anchor 9" , -1)},
            { "cex_beaver_creek", new LevelInfo("cex_beaver_creek", "Battle Canyon" , -1)},
            { "cex_damnation", new LevelInfo("cex_damnation", "Penance" , -1)},
            { "cex_ff_halo", new LevelInfo("cex_ff_halo", "Installation 04" , -1)},
            { "cex_hangemhigh", new LevelInfo("cex_hangemhigh", "High Noon" , -1)},
            { "cex_headlong", new LevelInfo("cex_headlong", "Breakneck" , -1)},
            { "cex_prisoner", new LevelInfo("cex_prisoner", "Solitary" , -1)},
            { "cex_timberland", new LevelInfo("cex_timberland", "Ridgeline" , -1)},
            { "condemned", new LevelInfo("condemned", "Condemned" , -1)},
            { "ff_unearthed", new LevelInfo("ff_unearthed", "Unearthed" , -1)},
            { "trainingpreserve", new LevelInfo("trainingpreserve", "Highlands" , -1)},
            { "ff10_prototype", new LevelInfo("ff10_prototype", "Overlook" , -1)},
            { "ff20_courtyard", new LevelInfo("ff20_courtyard", "Courtyard" , -1)},
            { "ff30_waterfront", new LevelInfo("ff30_waterfront", "Waterfront" , -1)},
            { "ff45_corvette", new LevelInfo("ff45_corvette", "Corvette" , -1)},
            { "ff50_park", new LevelInfo("ff50_park", "Beachhead" , -1)},
            { "ff60_airview", new LevelInfo("ff60_airview", "Outpost" , -1)},
            { "ff60 icecave", new LevelInfo("ff60 icecave", "Glacier" , -1)},
            { "ff70_holdout", new LevelInfo("ff70_holdout", "Holdout" , -1)},

        };


        public static readonly Dictionary<string, LevelInfo> LevelCodeToLevelInfoH4 = new Dictionary<string, LevelInfo>()
        { 


             // Halo 4
            //SP
            { "m05_prologue", new LevelInfo("m05_prologue", "Prologue" , 0)},
            { "m10_crash", new LevelInfo("m10_crash", "Dawn" , 1)},
            { "m020", new LevelInfo("m020", "Requiem" , 2)},
            { "m30_cryptum", new LevelInfo("m30_cryptum", "Forerunner" , 3)},
            { "m40_invasion", new LevelInfo("m40_invasion", "Reclaimer" , 4)},
            { "m60_rescue", new LevelInfo("m60_rescue", "Infinity" , 5)},
            { "m70_liftoff", new LevelInfo("m70_liftoff", "Shutdown" , 6)},
            { "m80_delta", new LevelInfo("m80_delta", "Composer" , 7)},
            { "m90_sacrifice", new LevelInfo("m90_sacrifice", "Midnight" , 8)},
            { "m95_epilogue", new LevelInfo("m95_epilogue", "Epilogue" , 9)},
            //Spartan Ops
            { "ff87_chopperbowl", new LevelInfo("ff87_chopperbowl", "Quarry" , -1)},
            { "ff86_sniperalley", new LevelInfo("ff86_sniperalley", "Sniper Alley" , -1)},
            { "ff90_fortsw", new LevelInfo("ff90_fortsw", "Fortress" , -1)},
            { "ff84_temple", new LevelInfo("ff84_temple", "The Refuge" , -1)},
            { "ff82_scurve", new LevelInfo("ff82_scurve", "The Cauldron" , -1)},
            { "ff81_courtyard", new LevelInfo("ff81_courtyard", "The Gate" , -1)},
            { "ff91_complex", new LevelInfo("ff91_complex", "The Galileo" , -1)},
            { "ff92_valhalla", new LevelInfo("ff92_valhalla", "Two Giants" , -1)},
            { "dlc01_factory", new LevelInfo("dlc01_factory", "Lockup" , -1)},
            { "ff151_mezzanine", new LevelInfo("ff151_mezzanine", "Control" , -1)},
            { "ff153_caverns", new LevelInfo("ff153_caverns", "Warrens" , -1)},
            { "ff152_vortex", new LevelInfo("ff152_vortex", "Cyclone" , -1)},
            { "ff155_breach", new LevelInfo("ff155_breach", "Harvester" , -1)},
            { "ff154_hillside", new LevelInfo("ff154_hillside", "Apex" , -1)},
            { "dlc01_engine", new LevelInfo("dlc01_engine", "Infinity" , -1)},
            //MP -- I haven't double checked that all of these are correct
            { "ca_blood_cavern", new LevelInfo("ca_blood_cavern", "Abandon" , -1)},
            { "ca_blood_crash", new LevelInfo("ca_blood_crash", "Exile" , -1)},
            { "ca_canyon", new LevelInfo("ca_canyon", "Meltdown" , -1)},
            { "ca_forge_bonzanza", new LevelInfo("ca_forge_bonzanza", "Impact" , -1)},
            { "ca_forge_erosion", new LevelInfo("ca_forge_erosion", "Erosion" , -1)},
            { "ca_forge_ravine", new LevelInfo("ca_forge_ravine", "Ravine" , -1)},
            { "ca_gore_valley", new LevelInfo("ca_gore_valley", "Longbow" , -1)},
            { "ca_redoubt", new LevelInfo("ca_redoubt", "Vortex" , -1)},
            { "ca_tower", new LevelInfo("ca_tower", "Solace" , -1)},
            { "ca_warhouse", new LevelInfo("ca_warhouse", "Adrift" , -1)},
            { "ca_wraparound", new LevelInfo("ca_wraparound", "Haven" , -1)},
            { "dlc_forge_island", new LevelInfo("dlc_forge_island", "Forge Island" , -1)},
            { "dlc dejewel", new LevelInfo("dlc dejewel", "Shatter" , -1)},
            { "dlc dejunkyard", new LevelInfo("dlc dejunkyard", "Wreckage" , -1)},
            { "z05_cliffside", new LevelInfo("z05_cliffside", "Complex" , -1)},
            { "zd_02_grind", new LevelInfo("zd_02_grind", "Harvest" , -1)},
            { "ca deadlycrossing", new LevelInfo("ca deadlycrossing", "Monolith" , -1)},
            { "ca_port", new LevelInfo("ca_port", "Landfall" , -1)},
            { "ca_rattler", new LevelInfo("ca_rattler", "Skyline" , -1)},
            { "ca_basin", new LevelInfo("ca_basin", "Outcast" , -1)},
            { "ca_highrise", new LevelInfo("ca_highrise", "Perdition" , -1)},
            { "ca_spiderweb", new LevelInfo("ca_spiderweb", "Daybreak" , -1)},
            { "ca_creeper", new LevelInfo("ca_creeper", "Pitfall" , -1)},
            { "ca_dropoff", new LevelInfo("ca_dropoff", "Daybreak" , -1)}, //nfi why there's two daybreaks

        };

        public static Dictionary<string, LevelInfo>[] GameToLevelInfoDictionary =
        {
        LevelCodeToLevelInfoH1,
        LevelCodeToLevelInfoH2,
        LevelCodeToLevelInfoH3,
        LevelCodeToLevelInfoOD,
        LevelCodeToLevelInfoHR,
        LevelCodeToLevelInfoH4,
        };


        // For displaying hotkeys
        public static readonly Dictionary<string, int> KeyStringToKeyCode = new()
        { { "A", 97 }, { "B", 98 }, { "C", 99 }, { "D", 100 }, { "E", 101 }, { "F", 102 }, { "G", 103 }, { "H", 104 }, { "I", 105 }, { "J", 106 }, { "K", 107 }, { "L", 108 }, { "M", 109 }, { "N", 110 }, { "O", 111 }, { "P", 112 }, { "Q", 113 }, { "R", 114 }, { "S", 115 }, { "T", 116 }, { "U", 117 }, { "V", 118 }, { "W", 119 }, { "X", 120 }, { "Y", 121 }, { "Z", 122 }, { "LEFTARROW", 143 }, { "DOWNARROW", 144 }, { "RIGHTARROW", 145 }, { "UPARROW", 146 }, { "F1", 162 }, { "F2", 163 }, { "F3", 164 }, { "F4", 165 }, { "F5", 166 }, { "F6", 167 }, { "F7", 168 }, { "F8", 169 }, { "F9", 170 }, { "F10", 171 }, { "F11", 182 }, { "F12", 183 }, { "ZERO", 48 }, { "ONE", 49 }, { "TWO", 50 }, { "THREE", 51 }, { "FOUR", 52 }, { "FIVE", 53 }, { "SIX", 54 }, { "SEVEN", 55 }, { "EIGHT", 56 }, { "NINE", 57 }, { "PAD0", 158 }, { "PAD1", 151 }, { "PAD2", 147 }, { "PAD3", 152 }, { "PAD4", 148 }, { "PAD5", 153 }, { "PAD6", 149 }, { "PAD7", 154 }, { "PAD8", 150 }, { "PAD9", 155 }, { "PADPERIOD", 156 }, { "PADSLASH", 157 }, { "PADASTER", 42 }, { "PADMI", 159 }, { "PADPLUS", 161 }, { "PADEN", 160 }, { "ACCENTGRAVE", 137 }, { "BACKSLASH", 139 }, { "BACKSPACE", 133 }, { "COMMA", 44 }, { "DEL", 134 }, { "END", 186 }, { "EQUAL", 140 }, { "ESC", 130 }, { "HOME", 176 }, { "INSERT", 175 }, { "LEFTBRACKET", 141 }, { "LINEFEED", 132 }, { "MINUS", 45 }, { "PAGEDOWN", 178 }, { "PAGEUP", 177 }, { "PAUSE", 174 }, { "PERIOD", 46 }, { "QUOTE", 136 }, { "RIGHTBRACKET", 142 }, { "RET", 13 }, { "SEMICOLON", 135 }, { "SLASH", 138 }, { "TAB", 131 }, { "LEFTCTRL", 124 }, { "LEFTALT", 125 }, { "RIGHTALT", 126 }, { "RIGHTCTRL", 127 }, { "RIGHTSHIFT", 128 }, { "LEFTSHIFT", 129 }, { "SPACE", 32 } };
        
        public static readonly Dictionary<int, string> KeyCodeToKeyString = KeyStringToKeyCode.ToDictionary((x) => x.Value, (x) => x.Key);

      
    }
}
