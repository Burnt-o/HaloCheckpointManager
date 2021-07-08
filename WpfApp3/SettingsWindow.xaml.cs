using System;
using System.Windows;
using System.Windows.Input;
using HCMHotkeys;

namespace WpfApp3
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class SettingsWindow : Window
    {
        public MainWindow.HCMConfig Config = MainWindow.HCMGlobal.SavedConfig;
        public KeyboardHook CPHotkeyHook = MainWindow.CPHotkeyHook;
        public KeyboardHook RevertHotkeyHook = MainWindow.RevertHotkeyHook;
        public KeyboardHook DoubleRevertHotkeyHook = MainWindow.DoubleRevertHotkeyHook;

        public SettingsWindow()
        {
            InitializeComponent();
            CPHotkey.Content = Config.CPHotkey.ToString() ?? "$lt;No Key>";
            RevertHotkey.Content = Config.RevertHotkey.ToString() ?? "$lt;No Key>";
            DoubleRevertHotkey.Content = Config.DoubleRevertHotkey.ToString() ?? "$lt;No Key>";
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        
        System.Windows.Controls.Button currentButton;

        void SetCPHotkey(object sender, System.Windows.Input.KeyEventArgs e)
        {
            CPHotkeyHook.Dispose();
            if (e.Key == Key.Escape) {
                Config.CPHotkey = Key.None;
                currentButton.Content = Config.CPHotkey.ToString() ?? "&lt;No Key>";
                currentButton = null;
                KeyDown -= SetCPHotkey;
                return;
            }
            
            Config.CPHotkey = e.Key;
            currentButton.Content = e.Key.ToString();
            currentButton = null;
            KeyDown -= SetCPHotkey;

            CPHotkeyHook.Dispose();

            CPHotkeyHook = new KeyboardHook();
            CPHotkeyHook.KeyPressed += new EventHandler<KeyPressedEventArgs>(MainWindow.ForceCP);
            CPHotkeyHook.RegisterHotKey(HCMHotkeys.ModifierKeys.None, (System.Windows.Forms.Keys)KeyInterop.VirtualKeyFromKey(e.Key));
            return;
        }

        void SetRevertHotkey(object sender, System.Windows.Input.KeyEventArgs e)
        {
            RevertHotkeyHook.Dispose();
            if (e.Key == Key.Escape)
            {
                Config.RevertHotkey = Key.None;
                currentButton.Content = Config.RevertHotkey.ToString() ?? "&lt;No Key>";
                currentButton = null;
                KeyDown -= SetRevertHotkey;
                return;
            }

            Config.RevertHotkey = e.Key;
            currentButton.Content = e.Key.ToString();
            currentButton = null;
            KeyDown -= SetRevertHotkey;

            RevertHotkeyHook.Dispose();

            RevertHotkeyHook = new KeyboardHook();
            RevertHotkeyHook.KeyPressed += new EventHandler<KeyPressedEventArgs>(MainWindow.ForceRevert);
            RevertHotkeyHook.RegisterHotKey(HCMHotkeys.ModifierKeys.None, (System.Windows.Forms.Keys)KeyInterop.VirtualKeyFromKey(e.Key));
            return;
        }

        void SetDoubleRevertHotkey(object sender, System.Windows.Input.KeyEventArgs e)
        {
            DoubleRevertHotkeyHook.Dispose();
            if (e.Key == Key.Escape)
            {
                Config.DoubleRevertHotkey = Key.None;
                currentButton.Content = Config.DoubleRevertHotkey.ToString() ?? "&lt;No Key>";
                currentButton = null;
                KeyDown -= SetDoubleRevertHotkey;
                return;
            }

            Config.DoubleRevertHotkey = e.Key;
            currentButton.Content = e.Key.ToString();
            currentButton = null;
            KeyDown -= SetDoubleRevertHotkey;

            DoubleRevertHotkeyHook.Dispose();

            DoubleRevertHotkeyHook = new KeyboardHook();
            DoubleRevertHotkeyHook.KeyPressed += new EventHandler<KeyPressedEventArgs>(MainWindow.ForceDoubleRevert);
            DoubleRevertHotkeyHook.RegisterHotKey(HCMHotkeys.ModifierKeys.None, (System.Windows.Forms.Keys)KeyInterop.VirtualKeyFromKey(e.Key));
            return;
        }

        private void CPHotkey_Click(object sender, RoutedEventArgs e)
        {
            CPHotkey.Content = "...";
            currentButton = (System.Windows.Controls.Button)sender;
            KeyDown += SetCPHotkey;
        }

        private void RevertHotkey_Click (object sender, RoutedEventArgs e)
        {
            RevertHotkey.Content = "...";
            currentButton = (System.Windows.Controls.Button)sender;
            KeyDown += SetRevertHotkey;
        }

        private void DoubleRevertHotkey_Click (object sender, RoutedEventArgs e)
        {
            DoubleRevertHotkey.Content = "...";
            currentButton = (System.Windows.Controls.Button)sender;
            KeyDown += SetDoubleRevertHotkey;
        }
    }
}
