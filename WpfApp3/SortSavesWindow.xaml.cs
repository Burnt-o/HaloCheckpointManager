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

namespace WpfApp3
{
    /// <summary>
    /// Interaction logic for SortSavesWindow.xaml
    /// </summary>
    public partial class SortSavesWindow : Window
    {
        public SortSavesWindow(string savepath)
        {
            
            InitializeComponent();
            MainLabel.Content = "Sort Saves in " + savepath;
        }

        public string returnvalue1 { get; set; }

        private void CancelClick(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;

            Close();
        }

        private void SortClick(object sender, RoutedEventArgs e)
        {
            this.returnvalue1 = "Something";
            this.DialogResult = true;

            Close();
        }
    }
}
