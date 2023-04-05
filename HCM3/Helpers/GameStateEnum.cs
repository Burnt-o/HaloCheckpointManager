using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCM3.Helpers
{
    // One half of HaloState class, along with LevelInfo
    public enum GameStateEnum
    {
        //UN = -2, // Unattached
        //MN = -1, // Menu
        //H1 = 0, // Halo 1
        //H2 = 1, // Halo 2
        //H3 = 2, // Halo 3
        //OD = 3, // Halo 3: ODST
        //HR = 4, // Halo Reach
        //H4 = 5, // Halo 4

        //Old version
        Unattached = -2,
        Menu = -1,
        Halo1 = 0,
        Halo2 = 1,
        Halo3 = 2,
        Halo3ODST = 3,
        HaloReach = 4,
        Halo4 = 5,
    }

    public static class GameStateEnumExtensions
    {
        // Used to get data from PointerData, eg "H1_LevelCode"
        public static string To2Letters(this GameStateEnum state)
        {
            switch (state)
            {
                case GameStateEnum.Unattached: return "UN";
                case GameStateEnum.Menu: return "MN";
                case GameStateEnum.Halo1: return "H1";
                case GameStateEnum.Halo2: return "H2";
                case GameStateEnum.Halo3: return "H3";
                case GameStateEnum.Halo3ODST: return "OD";
                case GameStateEnum.HaloReach: return "HR";
                case GameStateEnum.Halo4: return "H4";
                default: throw new ArgumentOutOfRangeException();
            }

        }
    }
    
}
