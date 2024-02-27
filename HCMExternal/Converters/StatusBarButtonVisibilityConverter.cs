using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media;
using HCMExternal.ViewModels;

namespace HCMExternal.Converters
{
    internal class StatusBarButtonVisibilityConverter : IValueConverter
    {

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            if (value == null || !(value is StatusBarState))
                return (Visibility.Hidden);



            switch ((StatusBarState)value)
            {
                case StatusBarState.InjectionFailed: return Visibility.Visible;

                default: // fall thru
                case StatusBarState.MCCProcessNotFound: 
                case StatusBarState.InjectionSucceeded: 
                case StatusBarState.InjectionInProgress: 
                    return Visibility.Hidden;

               
            }



        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }

    }
}
