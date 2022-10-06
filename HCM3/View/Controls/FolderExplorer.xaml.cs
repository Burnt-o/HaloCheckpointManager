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
using HCM3.ViewModel;

namespace HCM3.View.Controls
{
    /// <summary>
    /// Interaction logic for FolderExplorer.xaml
    /// </summary>
    public partial class FolderExplorer : UserControl
    {
        public FolderExplorer()
        {
            InitializeComponent();
        }

        private void TreeView_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            CheckpointViewModel vm = this.DataContext as CheckpointViewModel;
            vm.FolderChanged(sender, e);
        }
    }
}
