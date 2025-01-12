using HCMExternal.Services.Hotkeys.Impl;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Threading;
using XInputium.XInput;
using HCMExternal.ViewModels.Interfaces;
using System.ComponentModel;
using HCMExternal.Services.Hotkeys;
using System.Runtime.CompilerServices;

namespace HCMExternal.Views
{
    /// <summary>
    /// Interaction logic for RebindHotkeyView.xaml
    /// </summary>
    public partial class RebindHotkeyView : Window, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler? PropertyChanged;
        protected void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private int? _selectedKey;
        public int? SelectedKey
        {
            get { return _selectedKey; }
            set { _selectedKey = value; }
        }

        private string? _selectedKeyText;
        public string? SelectedKeyText
        {
            get { return _selectedKeyText; }
            set
            {
                _selectedKeyText = value;
                if (value == null) _selectedKeyText = "Unbound";
                OnPropertyChanged(nameof(SelectedKeyText));
            }
        }

        private XInputButton? _selectedPad;
        public XInputButton? SelectedPad
        {
            get { return _selectedPad; }
            set { _selectedPad = value; }
        }

        private string? _selectedPadText;
        public string? SelectedPadText
        {
            get { return _selectedPadText; }
            set
            {
                _selectedPadText = value;
                if (value == null) _selectedPadText = "Unbound";
                OnPropertyChanged(nameof(SelectedPadText));
            }
        }


        private HotkeyManager _hotkeyManager { get; init; }


        public RebindHotkeyView(int? currentKBhotkey, XInputButton? currentGPhotkey, HotkeyManager? hotkeyManager)
        {
            this.Owner = App.Current.MainWindow;
            this.KeyUp += ChangeHotkeyView_KeyUp;
            this.SelectedKey = currentKBhotkey;
            this.SelectedPad = currentGPhotkey;
            this.SelectedKeyText = currentKBhotkey != null ? KeyInterop.KeyFromVirtualKey(this.SelectedKey.Value).ToString() : null;
            this.SelectedPadText = currentGPhotkey != null ? "[" + currentGPhotkey.ToString() + "]" : null;



            InitializeComponent();

            _hotkeyManager = hotkeyManager;
            _hotkeyManager.Gamepad.ButtonPressed += Gamepad_ButtonPressed;
        }


        private void Gamepad_ButtonPressed(object? sender, XInputium.DigitalButtonEventArgs<XInputium.XInput.XInputButton> e)
        {
            //run at high priority to prevent lag
            Application.Current.Dispatcher.BeginInvoke(
                DispatcherPriority.Input,
                new Action(() => 
                {
                    SelectedPad = e.Button;
                    this.SelectedPadText = "[" + SelectedPad.ToString() + "]";
                })
            );

        }

        private void ChangeHotkeyView_KeyUp(object sender, KeyEventArgs e)
        {
            var virtualKeyCode = KeyInterop.VirtualKeyFromKey(e.Key);
            //Trace.WriteLine("READ KEYYYYY: " + virtualKeyCode);
            //TODO check for being invalid key
            SelectedKey = virtualKeyCode;

            this.SelectedKeyText = KeyInterop.KeyFromVirtualKey(this.SelectedKey.Value).ToString();
        }

        private void okButton_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
            _hotkeyManager.Gamepad.ButtonPressed -= Gamepad_ButtonPressed;
            this.Close();
        }

        private void clearKB_Click(object sender, RoutedEventArgs e)
        {

            SelectedKey = null;
            SelectedKeyText = null;
        }

        private void clearGP_Click(object sender, RoutedEventArgs e)
        {

            SelectedPad = null;
            SelectedPadText = null;
        }

        private void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;
            _hotkeyManager.Gamepad.ButtonPressed -= Gamepad_ButtonPressed;
            this.Close();

        }

        private void Window_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left)
                this.DragMove();
        }
    }
}
