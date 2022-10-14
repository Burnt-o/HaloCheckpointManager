using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System;
using System.Globalization;
using System.Windows.Controls;
using System.Windows.Data;
using HCM3.Models;
using System.Windows;
using System.Windows.Media.Imaging;
using System.IO;
using HCM3.Helpers;
namespace HCM3.Converters
{
    public class DifficultyConverterAsIcon : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            int? difficulty = (int?)value;
            if (difficulty == null || difficulty < 0 || difficulty > 4)
                return DependencyProperty.UnsetValue;

            // So I'd like this to check what the current tab index is directly, but haven't figured out how.
            // ConverterParameter won't seem to work since this is from a control under the tab control.
            // For now we'll used the saved setting which should always be accurate.
            int gameindex = HCM3.Properties.Settings.Default.LastSelectedTab;

            string game = Dictionaries.TabIndexTo2LetterGameCode[gameindex];

            string sourceImagePath = $"Images/{game}/diff_{difficulty}.png";

            Uri sourceImageUri = new Uri(sourceImagePath, UriKind.Relative);

            BitmapImage image = new(sourceImageUri);

            return image;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
