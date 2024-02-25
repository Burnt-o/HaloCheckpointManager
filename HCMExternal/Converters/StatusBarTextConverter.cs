using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media;
using HCMExternal.ViewModels;

namespace HCMExternal.Converters
{
    internal class StatusBarTextConverter : IValueConverter
    {

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            if (value == null || !(value is StatusBarState))
                return ("UI Error " + (value == null) + (value is StatusBarState));



            switch ((StatusBarState)value)
            {
                case StatusBarState.MCCProcessNotFound: return "MCC process not found.";
                case StatusBarState.InjectionSucceeded: return "Internal connected.";
                case StatusBarState.InjectionInProgress: return "Internal injecting...";
                case StatusBarState.InjectionFailed: return "Error connecting internal!";
                default: return "UI Error (default)";
            }



        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }

    }
}
