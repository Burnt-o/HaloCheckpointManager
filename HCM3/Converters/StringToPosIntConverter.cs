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
    public class StringToPosIntConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null) return "error null";

            if (value.GetType() != typeof(int)) return "error wrong type";

            int intValue = (int)value;
            string intString = intValue.ToString();
            return intString;

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                string inputString = value as string;

                int inputInt = int.Parse(inputString);

                if (inputInt < 0) throw new Exception("Input can't be less than zero");

                return inputInt;
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Invalid data! \n" + ex.ToString(), "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
                return null;
            }
        }
    }
}
