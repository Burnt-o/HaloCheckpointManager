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

namespace HCM3.Views.Controls.Trainer.ButtonViews.ButtonOptions
{
    /// <summary>
    /// Interaction logic for TeleportOptionsView.xaml
    /// </summary>
    public partial class TeleportOptionsView : Window
    {
        public TeleportOptionsView()
        {
            InitializeComponent();
        }

        //private void okButton_Click(object sender, RoutedEventArgs e)
        //{
        //    try
        //    {
        //        BindingExpression be0 = RadioTeleportForward.GetBindingExpression(RadioButton.IsCheckedProperty);
        //        be0.UpdateSource();
        //        if (RadioTeleportForward.IsChecked == true)
        //        {
        //            BindingExpression beLength = teleportLength.GetBindingExpression(TextBox.TextProperty);
        //            beLength.UpdateSource();

        //            BindingExpression beIgnoreZ = teleportIgnoreZ.GetBindingExpression(CheckBox.IsCheckedProperty);
        //            beIgnoreZ.UpdateSource();
        //        }
        //        else
        //        {
        //            BindingExpression beX = teleportX.GetBindingExpression(TextBox.TextProperty);
        //            beX.UpdateSource();


        //            BindingExpression beY = teleportY.GetBindingExpression(TextBox.TextProperty);
        //            beY.UpdateSource();

        //            BindingExpression beZ = teleportZ.GetBindingExpression(TextBox.TextProperty);
        //            beZ.UpdateSource();
        //        }
        //        this.Close();
        //    }
        //    catch
        //    { 
        //    }

        //}

        private void CloseOptionsWindow(object sender, RoutedEventArgs e)
        {
            this.Close();
        }


    }
}
