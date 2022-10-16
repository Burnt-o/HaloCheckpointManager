using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using System.Windows.Input;
using HCM3.ViewModels.Commands;

namespace HCM3.ViewModels
{
    public interface IControlWithHotkey
    {
        public string HotkeyText { get; set; }

    }
}