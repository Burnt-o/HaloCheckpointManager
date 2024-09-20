using System.Diagnostics;
using System.Windows;
using System.Windows.Navigation;

namespace HCMExternal.Views
{
    /// <summary>
    /// Interaction logic for ErrorDialogView.xaml
    /// </summary>
    public partial class ErrorDialogView : Window
    {
        public ErrorDialogView()
        {
            InitializeComponent();
        }




        private void okButton_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = true;
        }



        private void cancelButton_Click(object sender, RoutedEventArgs e)
        {

            DialogResult = false;
        }

        private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
        {
            System.Diagnostics.Process.Start(new ProcessStartInfo
            {
                FileName = e.Uri.AbsoluteUri,
                UseShellExecute = true
            });
            e.Handled = true;
        }
    }
}
