using HCMExternal.Helpers.DictionariesNS;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows.Data;

namespace HCMExternal.Converters
{
    public class LevelNameConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            string? valueString = value as string;
            if (valueString == null)
            {
                return "???";
            }


            // So I'd like this to check what the current tab index is directly, but haven't figured out how.
            // ConverterParameter won't seem to work since this is from a control under the tab control.
            // For now we'll used the saved setting which should always be accurate.
            HaloTabEnum game = (HaloTabEnum)HCMExternal.Properties.Settings.Default.LastSelectedGameTab;

            // Must be within bounds of array

            Dictionary<string, Dictionaries.LevelInfo> stringMapper = Dictionaries.GameToLevelInfoDictionary[game];
            if (stringMapper.ContainsKey(valueString))
            {
                return stringMapper[valueString].FullName;
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
