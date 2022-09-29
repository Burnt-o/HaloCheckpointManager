

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
using HCM3.Commands;
using HCM3.TestViews;
using System.Collections.ObjectModel;

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

            HCMTasks tasks = new();

            //Setup command object that will be datacontext for binding of HCM's commands. They need the pointer collection and and a reference to MainWindow
            //HCM3.Commands.Commands CommandObject = new(pcollection, tasks);
            //Button1.DataContext = CommandObject;


            ObservableCollection<Checkpoint> custdata = new();
            custdata.Add(new Checkpoint("ah", "ee"));
            custdata.Add(new Checkpoint("ah", "essfe"));
            custdata.Add(new Checkpoint("sadfasdfh", "ee"));
            custdata.Add(new Checkpoint("ah", "esse"));
            custdata.Add(new Checkpoint("asfsfh", "ee"));
            DataContext = custdata;
    

           



        }
        #endregion // Constructor

        #region Properties

        #endregion
        private void Window_Closed(object sender, EventArgs e)
        {
            Settings.Default.Save();
        }



    }

    public class Checkpoint
    {
        public Checkpoint(string a, string b, string c = "Easy", string d = "00:01:28", string e = "22/09/03", string f = "2645")
        { 
            Name = a;
            Level = b;
            Diff = c;
            IGT = d;
            Created = e;
            Version = f;

        }
        public int MinWidth = 10;
        public string Name { get; set; }
        public string Level { get; set; }
        public string Diff { get; set; }
        public string IGT { get; set; }
        public string Created { get; set; }
        public string Version { get; set; }

    }



    //yoink https://stackoverflow.com/a/61475351
    public static class GridColumn
    {
        public static readonly DependencyProperty MinWidthProperty =
            DependencyProperty.RegisterAttached("MinWidth", typeof(double), typeof(GridColumn), new PropertyMetadata(75d, (s, e) => {
                if (s is GridViewColumn gridColumn)
                {
                    SetMinWidth(gridColumn);
                    ((System.ComponentModel.INotifyPropertyChanged)gridColumn).PropertyChanged += (cs, ce) => {
                        if (ce.PropertyName == nameof(GridViewColumn.ActualWidth))
                        {
                            SetMinWidth(gridColumn);
                        }
                    };
                }
            }));

        private static void SetMinWidth(GridViewColumn column)
        {
            double minWidth = (double)column.GetValue(MinWidthProperty);

            if (column.Width < minWidth)
                column.Width = minWidth;
        }

        public static double GetMinWidth(DependencyObject obj) => (double)obj.GetValue(MinWidthProperty);

        public static void SetMinWidth(DependencyObject obj, double value) => obj.SetValue(MinWidthProperty, value);
    }

}
