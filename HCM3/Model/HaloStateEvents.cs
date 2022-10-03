using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;

namespace HCM3.Model
{
    public class HaloStateEvents : BurntMemory.Events
    {
        public class HaloStateChangedEventArgs : EventArgs
        {
            public HaloStateChangedEventArgs(int newHaloState)
            {
                NewHaloState = newHaloState;
            }
            public int NewHaloState { get; init; }
        }



        public static event EventHandler<HaloStateChangedEventArgs>? HALOSTATECHANGED_EVENT;

        public static void HALOSTATECHANGED_EVENT_INVOKE(object? sender, HaloStateChangedEventArgs e)
        {
            EventHandler<HaloStateChangedEventArgs>? handler = HALOSTATECHANGED_EVENT;
            if (handler != null)
            {
                handler(sender, e);
            }
        }
    }
}
