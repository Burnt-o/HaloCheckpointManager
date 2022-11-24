using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCM3.Helpers
{
    public struct HaloStateStruct
    {
        public int GameState;
        public string? LevelName;

        public HaloStateStruct(int gameState, string? levelName)
        {
            this.GameState = gameState;
            this.LevelName = levelName;
        }

        public HaloStateStruct(int gameState)
        { 
        this.GameState=gameState;
            this.LevelName = null;
        }


    }
}
