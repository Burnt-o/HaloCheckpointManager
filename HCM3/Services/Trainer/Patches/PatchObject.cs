using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCM3.Services.Trainer
{
    public class PatchObject
    {
        public string Game { get; init; }
        public bool Applied { get; set; }

        public IntPtr? PatchHandle { get; set; }
        public string PointerName { get; init; }


        public PatchObject(string game, string pointerName)
        { 
        Game = game;
            PointerName = pointerName;
            PatchHandle = null;
            Applied = false;
        }
    }
}
