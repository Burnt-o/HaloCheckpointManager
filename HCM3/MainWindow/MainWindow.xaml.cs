

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

            // Run some checks; have admin priviledges, have file access, have required folders & files.
            if (!HCMSetupChecks(out string errorMessage))
            {
                // If a check fails, tell the user why, then shutdown the application.
                System.Windows.MessageBox.Show(errorMessage, "Error", System.Windows.MessageBoxButton.OK);
                System.Windows.Application.Current.Shutdown();
            }



            // Initialize some properties
            // Starting with Offsets. 
                // You know what, let's just grab EVERY offset file on app start up. Hm. 
                // Well load it into our classes for every offset of every version that's online anyway.
                // Don't forget we may well be using Xero's database thingy.
                // In any case, we need a PointerCollection object, and a List<PointerCollection> (key is game version number).
                // ORRRR have a single PointerCollection object, consisting of many List<Pointer> (key is game version number).
                // We'll figure out binding of buttons to whether a pointer for that thing is valid or not later.

            //Each button could be bound to it's own CanExecute property. This property assess both whether we have valid pointers
            //  (for this game version) for all the things the button wants us to do, AND whether we're attached and in a valid gamestate.

            //Whenever gamestate changes, update CanExecute.
            //Whenever pointers are freshly loaded, update CanExecute (well, maybe we only load once at the start).
            //Every buttton call starts by updating the gamestate, which will update CanExecute for it.
            
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
