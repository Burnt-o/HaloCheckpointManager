using HCMExternal.Helpers.DictionariesNS;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows.Data;

namespace HCMExternal.Converters


{
    public class LevelImageConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            string? valueString = value as string;
            if (valueString == null)
            {
                return "???";
            }


            // So I'd like this to check what the current tab index is directly, but haven't figured out how.
            // ConverterParameter won't seem to work since this is from a control under the tab control.
            // For now we'll used the saved setting which should always be accurate.
            HaloTabEnum gameindex = (HaloTabEnum)HCMExternal.Properties.Settings.Default.LastSelectedGameTab;
            string gameID = Dictionaries.GameTo2LetterGameCode[gameindex];

            //check if level is a multiplayer level, if so we use the mp image
            Dictionary<string, Dictionaries.LevelInfo> levelInfoGrabber = Dictionaries.GameToLevelInfoDictionary[gameindex];
            if (levelInfoGrabber.TryGetValue(valueString, out Dictionaries.LevelInfo levelinfo) && levelinfo.LevelPosition == -1)
            {
                return $"Images/mp.png";
            }

            //Otherwise, check whether the user wants the anniversary or classic icon for this level, then return the url to the image
            string imageModeSuffix = HCMExternal.Properties.Settings.Default.ImageMode ? "anni" : "clas";
            string URL = $"Images/{gameID}/{valueString}_{imageModeSuffix}.png";

            return URL;

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
