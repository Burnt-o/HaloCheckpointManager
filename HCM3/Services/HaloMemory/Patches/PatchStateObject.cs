using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Helpers;

namespace HCM3.Services.HaloMemory
{

    //Used to keep track of whether a patch is applied or not and how to apply it
    public class PatchStateObject
    {
        public GameStateEnum Game { get; init; }
        public bool Applied { get; set; }

        public IntPtr? PatchHandle { get; set; }
        public string PointerName { get; init; }

        public bool IsDetour { get; init; }


        public PatchStateObject(GameStateEnum game, string pointerName, bool isDetour)
        { 
            IsDetour = isDetour;
        Game = game;
            PointerName = pointerName;
            PatchHandle = null;
            Applied = false;
        }
    }
}
