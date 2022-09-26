

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

namespace HCM3
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        #region Data
        #endregion // Data


        #region Constructor
        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;


            if (!InitializeHCM(out string errorMessage))
            {
                
                System.Windows.MessageBox.Show(errorMessage, "Error", System.Windows.MessageBoxButton.OK);
                System.Windows.Application.Current.Shutdown();
            }
            
        }
        #endregion // Constructor

        #region Properties



        #endregion // Properties

        private void Window_Closed(object sender, EventArgs e)
        {
            Settings.Default.Save();
        }


    }



}
