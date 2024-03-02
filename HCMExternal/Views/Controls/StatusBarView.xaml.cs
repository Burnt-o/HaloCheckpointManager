using HCMExternal.ViewModels;
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
using Serilog;

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
            var vm = DataContext as MCCHookStateViewModel;
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
            var vm = DataContext as MCCHookStateViewModel;
            if ( vm != null )
            {
                var dialogWindow = new ErrorDialogView();
                dialogWindow.DataContext = vm.mErrorDialogViewModel;
                dialogWindow.Owner = Application.Current.MainWindow;
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
