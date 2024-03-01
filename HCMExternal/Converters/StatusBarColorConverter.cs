using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media;
using HCMExternal.Models;
using HCMExternal.ViewModels;
using Serilog;

namespace HCMExternal.Converters
{
    internal class StatusBarColorConverter : IValueConverter
    {

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            if (value == null || !(value is MCCHookStateEnum))
                return Brushes.Transparent;

            Log.Verbose("Converting status bar colour " + ((MCCHookStateEnum)value).ToString());

            switch ((MCCHookStateEnum)value)
            {
                case MCCHookStateEnum.MCCNotFound: 
                    return Brushes.Transparent;

                case MCCHookStateEnum.InternalInjecting: 
                case MCCHookStateEnum.InternalInitialising: 
                    return Brushes.Yellow;

                case MCCHookStateEnum.InternalSuccess: 
                    return Brushes.LightGreen;

                case MCCHookStateEnum.InternalInjectError: 
                case MCCHookStateEnum.InternalException:
                case MCCHookStateEnum.MCCAccessError:
                case MCCHookStateEnum.MCCEACError:
                    return Brushes.Red;

                default: return Brushes.Transparent;
            }



        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }

    }
}
