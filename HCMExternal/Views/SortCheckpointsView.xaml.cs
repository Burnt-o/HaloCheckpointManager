using HCMExternal.Services;
using HCMExternal.Services.CheckpointIO;
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
        internal SortCheckpointsView(ICheckpointIOService cpio, FileViewModel fileViewModel)
        {
            InitializeComponent();
            SortCheckpointsViewModel vm = new SortCheckpointsViewModel(cpio, fileViewModel);
            DataContext = vm;
            if (vm.CloseAction == null)
            {
                vm.CloseAction = new Action(Close);
            }
        }
    }
}
