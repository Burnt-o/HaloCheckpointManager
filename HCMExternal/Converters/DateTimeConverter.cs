using System;
using System.Globalization;
using System.Windows.Data;
namespace HCMExternal.Converters
{
    public class DateTimeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            DateTime? valueDateTime = (DateTime?)value;
            if (valueDateTime == null)
            {
                return "???";
            }

            DateTime valueDateTimeNotNull = (DateTime)valueDateTime;
            string date = valueDateTimeNotNull.ToString("d");
            string time = valueDateTimeNotNull.ToString("HH:mm:ss");

            return date + " " + time;


        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
