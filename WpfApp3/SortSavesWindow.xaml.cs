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
            string[] sortoptions = { "None", "Difficulty", "Level Order", "Alphabetically", "Time into Level" };
            foreach (string option in sortoptions)
            {
                Sort1.Items.Add(option);
                Sort2.Items.Add(option);
                Sort3.Items.Add(option);
                Sort4.Items.Add(option);
            }
            Sort5.Items.Add("Previous position");
            Sort1.SelectedIndex = 0;
            Sort2.SelectedIndex = 0;
            Sort3.SelectedIndex = 0;
            Sort4.SelectedIndex = 0;
            Sort5.SelectedIndex = 0;
        }

        public int ReturnSort1 { get; set; }
        public bool ReturnReverse1 { get; set; }
        public int ReturnSort2 { get; set; }
        public bool ReturnReverse2 { get; set; }
        public int ReturnSort3 { get; set; }
        public bool ReturnReverse3 { get; set; }
        public int ReturnSort4 { get; set; }
        public bool ReturnReverse4 { get; set; }
        public bool ReturnReverse5 { get; set; }

        private void CancelClick(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;

            Close();
        }

        private void SortClick(object sender, RoutedEventArgs e)
        {
            this.ReturnSort1 = Sort1.SelectedIndex;
            this.ReturnReverse1 = Reverse1.IsChecked.GetValueOrDefault();
            this.ReturnSort2 = Sort2.SelectedIndex;
            this.ReturnReverse2 = Reverse2.IsChecked.GetValueOrDefault();
            this.ReturnSort3 = Sort3.SelectedIndex;
            this.ReturnReverse3 = Reverse3.IsChecked.GetValueOrDefault();
            this.ReturnSort4 = Sort4.SelectedIndex;
            this.ReturnReverse4 = Reverse4.IsChecked.GetValueOrDefault();
            this.ReturnReverse5 = Reverse5.IsChecked.GetValueOrDefault();
            this.DialogResult = true;

            Close();
        }
    }
}
