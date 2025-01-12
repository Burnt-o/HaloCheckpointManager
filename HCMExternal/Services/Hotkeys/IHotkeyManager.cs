using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.Services.Hotkeys
{
    public interface IHotkeyManager
    {
        public HotkeyViewModel RegisterHotkey(HotkeyEnum hotkeyEnum);
        public bool HotkeysEnabled { get; set; }
    }
}
