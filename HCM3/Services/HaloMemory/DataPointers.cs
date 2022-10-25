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
using Keystone;
using HCM3.Services.Trainer;

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

        



    }
}
