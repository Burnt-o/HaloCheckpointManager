using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System;
using System.Globalization;
using System.Windows.Controls;
using System.Windows.Data;
using System.Diagnostics;

namespace HCM3.Converters
{
    public class IGTConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
            {
                Trace.WriteLine("null value for IGT converter");
                return "Error";
            }

            int gameTicks = (int)value; // Should be stored in the 
            if (gameTicks <= 0) // To avoid dividing 0 later. Should never be negative unless there was an error reading the checkpoint data.
            { 
            return TimeSpan.Zero;
            }

            //TODO; change this to reference tabindex directorly (this is part of the view/viewmodel after all
            string game = HCM3.Properties.Settings.Default.SelectedGame;
            double divisor = (game == "H1") ? (double)30 : (double)60;
            double seconds = gameTicks / divisor;

            return TimeSpan.FromSeconds(seconds);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
