using System;
using System.Globalization;
using System.Windows.Data;

namespace HCM.Views
{
    /// <summary>
    /// This is a value converter that formats the raw 
    /// CreationDate of an Xray to formatted display text, 
    /// containing only the date portion of the value. 
    /// This is an example of the View formatting the 
    /// Model's data for display purposes.
    /// </summary>
    public class CreationDateToDisplayTextConverter : IValueConverter
    {
        public object Convert(
            object value, Type targetType,
            object parameter, CultureInfo culture)
        {
            if (value is DateTime == false)
                return String.Empty;
            else
                return ((DateTime)value).ToShortDateString();
        }

        public object ConvertBack(
            object value, Type targetType,
            object parameter, CultureInfo culture)
        {
            throw new NotSupportedException("Cannot convert back");
        }
    }
}