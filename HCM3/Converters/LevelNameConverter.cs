using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Globalization;
using System.Windows.Data;
using HCM3.Model;

namespace HCM3.Converters
{
    public class LevelNameConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            string? valueString = value as string;
            if (valueString == null)
                return "???";



            // So I'd like this to check what the current tab index is directly, but haven't figured out how.
            // ConverterParameter won't seem to work since this is from a control under the tab control.
            // For now we'll used the saved setting which should always be accurate.
            int game = HCM3.Properties.Settings.Default.LastSelectedTab;

            // Must be within bounds of array
            if (game < Dictionaries.GameToLevelCodeDictionary.Length)
            {
                Dictionary<string, string> stringMapper = Dictionaries.GameToLevelCodeDictionary[game];
                if (stringMapper.ContainsKey(valueString))
                {
                    return stringMapper[valueString];
                }
                else
                {
                    return "???";
                }
            }
            else
            {
                return "???";
            }



        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
