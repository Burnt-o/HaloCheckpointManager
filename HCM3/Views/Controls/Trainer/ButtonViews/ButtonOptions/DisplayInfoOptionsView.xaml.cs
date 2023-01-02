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
    public partial class DisplayInfoOptionsView : Window
    {
        public DisplayInfoOptionsView()
        {
            InitializeComponent();
        }

        private void okButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {

               BindingExpression fontsize = fontSize.GetBindingExpression(TextBox.TextProperty);
               fontsize.UpdateSource();
                BindingExpression screenx = screenX.GetBindingExpression(TextBox.TextProperty);
                screenx.UpdateSource();
                BindingExpression screeny = screenY.GetBindingExpression(TextBox.TextProperty);
                screeny.UpdateSource();
                BindingExpression sigdig = sigDig.GetBindingExpression(TextBox.TextProperty);
                sigdig.UpdateSource();



                this.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("One of your inputs was invalid! Error: " + ex.ToString());
            }
            
        }
    }
}
