using HCMExternal.Models;
using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace HCMExternal.Converters
{
    internal class StatusBarButtonVisibilityConverter : IValueConverter
    {

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            if (value == null || !(value is MCCHookStateEnum))
            {
                return (Visibility.Hidden);
            }

            switch ((MCCHookStateEnum)value)
            {
                case MCCHookStateEnum.InternalInjectError:
                case MCCHookStateEnum.InternalException:
                case MCCHookStateEnum.MCCAccessError:
                case MCCHookStateEnum.MCCEACError:
                case MCCHookStateEnum.StateMachineException:
                    return Visibility.Visible;

                default:
                case MCCHookStateEnum.MCCNotFound:
                case MCCHookStateEnum.InternalInjecting:
                case MCCHookStateEnum.InternalInitialising:
                case MCCHookStateEnum.InternalSuccess:
                    return Visibility.Hidden;


            }



        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }

    }
}
