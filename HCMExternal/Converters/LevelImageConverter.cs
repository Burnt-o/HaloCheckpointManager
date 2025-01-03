
using Serilog;
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
            string? levelCode = value as string;
            if (levelCode == null)
            {
                return $"Images/errorfile.png";
            }


            // So I'd like this to check what the current tab index is directly, but haven't figured out how.
            // ConverterParameter won't seem to work since this is from a control under the tab control.
            // For now we'll used the saved setting which should always be accurate.
            HaloGame gameEnum = (HaloGame)HCMExternal.Properties.Settings.Default.LastSelectedGameTab;

            //check if level is a multiplayer level, if so we use the mp image
            try
            {
                LevelInfo levelInfo = new LevelInfo(gameEnum, levelCode);

                // Check if level is a multiplayer map, which just share a generic image
                if (levelInfo.LevelPosition == -1)
                    return $"Images/mp.png";

                //Otherwise, check whether the user wants the anniversary or classic icon for this level, then return the url to the image
                string imageModeSuffix = HCMExternal.Properties.Settings.Default.ImageMode ? "anni" : "clas";
                string URL = $"Images/{gameEnum.ToAcronym()}/{levelCode}_{imageModeSuffix}.png";

                return URL;

            }
            catch(Exception ex)
            {
                Log.Error("Level converter error: " + ex.Message + "\n" + ex.Source + "n" + "for levelcode: " + levelCode);
                return $"Images/errorfile.png";
            }

           

        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
