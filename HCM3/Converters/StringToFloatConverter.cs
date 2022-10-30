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
    public class StringToFloatConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null) return "error null";

            if (value.GetType() != typeof(float)) return "error wrong type";
            
            float floatValue = (float)value;
            string floatString = floatValue.ToString();
            return floatString;

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                string inputString = value as string;

                float inputFloat = float.Parse(inputString);

                return inputFloat;
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Invalid data! \n" + ex.Message, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
                return null;
            }
        }
    }
}
