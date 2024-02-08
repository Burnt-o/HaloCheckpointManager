using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.Helpers.DictionariesNS
{

    public enum HaloTabEnum
    {
        Halo1 = 0,
        Halo2 = 1,
        Halo3 = 2,
        Halo3ODST = 3,
        HaloReach = 4,
        Halo4 = 5,
    }



    public static class Dictionaries
    {

        public static int HaloTabEnumToInternalIndex(HaloTabEnum tabEnum)
        {
            switch (tabEnum)
            {
                case HaloTabEnum.Halo1: return 0;
                case HaloTabEnum.Halo2: return 1;
                case HaloTabEnum.Halo3: return 2;
                case HaloTabEnum.Halo3ODST: return 5;
                case HaloTabEnum.HaloReach: return 6;
                case HaloTabEnum.Halo4: return 3;
                default: return 0;
            }
        }


        public static readonly Dictionary<HaloTabEnum, string> GameToRootFolderPath = new()
        {
            { HaloTabEnum.Halo1, @"Halo 1" },
            { HaloTabEnum.Halo2, @"Halo 2" },
            { HaloTabEnum.Halo3, @"Halo 3" },
            { HaloTabEnum.Halo3ODST, @"Halo 3 ODST" },
            { HaloTabEnum.HaloReach, @"Halo Reach" },
            { HaloTabEnum.Halo4, @"Halo 4" },
        };

        public static readonly Dictionary<HaloTabEnum, string> GameTo2LetterGameCode = new()
        {
            { HaloTabEnum.Halo1, @"H1" },
            { HaloTabEnum.Halo2, @"H2" },
            { HaloTabEnum.Halo3, @"H3" },
            { HaloTabEnum.Halo3ODST, @"OD" },
            { HaloTabEnum.HaloReach, @"HR" },
            { HaloTabEnum.Halo4, @"H4" },
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


        public static readonly Dictionary<string, LevelInfo> LevelCodeToLevelInfoH1 = new Dictionary<string, LevelInfo>()
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

        public static readonly Dictionary<string, LevelInfo> LevelCodeToLevelInfoH2 = new Dictionary<string, LevelInfo>()
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

        public static readonly Dictionary<string, LevelInfo> LevelCodeToLevelInfoH3 = new Dictionary<string, LevelInfo>()
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

        public static readonly Dictionary<string, LevelInfo> LevelCodeToLevelInfoOD = new Dictionary<string, LevelInfo>()
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

        public static readonly Dictionary<string, LevelInfo> LevelCodeToLevelInfoHR = new Dictionary<string, LevelInfo>()
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


        public static readonly Dictionary<string, LevelInfo> LevelCodeToLevelInfoH4 = new Dictionary<string, LevelInfo>()
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

        public static Dictionary<HaloTabEnum, Dictionary<string, LevelInfo>> GameToLevelInfoDictionary = new Dictionary<HaloTabEnum, Dictionary<string, LevelInfo>>()
        {
        { HaloTabEnum.Halo1, LevelCodeToLevelInfoH1 },
        { HaloTabEnum.Halo2, LevelCodeToLevelInfoH2 },
        { HaloTabEnum.Halo3, LevelCodeToLevelInfoH3 },
        { HaloTabEnum.Halo3ODST, LevelCodeToLevelInfoOD },
        { HaloTabEnum.HaloReach, LevelCodeToLevelInfoHR },
        { HaloTabEnum.Halo4, LevelCodeToLevelInfoH4 },
        };

    }
}
