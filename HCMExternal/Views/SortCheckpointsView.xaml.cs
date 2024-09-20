using HCMExternal.Services.CheckpointServiceNS;
using HCMExternal.ViewModels;
using System;
using System.Windows;

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
            DataContext = vm;
            if (vm.CloseAction == null)
            {
                vm.CloseAction = new Action(Close);
            }
        }
    }
}
