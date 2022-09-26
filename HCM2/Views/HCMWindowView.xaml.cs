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
using System.Windows.Navigation;
using System.Windows.Shapes;
using HCM.Model;
using System.Collections.ObjectModel;
using System.Collections;
using System.Diagnostics;

namespace HCM.Views
{
    /// <summary>
    /// Interaction logic for HCMWindowView.xaml
    /// </summary>
    public partial class HCMWindowView : UserControl
    {
        public HCMWindowView()
        {
            InitializeComponent();
            HCMWindow window = Window.GetWindow(App.Current.MainWindow) as HCMWindow;
            if (window != null)
            {
                Trace.WriteLine("winodw wasn't null");
                testTreeView.ItemsSource = window.GameSaveManager.TreeItems;
            }
            else
            {
                Trace.WriteLine("christ kill me");
            }
           


        }

    }
}
