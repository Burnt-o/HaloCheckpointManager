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
    internal class StatusBarColorConverter : IValueConverter
    {

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            if (value == null || !(value is StatusBarState))
                return Brushes.Transparent;



            switch ((StatusBarState)value)
            {
                case StatusBarState.MCCProcessNotFound: return Brushes.Transparent;
                case StatusBarState.InjectionSucceeded: return Brushes.LightGreen;
                case StatusBarState.InjectionInProgress: return Brushes.Yellow;
                case StatusBarState.InjectionFailed: return Brushes.Red;
                default: return Brushes.Transparent;
            }



        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }

    }
}
