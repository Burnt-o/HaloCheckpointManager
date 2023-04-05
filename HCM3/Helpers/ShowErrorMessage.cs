using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Windows;
using System.Threading;
using HCM3.Views;

namespace HCM3.Helpers
{
    public static class ErrorMessage
    {
        public static void Show(string message)
        {
            Show(message, "HaloCheckpointManager Error");
        }

        public static void Show(string message, string caption)
        {
            System.Windows.Application.Current.Dispatcher.Invoke((Action)delegate {
                //TODO; instead of using a messagebox, create a custom form so we can force it on top
                Window mainWindow = Application.Current.MainWindow;
                MessageBox.Show(mainWindow, message, caption, MessageBoxButton.OK, MessageBoxImage.Error);
            });

        }
    }
}
