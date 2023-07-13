using System.IO;
using System.Diagnostics;
using HCMExternal.Models;

namespace HCMExternal.Services.CheckpointServiceNS
{

    public partial class CheckpointService
    {
        /// <summary>
        /// Opens a saveFolder in windows explorer.
        /// </summary>
        /// <param name="SelectedSaveFolder">The saveFolder to view.</param>
        public void OpenInExplorer(SaveFolder? SelectedSaveFolder)
        {
            if (SelectedSaveFolder == null) return;

            if (!Directory.Exists(SelectedSaveFolder.SaveFolderPath) ) return;

            Process.Start(new ProcessStartInfo(SelectedSaveFolder.SaveFolderPath) { UseShellExecute = true });

        }
    }
   
}
