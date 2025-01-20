using HCMExternal.ViewModels.Commands;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using NonInvasiveKeyboardHookLibrary;
using XInputium;
using XInputium.XInput;
using System.Runtime.CompilerServices;
using static HCMExternal.Services.Hotkeys.Impl.Hotkey;
using System.Windows;
using HCMExternal.Views;
using Serilog;
using System.Windows.Controls;
using System.Globalization;

namespace HCMExternal.Services.Hotkeys.Impl
{
    

    public class HotkeyManager : IHotkeyManager
    {
        (VirtualKey?, XInputButton?) getDefaultBinding(HotkeyEnum hotkeyEnum)
        {
            if (!defaultBindings.ContainsKey(hotkeyEnum))
                throw new Exception("Forgot to add default binding for hotkey..");
            return defaultBindings[hotkeyEnum];
        }

        (VirtualKey?, XInputButton?)? deserialiseBinding(HotkeyEnum hotkeyEnum)
        {
            Log.Verbose("deserialising binding for hotkey: " + hotkeyEnum.ToString());
            string settingName = "HK_" + hotkeyEnum.ToString();


            if (Properties.Settings.Default[settingName] == null)
            {
                Log.Error("Properties did not contain setting by name: " + settingName);
                return null;
            }
               

            string? serialisedBinding = Properties.Settings.Default[settingName].ToString();
            if (serialisedBinding == null && serialisedBinding != "")
            {
                Log.Error("Properties setting at name [" + settingName + "] was null.");
                return null;
            }

            Log.Information("Hotkey Setting string: " + serialisedBinding);

            if (serialisedBinding.Contains(',') == false)
            {
                Log.Error("Setting did not contain a comma");
                return null;
            }

            string[] serialisedBindingPair = serialisedBinding.Split(',');

            if (serialisedBindingPair.Length != 2)
            {
                Log.Error("split setting string was wrong size: " + serialisedBindingPair.Length);
                return null;
            }

            bool parsedVirtualKey = Enum.TryParse(serialisedBindingPair[0], out VirtualKey virtualKey);
            
            bool parsedGamepadButton = Enum.TryParse(serialisedBindingPair[1], out XButtons gamepadButtonButton);


            if (!parsedVirtualKey)
                Log.Error("Could not parse virtual key enum");

            if (!parsedGamepadButton)
                Log.Error("Could not parse game pad button enum");

            XInputButton? gamepadButton = null;
            try
            {
                if (parsedGamepadButton)
                    gamepadButton = new XInputButton(gamepadButtonButton, false);
            }
            catch (ArgumentException ex)
            {
                Log.Error("Failled to parse gamepadButton: " + ex.Message);
            }

            return ((parsedVirtualKey ? virtualKey : null), gamepadButton);

        }


        private Dictionary<HotkeyEnum, (HotkeyViewModel, Hotkey)> _hotkeyMap = new();

        public void OpenHotkeyRebindDialog(HotkeyEnum hotkeyEnum)
        {
            Log.Verbose("Opening new rebind dialog for hotkey: " + hotkeyEnum);

            //must occur on UI thread
            Application.Current.Dispatcher.Invoke((Action)delegate {

                HotkeyViewModel selectedHotkeyViewModel = _hotkeyMap[hotkeyEnum].Item1;
                Hotkey selectedHotkey = _hotkeyMap[hotkeyEnum].Item2;
                RebindHotkeyView rebindDialog = new((int?)selectedHotkey.VirtualKeyCode?.Item1, selectedHotkey.GamepadButton, this);

                if (rebindDialog.ShowDialog() == true) // user pressed ok instead of cancel
                {
                    // keyboard needs to unregister previous virtualKeyCode if there was one
                    if (selectedHotkey.VirtualKeyCode != null)
                        Keyboard.UnregisterHotkey(selectedHotkey.VirtualKeyCode.Value.Item2); // unregister by guid

                    // register new virtualKeyCode if there is one
                    if (rebindDialog.SelectedKey != null)
                    { 
                        Guid guid = Keyboard.RegisterHotkey((int)rebindDialog.SelectedKey, () => { selectedHotkeyViewModel.InvokeHotkeyTrigger(this); });
                        selectedHotkey.UpdateBindings(((VirtualKey)rebindDialog.SelectedKey, guid), rebindDialog.SelectedPad);
                    }
                     else
                        selectedHotkey.UpdateBindings(null, rebindDialog.SelectedPad);

                }
            });

        }

