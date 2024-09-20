using System;
using System.Windows.Data;

namespace HCMExternal.Converters
{
    public class InverseBooleanConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter,
            System.Globalization.CultureInfo culture)
        {

            if (targetType == typeof(bool?))
            { return !(bool?)value; }

            if (targetType == typeof(bool))
            { return !(bool)value; }


            //throw new InvalidOperationException("The target must be a boolean");
            return false;

        }

        public object ConvertBack(object value, Type targetType, object parameter,
            System.Globalization.CultureInfo culture)
        {
            return Convert(value, targetType, parameter, culture);
        }
    }
}
