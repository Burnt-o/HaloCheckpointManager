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
using HCMExternal.Helpers.DictionariesNS;

namespace HCMExternal.Converters
{
    public class IGTConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return "???";

            int gameTicks = (int)value; 
            if (gameTicks <= 0) // To avoid dividing 0 later. Should never be negative unless there was an error reading the checkpoint data.
            {
                return "???";
            }

            // So I'd like this to check what the current tab index is directly, but haven't figured out how.
            // ConverterParameter won't seem to work since this is from a control under the tab control.
            // For now we'll used the saved setting which should always be accurate.
            var gameindex = (HaloTabEnum)HCMExternal.Properties.Settings.Default.LastSelectedGameTab;
            // Halo 1 (game/tab 0) runs at 30 ticks per sec, the rest at 60.
            double divisor = (gameindex == HaloTabEnum.Halo1) ? (double)30 : (double)60;
            double seconds = gameTicks / divisor;

            string formatter = @"mm\:ss";
            if (seconds >= (double)3600)
            {
                Trace.WriteLine("ALTERNATE WAS HIT");
                formatter = @"hh\:mm\:ss";
            }

            //for detailed checkpoint info, we want the deci-seconds
            if (parameter != null && parameter.GetType() == typeof(string) && (string)parameter == "true")
            {
                formatter = formatter + @"\.ff";
            }

            TimeSpan ts = TimeSpan.FromSeconds(seconds);
            string formattedstring = ts.ToString(formatter);
            Trace.WriteLine(formattedstring);
            return formattedstring;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
