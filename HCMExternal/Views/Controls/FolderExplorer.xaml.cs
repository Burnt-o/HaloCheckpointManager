using HCMExternal.ViewModels;
using System.Windows;
using System.Windows.Controls;

namespace HCMExternal.Views.Controls
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
            FileViewModel vm = DataContext as FileViewModel;
            vm.TreeFolderChanged(sender, e);
        }
    }
}
