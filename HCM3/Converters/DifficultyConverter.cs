using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System;
using System.Globalization;
using System.Windows.Controls;
using System.Windows.Data;

namespace HCM3.Converters
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
            
          //TODO: image mapping for the difficulty icons.. 



            return (string) DifficultyMap[(int)value];
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
