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

    }
}
