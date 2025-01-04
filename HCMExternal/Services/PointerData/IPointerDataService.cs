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
        /// Public accessor for PointerData of generic data types.
        /// </summary>
        /// <param name="dataName">Name of the data.</param>
        /// <param name="gameVersion">Which version of MCC/cartographer do you need the data for?</param>
        /// <returns>could be int or string or whatever</returns>
        public T GetData<T>(string dataName, string gameVersion);

        // as above but you also need to provide process type and game. 
        public T GetGameProcessData<T>(HaloProcessType process, HaloGame game, string pointerName, string pointerVersion);


        public string HighestSupportedMCCVersion { get; init; }
        public string HighestSupportedCartographerVersion { get; init; }
        public List<string> LatestHCMVersions { get; init; }
        public List<string> ObsoleteHCMVersions { get; init; }
    }
}
