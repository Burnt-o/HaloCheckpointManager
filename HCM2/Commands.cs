using System.Windows.Input;

namespace HCM
{
    /// <summary>
    /// Contains application-specific routed commands.
    /// </summary>
    public static class Commands
    {
        /// <summary>
        /// Executed when the selected Xray's 
        /// image should be displayed.
        /// </summary>
        public static readonly RoutedUICommand RenameSelectedItem;

        static Commands()
        {
            RenameSelectedItem = new RoutedUICommand(
                "Lemme rename you",
                "RenameItem",
                typeof(Commands));
        }
    }
}