using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System;
using System.Globalization;
using System.Windows.Controls;
using System.Windows.Data;

namespace HCMExternal.Converters
{
    public class DifficultyConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            if (value == null)
                return "???";

            Dictionary<int, string> DifficultyMap = new()
                {
                    { 0, "Easy" },
                    { 1, "Normal" },
                    { 2, "Heroic" },
                    { 3, "Legendary" },
                };



            return DifficultyMap.ElementAtOrDefault((int)value).Value ?? "Invalid";

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
