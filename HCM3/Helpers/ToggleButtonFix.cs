using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls.Primitives;

namespace HCM3.Helpers
{
    // from https://stackoverflow.com/a/5822551
    // To fix ToggleButton (ie Checkbox) IsChecked not binding OneWay properly
    public class ToggleButtonEx
    {
        public static readonly DependencyProperty IsCheckedProperty =
            DependencyProperty.RegisterAttached("IsChecked", typeof(bool), typeof(ToggleButtonEx), new PropertyMetadata(false, OnChanged));

        public static bool GetIsChecked(DependencyObject obj)
        {
            return (bool)obj.GetValue(IsCheckedProperty);
        }
        public static void SetIsChecked(DependencyObject obj, bool value)
        {
            System.Diagnostics.Trace.WriteLine("ARGGGGGGGGGG" + value);
            obj.SetValue(IsCheckedProperty, value);
        }

        private static void OnChanged(DependencyObject o, DependencyPropertyChangedEventArgs args)
        {
            var tb = o as ToggleButton;
            if (null != tb)
                tb.IsChecked = (bool)args.NewValue;
        }
    }

}
