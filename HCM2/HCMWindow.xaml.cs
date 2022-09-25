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

        #endregion //Data

        #region Contructor
        public HCMWindow()
        {
            


            
            GameSaveCollection testSaveCollection = new GameSaveCollection();
            testSaveCollection.Add(new GameSave("My First Checkpoint", DateTime.Now, "a10", 3, TimeSpan.FromSeconds(6), DateTime.Now));
            _controller = new HCMWindowController(this, testSaveCollection);

            GameSaveFolder folder1 = new GameSaveFolder("folder1", testSaveCollection);

            Folders = new List<GameSaveFolder>();
            Folders.Add(folder1);


            // Use the list of Xray objects as 
            // this Window's data source.

            //base.DataContext = testCheckpoints;
            this.DataContext = this;
            InitializeComponent();

        }
        #endregion //Constructor

        public List<GameSaveFolder> Folders { get; set; }

        private void RunTestCode()
        {

            
            List<GameSaveFolder> folders = new List<GameSaveFolder>();
            GameSaveCollection testCollection1 = new GameSaveCollection();
            testCollection1.Add(new GameSave("My First Checkpoint", DateTime.Now, "a10", 3, TimeSpan.FromSeconds(6), DateTime.Now));

            GameSaveCollection testCollection2 = new GameSaveCollection();

            GameSaveFolder folder1 = new GameSaveFolder("folder1", testCollection1);
            GameSaveFolder folder2 = new GameSaveFolder("folder2", testCollection2);

            folders.Add(folder1);
            folders.Add(folder2);
            

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
