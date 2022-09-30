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
            double divisor;
            Trace.WriteLine("EEEE: " + value.ToString());
            switch (parameter as string)
            {
                case "H1":
                    divisor = 30;
                    break;

                case "H2":
                case "H3":
                case "OD":
                case "HR":
                case "H4":
                    divisor = 60;
                    break;

                default:
                    Trace.WriteLine("IGTConverter recieved bad parameter");
                    Trace.WriteLine(parameter as string);
                    throw new NotImplementedException();
            }
            

            Trace.WriteLine("FFFF: " + ((int)value / divisor));
            TimeSpan gameTime = TimeSpan.FromSeconds((int)value / divisor);
            string format = @"mm\:ss\.ff";


            return gameTime;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
