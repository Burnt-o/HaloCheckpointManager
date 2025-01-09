using HCMExternal.Services.PointerData.Impl;
using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace HCMExternal.Services.PointerData
{
    public class InjectRequirements
    {
        public bool singleCheckpoint { get; init; } // Game uses a single checkpoint slot as opposoed to two alternating slots
        public bool preserveLocations { get; init; } // Whether to not overwrite certain parts of checkpoint memory according to a PreserveLocationArray
        public bool SHA { get; init; } // Whether or not the checkpoint has a SHA checksum that will need to be recalculated
        public bool BSP { get; init; } // Whether or not the game will need to have checkpoint-cache-bsp index set to match the checkpoints BSP index.

        public InjectRequirements(XElement entry)
        {
            singleCheckpoint = ((bool?)entry.Element("singleCheckpoint")) ?? throw new Exception("Failed to read singleCheckpoint field");
            preserveLocations = ((bool?)entry.Element("preserveLocations")) ?? throw new Exception("Failed to read preserveLocations field");
            SHA = ((bool?)entry.Element("SHA")) ?? throw new Exception("Failed to read SHA field");
            BSP = ((bool?)entry.Element("BSP")) ?? throw new Exception("Failed to read BSP field");


            Log.Verbose("singleCheckpoint: " + singleCheckpoint);
        }

    }


}