        public HotkeyViewModel RegisterHotkey(HotkeyEnum hotkeyEnum)
        {
            Log.Verbose("Registering hotkey for " + hotkeyEnum);

            var initialBinding = deserialiseBinding(hotkeyEnum) ?? getDefaultBinding(hotkeyEnum);
            
            Hotkey hotkey = new Hotkey(hotkeyEnum);
            HotkeyViewModel hotkeyViewModel = new(hotkey.ToString(), () => { OpenHotkeyRebindDialog(hotkeyEnum); }, hotkey as IHotkey);

            // register keyboard
            if (initialBinding.Item1 != null)
            {
                Guid guid = Keyboard.RegisterHotkey((int)initialBinding.Item1, () => { hotkeyViewModel.InvokeHotkeyTrigger(this); });
                hotkey.UpdateBindings((initialBinding.Item1.Value, guid), initialBinding.Item2);
            }
            else
                hotkey.UpdateBindings(null, initialBinding.Item2);


            _hotkeyMap.Add(hotkeyEnum, (hotkeyViewModel, hotkey));
            return hotkeyViewModel;
        }

        private bool _hotkeysEnabled;
        public bool HotkeysEnabled {
            get => _hotkeysEnabled;
            set
            {
                _hotkeysEnabled = value;
                if (value)
                    Keyboard.Start();
                else
                    Keyboard.Stop();
            }
        }



        private static readonly Dictionary<HotkeyEnum, (VirtualKey?, XInputButton?)> defaultBindings = new()
            {
                { HotkeyEnum.ForceCheckpoint, (VirtualKey.F1, null)},
                { HotkeyEnum.ForceRevert, (VirtualKey.F2, null)},
                { HotkeyEnum.DoubleRevert, (VirtualKey.F3, null)},
                { HotkeyEnum.DumpCheckpoint, (VirtualKey.F4, null)},
                { HotkeyEnum.InjectCheckpoint, (VirtualKey.F5, null)},
                { HotkeyEnum.DisableCheckpoints, (VirtualKey.F7, null)},
            };


        public HotkeyManager()
        {
            HotkeysEnabled = false;

            // gamepad listener
            Gamepad.Device = XInputium.XInput.XInputDevice.GetFirstConnectedDevice();
            Gamepad.ButtonPressed += Gamepad_ButtonPressed;

            // update gamepad state using a timer
            _gamepadUpdateTimer.Elapsed += (o, e) =>
            {
                Gamepad.Update();
            };

            // update which gamepad device to use whenever one is (un)plugged in/out.
            GamepadDeviceManager.DeviceStateChanged += (o, e) =>
            {
                Gamepad.Device = XInputium.XInput.XInputDevice.GetFirstConnectedDevice();
            };

        }


        private static readonly System.Timers.Timer _gamepadUpdateTimer = new System.Timers.Timer() { Interval = 30, Enabled = true };

        private void Gamepad_ButtonPressed(object? sender, DigitalButtonEventArgs<XInputButton> e)
        {
            if (HotkeysEnabled == false)
                return;

            if (e.Button.Duration > TimeSpan.Zero)  // don't refire on long press
                return; 

            foreach (var (hotkeyViewModel, hotkey) in _hotkeyMap.Values)
            {
                if (hotkey.GamepadButton != null && hotkey.GamepadButton == e.Button)
                {
                    hotkeyViewModel.InvokeHotkeyTrigger(this);
                }
            }
        }

        XInputium.XInput.XInputDeviceManager GamepadDeviceManager = new();
        public readonly XGamepad Gamepad = new();
        public readonly KeyboardHookManager Keyboard = new();

    }



}
