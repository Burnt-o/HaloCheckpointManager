using HCMExternal.ViewModels;
using Serilog;
using System;
using System.Windows;
using System.Windows.Controls;

namespace HCMExternal.Views.Controls
{
    /// <summary>
    /// Interaction logic for UserControl1.xaml
    /// </summary>
    public partial class StatusBarView : UserControl
    {
        public StatusBarView()
        {
            InitializeComponent();
            DataContextChanged += StatusBarView_DataContextChanged;


        }

        private void StatusBarView_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            MCCHookStateViewModel? vm = DataContext as MCCHookStateViewModel;
            if (vm != null)
            {
                Log.Information("Subscribing to vm.mErrorDialogViewModel.ShowErrorDialogAndAskRetryEvent");
                vm.mErrorDialogViewModel.ShowErrorDialogAndAskRetryEvent += Vm_ShowErrorDialogAndAskRetryEvent;
            }
            else
            {
                Log.Error("null MCCHookStateViewModel vm");
            }
        }

        private bool Vm_ShowErrorDialogAndAskRetryEvent(object sender, EventArgs args)
        {
            MCCHookStateViewModel? vm = DataContext as MCCHookStateViewModel;
            if (vm != null)
            {
                ErrorDialogView dialogWindow = new ErrorDialogView
                {
                    DataContext = vm.mErrorDialogViewModel,
                    Owner = Application.Current.MainWindow
                };
                return dialogWindow.ShowDialog() ?? false;
            }
            else
            {
                Log.Error("null MCCHookStateViewModel vm");
                return false;
            }

        }


    }
}
