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
using System.Diagnostics;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using HCM3.Services;
using XInputium.XInput;
using System.Windows.Threading;

namespace HCM3.Views.Controls.Trainer.ButtonViews.ButtonOptions
{
    /// <summary>
    /// Interaction logic for TeleportOptionsView.xaml
    /// </summary>
    public partial class ChangeHotkeyView : Window, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler? PropertyChanged;

        protected void OnPropertyChanged([CallerMemberName] string? name = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
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
            set { _selectedKeyText = value;
                if (value == null) _selectedKeyText = "Unbound";
                OnPropertyChanged(nameof(SelectedKeyText)); }
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
            set { _selectedPadText = value;
                if (value == null) _selectedPadText = "Unbound";
                OnPropertyChanged(nameof(SelectedPadText)); }
        }

        public string NameOfBinding { get; init; }

        HotkeyManager HotkeyManager { get; init; }


        public ChangeHotkeyView(int? currentKBhotkey, XInputButton? currentGPhotkey, HotkeyManager? hotkeyManager, string nameOfBinding)
        {
            this.Owner = App.Current.MainWindow;
            this.KeyUp += ChangeHotkeyView_KeyUp;
            this.SelectedKey = currentKBhotkey;
            this.SelectedPad = currentGPhotkey;
            this.SelectedKeyText = currentKBhotkey != null ? KeyInterop.KeyFromVirtualKey(this.SelectedKey.Value).ToString() : null;
            this.SelectedPadText = currentGPhotkey != null ? "[" + currentGPhotkey.ToString() + "]": null;
            this.NameOfBinding = "Edit hotkeys for " + nameOfBinding;



            InitializeComponent();

            HotkeyManager = hotkeyManager;
            HotkeyManager.gamepad.ButtonPressed += Gamepad_ButtonPressed;
        }


        private void Gamepad_ButtonPressed(object? sender, XInputium.DigitalButtonEventArgs<XInputium.XInput.XInputButton> e)
        {
            //run at high priority to prevent lag
            Application.Current.Dispatcher.BeginInvoke(
  DispatcherPriority.Input,
  new Action(() => {

      SelectedPad = e.Button;
      this.SelectedPadText = "[" + SelectedPad.ToString() + "]";

  }));



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
            HotkeyManager.gamepad.ButtonPressed -= Gamepad_ButtonPressed;
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
            HotkeyManager.gamepad.ButtonPressed -= Gamepad_ButtonPressed;
            this.Close();

        }

        private void Window_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left)
                this.DragMove();
        }


    }
}
