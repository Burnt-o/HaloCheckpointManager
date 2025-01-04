using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal
{
    public static class Utility
    {
        // Generally used for parsing xml offsets
        public static int ParseHexadecimalString(string s)
        {
            if (s == "0x0" || s == "0")
                return 0;

            int sign = 1;
            if (s.StartsWith("-"))
            {
                sign = -1;
                s = s.Substring(1);
            }

            return s.StartsWith("0x") ?
                Convert.ToInt32(s.Substring(2), 16) * sign :
                Convert.ToInt32(s) * sign;
        }

        public static string ToVerString(this FileVersionInfo versionInfo)
        {
            return string.Format("{0}.{1}.{2}.{3}", versionInfo.FileMajorPart,
                                                    versionInfo.FileMinorPart,
                                                      versionInfo.FileBuildPart,
                                                      versionInfo.FilePrivatePart);
        }
    }
}
