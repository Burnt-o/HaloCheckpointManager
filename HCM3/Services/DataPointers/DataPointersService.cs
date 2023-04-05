using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using System.Xml.Linq;
using System.Xml.XPath;
using System.Diagnostics;
using System.IO;
using System.Xml;
using HCM3.Models;
using HCM3.Helpers;
using HCM3.Services.Cheats;

namespace HCM3.Services
{
    public partial class DataPointersService
    {

        private Dictionary<string, Dictionary<string, Object>> PointerData { get; set; } = new()
        {
        };


        public DataPointersService()
        {
            PointerData = new();
            HighestSupportedMCCVersion = null;

        }



        public string? HighestSupportedMCCVersion { get; set; }
        public string? LatestHCMVersion { get; set; }
        public List<string> LatestHCMVersions = new();
        public List<string> ObsoleteHCMVersions = new();


        public object? GetPointer(string? pointerName, string? pointerVersion)
        {
            if (pointerName == null || pointerVersion == null)
            {
                return null;
            }

            bool success = PointerData.TryGetValue(pointerName, out Dictionary<string, object>? pointerVersionDictionary);
            if (!success || pointerVersionDictionary == null)
            {
                return null;
            }

            success = pointerVersionDictionary.TryGetValue(pointerVersion, out object? pointer);
            if (!success || pointer == null)
            {
                return null;
            }

            return (object)pointer;
        }



        // If you just want a single pointer
        public object? GetRequiredPointers(string pointerName, HaloState haloState)
        {
            return GetPointer(pointerName, haloState.MCCVersion);
        }

        // If you want a whole bunch
        public Dictionary<string, object> GetRequiredPointers(List<string> requiredPointerNames, HaloState haloState)
        {
            if (haloState.MCCVersion == null) throw new InvalidOperationException("HCM couldn't detect which version of MCC was running");

            Dictionary<string, object> result = new Dictionary<string,object>();
            foreach (string requiredPointerName in requiredPointerNames)
            {
                object? pointer = GetPointer(requiredPointerName, haloState.MCCVersion);
                if (pointer == null) throw new InvalidOperationException("HCM doesn't have offsets loaded to perform this operation with this version of MCC." + $"\nSpecifically: {requiredPointerName}" + ", MCC version: " + haloState.MCCVersion);
                
                // the "[3..]" is to remove the first 3 letters of the pointerName, ie the "H1_" or "H3_"
                result.Add(requiredPointerName[3..], pointer);
            }

            return result;
            
        }





    }
}
