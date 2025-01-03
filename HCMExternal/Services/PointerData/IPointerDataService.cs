using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.Services.PointerData
{
    public interface IPointerDataService
    {
        /// <summary>
        /// Public accessor for PointerData.
        /// </summary>
        /// <param name="pointerName">Name of the pointer.</param>
        /// <param name="pointerVersion">Which version of MCC do you need the data for?</param>
        /// <returns></returns>
        public object? GetPointer(string? pointerName, string? pointerVersion);



        public string HighestSupportedMCCVersion { get; init; }
        public List<string> LatestHCMVersions { get; init; }
        public List<string> ObsoleteHCMVersions { get; init; }
    }
}
