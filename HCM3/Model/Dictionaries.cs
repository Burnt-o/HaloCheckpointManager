using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCM3.Model
{
    internal static class Dictionaries
    {

        public static readonly Dictionary<int, string> TabIndexToRootFolderPath = new()
        {
            { 0, @"Halo 1" },
            { 1, @"Halo 2" },
            { 2, @"Halo 3" },
            { 3, @"Halo 3 ODST" },
            { 4, @"Halo Reach" },
            { 5, @"Halo 4" },
        };

        public static readonly Dictionary<int, string> TabIndexTo2LetterGameCode = new()
        {
            { 0, @"H1" },
            { 1, @"H2" },
            { 2, @"H3" },
            { 3, @"OD" },
            { 4, @"HR" },
            { 5, @"H4" },
        };

    }
}
