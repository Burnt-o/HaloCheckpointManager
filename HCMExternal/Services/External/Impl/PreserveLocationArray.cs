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
    public struct PreserveLocation
    { 
        public int Offset { get; init; }
        public int Length { get; init; }
    }

    public struct PreserveLocationArray
    {
        public List<PreserveLocation> data { get; init; } = new();

        public PreserveLocationArray(XElement entry)
        {
            foreach (XElement PreserveLocation in entry.Elements("PreserveLocation") ?? throw new Exception("PreserveLocationArray xml missing PreserveLocations"))
            {
                int Offset = Utility.ParseHexadecimalString(PreserveLocation.Element("Offset")?.Value ?? throw new Exception("PreserveLocation missing Offset entry"));
                int Length = Utility.ParseHexadecimalString(PreserveLocation.Element("Length")?.Value ?? throw new Exception("PreserveLocation missing Length entry"));
                data.Add(new PreserveLocation() { Offset = Offset, Length = Length });
            }
        }
    }

}
