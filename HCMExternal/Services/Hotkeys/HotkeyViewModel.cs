using HCMExternal.ViewModels.Commands;
using HCMExternal.ViewModels.Interfaces;
using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace HCMExternal.Services.Hotkeys
{
    public class HotkeyViewModel : Presenter
    {
        private string _bindingText;
        public string BindingText
        {
            get => _bindingText;
            set
            {
                _bindingText = value;
                Log.Verbose("should be: " + value);
                OnPropertyChanged(nameof(BindingText)); // update ui
            }
        }

        public ICommand OpenHotkeyWindowCommand { get; init; }

        public event EventHandler HotkeyTriggered = new EventHandler((o, e) => { });
        public void InvokeHotkeyTrigger(object? sender)
        {
            HotkeyTriggered(sender, EventArgs.Empty);
        }

        public HotkeyViewModel(string bindingText, Action onOpenHotkey, IHotkey ihotkey)
        {
            BindingText = bindingText;
            OpenHotkeyWindowCommand = new RelayCommand((o) => { onOpenHotkey(); });

            ihotkey.BindingChangeEvent += (o, e) => { BindingText = e.BindingString; };
        }

        // HotkeyManager will keep a reference to all HotkeyViewModels and update binding text,
        // fire HotkeyTriggered.
    }
}
