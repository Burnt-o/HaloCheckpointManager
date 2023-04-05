using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace HCM3.Services.Internal
{
    
    /// <summary>
    /// Contains information on how the DisplayInfo strings should be written to the screen.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct DisplayInfoArguments
    {
        public float ScreenX;
        public float ScreenY;
        public int SignificantDigits;
        public float FontSize;

    }
}
