using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal
{

    public enum HaloProcessType
    {
        MCCSteam,
        MCCWinStore,
        ProjectCartographer
    }
    public static class HaloProcessTypeMethods
    {
        public static HaloProcessType FromString(string str)
        {
            switch (str)
            {
                case "MCCSteam": return HaloProcessType.MCCSteam;
                case "MCCWinStore": return HaloProcessType.MCCWinStore;
                case "ProjectCartographer": return HaloProcessType.ProjectCartographer;
                default: throw new System.Exception("Bad HaloProcessType String: " + str);
            }

        }
    }
}
