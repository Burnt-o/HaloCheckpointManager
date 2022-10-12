

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
using System.Diagnostics;
using BurntMemory;
using HCM3.View;
using System.Collections.ObjectModel;
using HCM3.ViewModel;
using HCM3.Model;


namespace HCM3.Startup
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



        }
        #endregion // Constructor

        #region Properties

        #endregion
        private void Window_Closed(object sender, EventArgs e)
        { 
            Properties.Settings.Default.Save();
            //mainModel.HaloMemory.SpeedhackManager.RemoveSpeedHack(sender, e);
            //mainModel.HaloMemory.DebugManager.GracefullyCloseDebugger(sender, e);
        }

    }

   



   

}
