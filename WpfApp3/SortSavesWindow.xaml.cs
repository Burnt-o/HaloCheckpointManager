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
        (ComboBox Key, CheckBox Reverse)[] SortFields;

        public SortSavesWindow(string savepath, ref HCMConfig config)
        {
            
            InitializeComponent();
            SortFields = new[] { (Sort1, Reverse1), (Sort2, Reverse2), (Sort3, Reverse3), (Sort4, Reverse4) };

            MainLabel.Content = "Sort Saves in " + savepath;
            string[] sortoptions = { "None", "Difficulty", "Level Name", "File Name", "Time into Level" };
            foreach (var (field, i) in SortFields.Select((x, i) => (x, i)))
            {
                foreach (string option in sortoptions)
                    field.Key.Items.Add(option);
                field.Key.SelectedIndex = config.Sort[i].Item1;
                field.Reverse.IsChecked = config.Sort[i].Item2;
            }
            Sort5.Items.Add("Previous position");
            Sort5.SelectedIndex = 0;
            Reverse5.IsChecked = config.Sort.ReversePreviousPosition;
        }

        public (int, bool)[] Criteria()
        {
            return SortFields.Select((x, i) => (x.Key.SelectedIndex, x.Reverse.IsChecked.GetValueOrDefault())).ToArray();
        }

        public bool ReversePreviousPosition()
        {
            return Reverse5.IsChecked.GetValueOrDefault();
        }

        private void CancelClick(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;

            Close();
        }

        private void SortClick(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;

            Close();
        }
    }
}
