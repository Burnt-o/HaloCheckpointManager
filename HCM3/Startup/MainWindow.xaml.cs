

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
            
       HCMSetup setup = new();


            // Run some checks; have admin priviledges, have file access, have required folders & files.
            if (!setup.HCMSetupChecks(out string errorMessage))
            {
                // If a check fails, tell the user why, then shutdown the application.
                System.Windows.MessageBox.Show(errorMessage, "Error", System.Windows.MessageBoxButton.OK);
                System.Windows.Application.Current.Shutdown();
            }

            // Create collection of all our ReadWrite.Pointers and load them from the online repository
            PointerCollection pcollection = new();
            if (!pcollection.LoadPointersFromGit(out string error))
            {
                System.Windows.MessageBox.Show(error, "Error", System.Windows.MessageBoxButton.OK);
                System.Windows.Application.Current.Shutdown();
            }

            HaloMemory HaloMemory = new HaloMemory();
            InitializeComponent();



            MainModel mainModel = new(pcollection, HaloMemory);
            MainViewModel mainViewModel = new(mainModel);

            this.DataContext = mainViewModel;
            

            //checkpointsH1.Add(new Checkpoint());

        }
        #endregion // Constructor

        #region Properties

        #endregion
        private void Window_Closed(object sender, EventArgs e)
        {
            Properties.Settings.Default.Save();
        }



    }

   



   

}
