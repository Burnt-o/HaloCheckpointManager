﻿using HCMExternal.Models;
using System;
using System.Globalization;
using System.Windows.Data;

namespace HCMExternal.Converters
{
    internal class StatusBarTextConverter : IValueConverter
    {

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {

            if (value == null || !(value is MCCHookStateEnum))
            {
                return ("UI Error " + (value == null) + (value is MCCHookStateEnum));
            }

            switch ((MCCHookStateEnum)value)
            {
                case MCCHookStateEnum.MCCNotFound: return "MCC process not found.";
                case MCCHookStateEnum.InternalInjecting: return "Internal Injecting..";
                case MCCHookStateEnum.InternalInjectError: return "Error injecting!";
                case MCCHookStateEnum.InternalInitialising: return "Internal Initialising";
                case MCCHookStateEnum.InternalException: return "Internal Exception!";
                case MCCHookStateEnum.InternalSuccess: return "Internal connected.";
                case MCCHookStateEnum.MCCAccessError: return "Access error!";
                case MCCHookStateEnum.MCCEACError: return "Anti-cheat error!";
                default: return "UI Error (default)";
            }



        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }

    }
}
