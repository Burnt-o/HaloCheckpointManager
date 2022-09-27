

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

            // Run some checks; have admin priviledges, have file access, have required folders & files.
            if (!HCMSetupChecks(out string errorMessage))
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

            UIActionProperties bps = new(Command1, CommandCan);
            Button1.DataContext = bps;

            UIActionProperties bps2 = new(Command2, CommandCan);
            Button2.DataContext = bps2;


            //Each button could be bound to it's own CanExecute property. This property assess both whether we have valid pointers
            //  (for this game version) for all the things the button wants us to do, AND whether we're attached and in a valid gamestate.

            //Whenever gamestate changes, update CanExecute.
            //Whenever pointers are freshly loaded, update CanExecute (well, maybe we only load once at the start).
            //Every buttton call starts by updating the gamestate, which will update CanExecute for it.

        }
        #endregion // Constructor

        #region Properties
        private bool testme = true;

        private void Command1(object? parameter)
        {
            Trace.WriteLine("WOOOOOO");
        }

        private void Command2(object? parameter)
        {
            testme = false;
            Trace.WriteLine("aaaaaaaaarrrrrrrr");
        }

        private bool CommandCan(object? parameter)
        {
            return testme;
        }
        #endregion // Properties

        private void Window_Closed(object sender, EventArgs e)
        {
            Settings.Default.Save();
        }



    }



}
