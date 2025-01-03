using HCMExternal.Models;
using Serilog;
using System.Diagnostics;
using System.IO;

namespace HCMExternal.Services.CheckpointIO.Impl
{
    public partial class CheckpointIOService
    {
        /// <summary>
        /// Opens a saveFolder in windows explorer.
        /// </summary>
        /// <param name="SelectedSaveFolder">The saveFolder to view.</param>
        public void OpenInExplorer(SaveFolder SelectedSaveFolder)
        {
            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath))
            {
                Log.Error("OpenInExplorer: selected save folder didn't actually exist at path " + SelectedSaveFolder.SaveFolderPath);
                return;
            }

            Process.Start(new ProcessStartInfo(SelectedSaveFolder.SaveFolderPath) { UseShellExecute = true });

        }
    }

}
