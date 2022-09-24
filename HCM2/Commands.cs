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
        public static readonly RoutedUICommand RenameSelectedCheckpoint;

        static Commands()
        {
            RenameSelectedCheckpoint = new RoutedUICommand(
                "Lemme rename you",
                "RenameCheckpoint",
                typeof(Commands));
        }
    }
}