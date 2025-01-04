using System;
using System.Collections.Generic;

namespace HCMExternal
{

    public enum HaloGame
    {
        Halo1 = 0,
        Halo2 = 1,
        Halo3 = 2,
        Halo3ODST = 3,
        HaloReach = 4,
        Halo4 = 5,
        ProjectCartographer = 6
    }


    public static class HaloGameMethods
    {

        public static int ToInternalIndex(this HaloGame gameEnum)
        {
            switch (gameEnum)
            {
                case HaloGame.Halo1: return 0;
                case HaloGame.Halo2: return 1;
                case HaloGame.Halo3: return 2;
                case HaloGame.Halo3ODST: return 5;
                case HaloGame.HaloReach: return 6;
                case HaloGame.Halo4: return 3;
                case HaloGame.ProjectCartographer: return 7;
                default: throw new System.Exception("Invalid HaloGame index: " + gameEnum);
            }
        }


        public static string ToRootFolderPath(this HaloGame gameEnum)
        {
            switch (gameEnum)
            {
                case HaloGame.Halo1: return @"Halo 1";
                case HaloGame.Halo2: return @"Halo 2";
                case HaloGame.Halo3: return @"Halo 3";
                case HaloGame.Halo3ODST: return @"Halo 3 ODST";
                case HaloGame.HaloReach: return @"Halo Reach";
                case HaloGame.Halo4: return @"Halo 4";
                case HaloGame.ProjectCartographer: return @"Project Cartographer";
                default: throw new System.Exception("Invalid HaloGame index: " + gameEnum);
            }
        }

        public static string ToAcronym(this HaloGame gameEnum)
        {
            switch (gameEnum)
            {
                case HaloGame.Halo1: return @"H1";
                case HaloGame.Halo2: return @"H2";
                case HaloGame.Halo3: return @"H3";
                case HaloGame.Halo3ODST: return @"OD";
                case HaloGame.HaloReach: return @"HR";
                case HaloGame.Halo4: return @"H4";
                case HaloGame.ProjectCartographer: return @"PC";
                default: throw new System.Exception("Invalid HaloGame index: " + gameEnum);
            }
        }
        

        public static HaloGame FromAcronym(string acronym)
        {
            switch (acronym)
            {
                case "H1": return HaloGame.Halo1;
                case "H2": return HaloGame.Halo2;
                case "H3": return HaloGame.Halo3;
                case "OD": return HaloGame.Halo3ODST;
                case "HR": return HaloGame.HaloReach;
                case "H4": return HaloGame.Halo4;
                case "PC": return HaloGame.ProjectCartographer;
                default: throw new System.Exception("Invalid HaloGame acronym: " + acronym);
            }
        }
    }
}
