using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.Services.Hotkeys
{
    public class BindingChangedEventArgs : EventArgs
    {
        public string BindingString { get; init; }
        public BindingChangedEventArgs(string bindingString)
        { 
            BindingString = bindingString; 
        }
    }

    public interface IHotkey
    {

        // subcribe to this to get events whenver VirtualKeyCode or GamepadButton changes.
        public event EventHandler<BindingChangedEventArgs> BindingChangeEvent;

        public HotkeyEnum HotkeyEnum { get; init; }
    }
}
