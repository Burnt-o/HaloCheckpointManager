using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal
{

    public struct LevelInfo
    {
        public string FullName { get; }
        public int LevelPosition { get; }


        public LevelInfo(HaloGame gameEnum, string levelCode)
        {
            switch ((gameEnum, levelCode)) // c# has multivariable switching! that's cool
            {
                // Halo 1
                //SP
                case (HaloGame.Halo1, "a10"): this.FullName = "Pillar of Autumn"; this.LevelPosition = 0; break;
                case (HaloGame.Halo1, "a30"): this.FullName = "Halo"; this.LevelPosition = 1; break;
                case (HaloGame.Halo1, "a50"): this.FullName = "Truth and Rec"; this.LevelPosition = 2; break;
                case (HaloGame.Halo1, "b30"): this.FullName = "Silent Cartographer"; this.LevelPosition = 3; break;
                case (HaloGame.Halo1, "b40"): this.FullName = "AotCR"; this.LevelPosition = 4; break;
                case (HaloGame.Halo1, "c10"): this.FullName = "343 Guilty Spark"; this.LevelPosition = 5; break;
                case (HaloGame.Halo1, "c20"): this.FullName = "The Library"; this.LevelPosition = 6; break;
                case (HaloGame.Halo1, "c40"): this.FullName = "Two Betrayals"; this.LevelPosition = 7; break;
                case (HaloGame.Halo1, "d20"): this.FullName = "Keyes"; this.LevelPosition = 8; break;
                case (HaloGame.Halo1, "d40"): this.FullName = "The Maw"; this.LevelPosition = 9; break;
                //MP
                case (HaloGame.Halo1, "beavercreek"): this.FullName = "Battle Creek"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "boardingaction"): this.FullName = "Boarding Action"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "bloodgulch"): this.FullName = "Blood Gulch"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "carousel"): this.FullName = "Derelict"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "chillout"): this.FullName = "Chill Out"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "damnation"): this.FullName = "Damnation"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "dangercanyon"): this.FullName = "Danger Canyon"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "deathisland"): this.FullName = "Death Island"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "gephyrophobia"): this.FullName = "Gephyrophobia"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "hangemhigh"): this.FullName = "Hang 'Em High"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "icefields"): this.FullName = "Ice Fields"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "infinity"): this.FullName = "Infinity"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "longest"): this.FullName = "Longest"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "prisoner"): this.FullName = "Prisoner"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "putput"): this.FullName = "Chiron TL-34"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "ratrace"): this.FullName = "Rat Race"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "sidewinder"): this.FullName = "Sidewinder"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "timberland"): this.FullName = "Timberland"; this.LevelPosition = -1; break;
                case (HaloGame.Halo1, "wizard"): this.FullName = "Wizard"; this.LevelPosition = -1; break;




                // Halo 2 (MCC)
                //SP
                case (HaloGame.Halo2, "00a_introduction"): this.FullName = "The Heretic"; this.LevelPosition = 0; break;
                case (HaloGame.Halo2, "01a_tutorial"): this.FullName = "The Armory"; this.LevelPosition = 1; break;
                case (HaloGame.Halo2, "01b_spacestation"): this.FullName = "Cairo Station"; this.LevelPosition = 2; break;
                case (HaloGame.Halo2, "03a_oldmombasa"): this.FullName = "Outskirts"; this.LevelPosition = 3; break;
                case (HaloGame.Halo2, "03b_newmombasa"): this.FullName = "Metropolis"; this.LevelPosition = 4; break;
                case (HaloGame.Halo2, "04a_gasgiant"): this.FullName = "The Arbiter"; this.LevelPosition = 5; break;
                case (HaloGame.Halo2, "04b_floodlab"): this.FullName = "The Oracle"; this.LevelPosition = 6; break;
                case (HaloGame.Halo2, "05a_deltaapproach"): this.FullName = "Delta Halo"; this.LevelPosition = 7; break;
                case (HaloGame.Halo2, "05b_deltatowers"): this.FullName = "Regret"; this.LevelPosition = 8; break;
                case (HaloGame.Halo2, "06a_sentinelwalls"): this.FullName = "Sacred Icon"; this.LevelPosition = 9; break;
                case (HaloGame.Halo2, "06b_floodzone"): this.FullName = "Quarantine Zone"; this.LevelPosition = 10; break;
                case (HaloGame.Halo2, "07a_highcharity"): this.FullName = "Gravemind"; this.LevelPosition = 11; break;
                case (HaloGame.Halo2, "07b_forerunnership"): this.FullName = "High Charity"; this.LevelPosition = 12; break;
                case (HaloGame.Halo2, "08a_deltacliffs"): this.FullName = "Uprising"; this.LevelPosition = 13; break;
                case (HaloGame.Halo2, "08b_deltacontrol"): this.FullName = "The Great Journey"; this.LevelPosition = 14; break;
                //MP
                case (HaloGame.Halo2, "ascension"): this.FullName = "Ascension"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "backwash"): this.FullName = "Backwash"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "beavercreek"): this.FullName = "placeholder"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "burial_mounds"): this.FullName = "Burial Mounds"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "coagulation"): this.FullName = "Coagulation"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "colossus"): this.FullName = "Colossus"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "containment"): this.FullName = "Containment"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "cyclotron"): this.FullName = "Ivory Tower"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "deltatap"): this.FullName = "Sanctuary"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "derelict"): this.FullName = "Desolation"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "dune"): this.FullName = "Relic"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "elongation"): this.FullName = "Elongation"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "foundation"): this.FullName = "Foundation"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "gemini"): this.FullName = "Gemini"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "headlong"): this.FullName = "Headlong"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "highplains"): this.FullName = "Tombstone"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "lockout"): this.FullName = "Lockout"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "midship"): this.FullName = "Midship"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "needle"): this.FullName = "Uplift"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "street_sweeper"): this.FullName = "District"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "triplicate"): this.FullName = "Terminal"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "turf"): this.FullName = "Turf"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "warlock"): this.FullName = "Warlock"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "waterworks"): this.FullName = "Waterworks"; this.LevelPosition = -1; break;
                case (HaloGame.Halo2, "zanzibar"): this.FullName = "Zanzibar"; this.LevelPosition = -1; break;





                // Halo 3
                //SP
                case (HaloGame.Halo3, "005_intro"): this.FullName = "Arrival"; this.LevelPosition = 0; break;
                case (HaloGame.Halo3, "010_jungle"): this.FullName = "Sierra 117"; this.LevelPosition = 1; break;
                case (HaloGame.Halo3, "020_base"): this.FullName = "Crow's Nest"; this.LevelPosition = 2; break;
                case (HaloGame.Halo3, "030_outskirts"): this.FullName = "Tsavo Highway"; this.LevelPosition = 3; break;
                case (HaloGame.Halo3, "040_voi"): this.FullName = "The Storm"; this.LevelPosition = 4; break;
                case (HaloGame.Halo3, "050_floodvoi"): this.FullName = "Floodgate"; this.LevelPosition = 5; break;
                case (HaloGame.Halo3, "070_waste"): this.FullName = "The Ark"; this.LevelPosition = 6; break;
                case (HaloGame.Halo3, "100_citadel"): this.FullName = "The Covenant"; this.LevelPosition = 7; break;
                case (HaloGame.Halo3, "110_hc"): this.FullName = "Cortana"; this.LevelPosition = 8; break;
                case (HaloGame.Halo3, "120_halo"): this.FullName = "Halo"; this.LevelPosition = 9; break;
                case (HaloGame.Halo3, "130_epilogue"): this.FullName = "Epilogue"; this.LevelPosition = 10; break;
                //MP
                case (HaloGame.Halo3, "armory"): this.FullName = "Rat's Nest"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "bunkerworld"): this.FullName = "Standoff"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "chill"): this.FullName = "Narrows"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "chillout"): this.FullName = "Cold Storage"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "construct"): this.FullName = "Construct"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "cyberdyne"): this.FullName = "The Pit"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "deadlock"): this.FullName = "Highground"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "descent"): this.FullName = "Assembly"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "docks"): this.FullName = "Longshore"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "fortress"): this.FullName = "Citadel"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "ghosttown"): this.FullName = "Ghost Town"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "guardian"): this.FullName = "Guardian"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "isolation"): this.FullName = "Isolation"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "lockout"): this.FullName = "Blackout"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "midship"): this.FullName = "Heretic"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "riverworld"): this.FullName = "Valhalla"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "salvation"): this.FullName = "Epitaph"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "sandbox"): this.FullName = "Sandbox"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "shrine"): this.FullName = "Sandtrap"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "sidewinder"): this.FullName = "Avalanche"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "snowbound"): this.FullName = "Snowbound"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "spacecamp"): this.FullName = "Orbital"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "warehouse"): this.FullName = "Foundry"; this.LevelPosition = -1; break;
                case (HaloGame.Halo3, "zanzibar"): this.FullName = "Last Resort"; this.LevelPosition = -1; break;





                // ODST
                //SP -- need to double check these. ODST is weird. also double check cases
                case (HaloGame.Halo3ODST, "c100"): this.FullName = "Prepare to Drop"; this.LevelPosition = 0; break; //aka the cutscene, not the MS1 level
                case (HaloGame.Halo3ODST, "c200"): this.FullName = "Epilogue"; this.LevelPosition = 10; break;
                case (HaloGame.Halo3ODST, "h100"): this.FullName = "Mombasa Streets"; this.LevelPosition = 1; break;
                case (HaloGame.Halo3ODST, "l200"): this.FullName = "Data Hive"; this.LevelPosition = 8; break;
                case (HaloGame.Halo3ODST, "l300"): this.FullName = "Coastal Highway"; this.LevelPosition = 9; break;
                case (HaloGame.Halo3ODST, "sc100"): this.FullName = "Tayari Plaza"; this.LevelPosition = 2; break;
                case (HaloGame.Halo3ODST, "sc110"): this.FullName = "Uplift Reserve"; this.LevelPosition = 3; break;
                case (HaloGame.Halo3ODST, "sc120"): this.FullName = "Kinzingo Boulevard"; this.LevelPosition = 4; break;
                case (HaloGame.Halo3ODST, "sc130"): this.FullName = "ONI Alpha Site"; this.LevelPosition = 5; break;
                case (HaloGame.Halo3ODST, "sc140"): this.FullName = "NMPD HQ"; this.LevelPosition = 6; break;
                case (HaloGame.Halo3ODST, "sc150"): this.FullName = "Kikowani Station"; this.LevelPosition = 7; break;
                //MP
                //imagine





                // Halo Reach
                //SP
                case (HaloGame.HaloReach, "m05"): this.FullName = "Noble Actual"; this.LevelPosition = 0; break;
                case (HaloGame.HaloReach, "m10"): this.FullName = "Winter Contingency"; this.LevelPosition = 1; break;
                case (HaloGame.HaloReach, "m20"): this.FullName = "ONI: Sword Base"; this.LevelPosition = 2; break;
                case (HaloGame.HaloReach, "m30"): this.FullName = "Nightfall"; this.LevelPosition = 3; break;
                case (HaloGame.HaloReach, "m35"): this.FullName = "Tip of the Spear"; this.LevelPosition = 4; break;
                case (HaloGame.HaloReach, "m45"): this.FullName = "Long Night of Solace"; this.LevelPosition = 5; break;
                case (HaloGame.HaloReach, "m50"): this.FullName = "Exodus"; this.LevelPosition = 6; break;
                case (HaloGame.HaloReach, "m52"): this.FullName = "New Alexandria"; this.LevelPosition = 7; break;
                case (HaloGame.HaloReach, "m60"): this.FullName = "The Package"; this.LevelPosition = 8; break;
                case (HaloGame.HaloReach, "m70"): this.FullName = "The Pillar of Autumn"; this.LevelPosition = 9; break;
                case (HaloGame.HaloReach, "m70_a"): this.FullName = "Credits"; this.LevelPosition = 10; break;
                case (HaloGame.HaloReach, "m70_bonus"): this.FullName = "Lone Wolf"; this.LevelPosition = 11; break;
                //MP
                case (HaloGame.HaloReach, "20_sword_slayer"): this.FullName = "Sword Base"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "30_settlement"): this.FullName = "Powerhouse"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "35_island"): this.FullName = "Spire"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "45_aftship"): this.FullName = "Zealot"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "45_launch_station"): this.FullName = "Countdown"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "50_panopticon"): this.FullName = "Boardwalk"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "52_ivory_tower"): this.FullName = "Reflection"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "70_boneyard"): this.FullName = "Boneyard"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "forge_halo"): this.FullName = "Forge World"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "dlc_invasion "): this.FullName = "Breakpoint"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "dlc_medium "): this.FullName = "Tempest"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "dlc_slayer "): this.FullName = "Anchor 9"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "cex_beaver_creek "): this.FullName = "Battle Canyon"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "cex_damnation "): this.FullName = "Penance"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "cex_ff_halo "): this.FullName = "Installation 04"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "cex_hangemhigh "): this.FullName = "High Noon"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "cex_headlong "): this.FullName = "Breakneck"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "cex_prisoner "): this.FullName = "Solitary"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "cex_timberland "): this.FullName = "Ridgeline"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "condemned "): this.FullName = "Condemned"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "ff_unearthed "): this.FullName = "Unearthed"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "trainingpreserve "): this.FullName = "Highlands"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "ff10_prototype "): this.FullName = "Overlook"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "ff20_courtyard "): this.FullName = "Courtyard"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "ff30_waterfront "): this.FullName = "Waterfront"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "ff45_corvette "): this.FullName = "Corvette"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "ff50_park "): this.FullName = "Beachhead"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "ff60_airview "): this.FullName = "Outpost"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "ff60 icecave "): this.FullName = "Glacier"; this.LevelPosition = -1; break;
                case (HaloGame.HaloReach, "ff70_holdout "): this.FullName = "Holdout"; this.LevelPosition = -1; break;




                // Halo 4
                //SP
                case (HaloGame.Halo4, "m05_prologue"): this.FullName = "Prologue"; this.LevelPosition = 0; break;
                case (HaloGame.Halo4, "m10_crash"): this.FullName = "Dawn"; this.LevelPosition = 1; break;
                case (HaloGame.Halo4, "m020"): this.FullName = "Requiem"; this.LevelPosition = 2; break;
                case (HaloGame.Halo4, "m30_cryptum"): this.FullName = "Forerunner"; this.LevelPosition = 3; break;
                case (HaloGame.Halo4, "m40_invasion"): this.FullName = "Reclaimer"; this.LevelPosition = 4; break;
                case (HaloGame.Halo4, "m60_rescue"): this.FullName = "Infinity"; this.LevelPosition = 5; break;
                case (HaloGame.Halo4, "m70_liftoff"): this.FullName = "Shutdown"; this.LevelPosition = 6; break;
                case (HaloGame.Halo4, "m80_delta"): this.FullName = "Composer"; this.LevelPosition = 7; break;
                case (HaloGame.Halo4, "m90_sacrifice"): this.FullName = "Midnight"; this.LevelPosition = 8; break;
                case (HaloGame.Halo4, "m95_epilogue"): this.FullName = "Epilogue"; this.LevelPosition = 9; break;
                //Spartan Ops
                case (HaloGame.Halo4, "ff87_chopperbowl"): this.FullName = "Quarry"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ff86_sniperalley"): this.FullName = "Sniper Alley"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ff90_fortsw"): this.FullName = "Fortress"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ff84_temple"): this.FullName = "The Refuge"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ff82_scurve"): this.FullName = "The Cauldron"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ff81_courtyard"): this.FullName = "The Gate"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ff91_complex"): this.FullName = "The Galileo"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ff92_valhalla"): this.FullName = "Two Giants"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "dlc01_factory"): this.FullName = "Lockup"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ff151_mezzanine"): this.FullName = "Control"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ff153_caverns"): this.FullName = "Warrens"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ff152_vortex"): this.FullName = "Cyclone"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ff155_breach"): this.FullName = "Harvester"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ff154_hillside"): this.FullName = "Apex"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "dlc01_engine"): this.FullName = "Infinity"; this.LevelPosition = -1; break;
                //MP -- I haven't double checked that all of these are correct
                case (HaloGame.Halo4, "ca_blood_cavern"): this.FullName = "Abandon"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_blood_crash"): this.FullName = "Exile"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_canyon"): this.FullName = "Meltdown"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_forge_bonzanza"): this.FullName = "Impact"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_forge_erosion"): this.FullName = "Erosion"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_forge_ravine"): this.FullName = "Ravine"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_gore_valley"): this.FullName = "Longbow"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_redoubt"): this.FullName = "Vortex"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_tower"): this.FullName = "Solace"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_warhouse"): this.FullName = "Adrift"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_wraparound"): this.FullName = "Haven"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "dlc_forge_island"): this.FullName = "Forge Island"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "dlc dejewel"): this.FullName = "Shatter"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "dlc dejunkyard"): this.FullName = "Wreckage"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "z05_cliffside"): this.FullName = "Complex"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "zd_02_grind"): this.FullName = "Harvest"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca deadlycrossing"): this.FullName = "Monolith"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_port"): this.FullName = "Landfall"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_rattler"): this.FullName = "Skyline"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_basin"): this.FullName = "Outcast"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_highrise"): this.FullName = "Perdition"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_spiderweb"): this.FullName = "Daybreak"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_creeper"): this.FullName = "Pitfall"; this.LevelPosition = -1; break;
                case (HaloGame.Halo4, "ca_dropoff"): this.FullName = "Daybreak"; this.LevelPosition = -1; break; //nfi why there's two daybreaks




                // Halo 2 (Project Cartographer) TODO: check this
                //SP
                case (HaloGame.ProjectCartographer, "00a_introduction"): this.FullName = "The Heretic"; this.LevelPosition = 0; break;
                case (HaloGame.ProjectCartographer, "01a_tutorial"): this.FullName = "The Armory"; this.LevelPosition = 1; break;
                case (HaloGame.ProjectCartographer, "01b_spacestation"): this.FullName = "Cairo Station"; this.LevelPosition = 2; break;
                case (HaloGame.ProjectCartographer, "03a_oldmombasa"): this.FullName = "Outskirts"; this.LevelPosition = 3; break;
                case (HaloGame.ProjectCartographer, "03b_newmombasa"): this.FullName = "Metropolis"; this.LevelPosition = 4; break;
                case (HaloGame.ProjectCartographer, "04a_gasgiant"): this.FullName = "The Arbiter"; this.LevelPosition = 5; break;
                case (HaloGame.ProjectCartographer, "04b_floodlab"): this.FullName = "The Oracle"; this.LevelPosition = 6; break;
                case (HaloGame.ProjectCartographer, "05a_deltaapproach"): this.FullName = "Delta Halo"; this.LevelPosition = 7; break;
                case (HaloGame.ProjectCartographer, "05b_deltatowers"): this.FullName = "Regret"; this.LevelPosition = 8; break;
                case (HaloGame.ProjectCartographer, "06a_sentinelwalls"): this.FullName = "Sacred Icon"; this.LevelPosition = 9; break;
                case (HaloGame.ProjectCartographer, "06b_floodzone"): this.FullName = "Quarantine Zone"; this.LevelPosition = 10; break;
                case (HaloGame.ProjectCartographer, "07a_highcharity"): this.FullName = "Gravemind"; this.LevelPosition = 11; break;
                case (HaloGame.ProjectCartographer, "07b_forerunnership"): this.FullName = "High Charity"; this.LevelPosition = 12; break;
                case (HaloGame.ProjectCartographer, "08a_deltacliffs"): this.FullName = "Uprising"; this.LevelPosition = 13; break;
                case (HaloGame.ProjectCartographer, "08b_deltacontrol"): this.FullName = "The Great Journey"; this.LevelPosition = 14; break;
                //MP
                case (HaloGame.ProjectCartographer, "ascension"): this.FullName = "Ascension"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "backwash"): this.FullName = "Backwash"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "beavercreek"): this.FullName = "placeholder"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "burial_mounds"): this.FullName = "Burial Mounds"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "coagulation"): this.FullName = "Coagulation"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "colossus"): this.FullName = "Colossus"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "containment"): this.FullName = "Containment"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "cyclotron"): this.FullName = "Ivory Tower"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "deltatap"): this.FullName = "Sanctuary"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "derelict"): this.FullName = "Desolation"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "dune"): this.FullName = "Relic"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "elongation"): this.FullName = "Elongation"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "foundation"): this.FullName = "Foundation"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "gemini"): this.FullName = "Gemini"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "headlong"): this.FullName = "Headlong"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "highplains"): this.FullName = "Tombstone"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "lockout"): this.FullName = "Lockout"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "midship"): this.FullName = "Midship"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "needle"): this.FullName = "Uplift"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "street_sweeper"): this.FullName = "District"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "triplicate"): this.FullName = "Terminal"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "turf"): this.FullName = "Turf"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "warlock"): this.FullName = "Warlock"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "waterworks"): this.FullName = "Waterworks"; this.LevelPosition = -1; break;
                case (HaloGame.ProjectCartographer, "zanzibar"): this.FullName = "Zanzibar"; this.LevelPosition = -1; break;

                default:
                    throw new System.Exception("Invalid LevelCode: " + levelCode + " for game: " + gameEnum);

            }

        }
    };

    


    }
