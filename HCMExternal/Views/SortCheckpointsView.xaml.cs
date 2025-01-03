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
        internal SortCheckpointsView(ICheckpointIOService cpservice, CheckpointViewModel CheckpointViewModel)
        {
            InitializeComponent();
            SortCheckpointsViewModel vm = new SortCheckpointsViewModel(cpservice, CheckpointViewModel);
            DataContext = vm;
            if (vm.CloseAction == null)
            {
                vm.CloseAction = new Action(Close);
            }
        }
    }
}
