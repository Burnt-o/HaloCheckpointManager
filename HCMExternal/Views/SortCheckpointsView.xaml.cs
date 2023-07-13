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
using HCMExternal.ViewModels;
using HCMExternal.Services.CheckpointServiceNS;

namespace HCMExternal.Views
{
    /// <summary>
    /// Interaction logic for SortCheckpointsView.xaml
    /// </summary>
    public partial class SortCheckpointsView : Window
    {
        internal SortCheckpointsView(CheckpointService CheckpointServices, CheckpointViewModel CheckpointViewModel)
        {
            InitializeComponent();
            SortCheckpointsViewModel vm = new SortCheckpointsViewModel(CheckpointServices, CheckpointViewModel);
            this.DataContext = vm;
            if (vm.CloseAction == null)
                vm.CloseAction = new Action(this.Close);
        }
    }
}
