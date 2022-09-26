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
using HCM.Controllers;
using HCM.Model;

namespace HCM.Views
{
    /// <summary>
    /// The main Window of HCM
    /// </summary>
    public partial class HCMWindow : Window
    {
        #region Data

        readonly HCMWindowController _controller;
        readonly GameSaveManager _gameSaveManager;
        #endregion //Data

        #region Contructor
        public HCMWindow()
        {
            

            _gameSaveManager = new GameSaveManager();
            
            _controller = new HCMWindowController(this, GameSaveManager.TreeItems);



            // Use the list of Xray objects as 
            // this Window's data source.

            //base.DataContext = testCheckpoints;
            this.DataContext = this;
            InitializeComponent();

        }
        #endregion //Constructor

        public GameSaveManager GameSaveManager
        {
            get
            { 
                return _gameSaveManager;
            }
        }



        #region Command Sinks

        // These methods handle events of  
        // the RenameCheckpoint command.

        void RenameItem_CanExecute(
            object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = _controller.IsACheckpointSelected;
        }

        void RenameItem_Executed(
            object sender, ExecutedRoutedEventArgs e)
        {
            _controller.RenameSelectedItem("hi mark");
        }

        #endregion // Command Sinks

    }
}
