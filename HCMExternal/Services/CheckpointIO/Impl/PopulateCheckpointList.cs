
using HCMExternal.Models;
using Serilog;
using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;

namespace HCMExternal.Services.CheckpointIO.Impl
{
    public partial class CheckpointIOService
    {
        /// <summary>
        /// Iterates through all the checkpoint files in a saveFolder and decodes them, creating the checkpointCollection observed by CheckpointViewModel.
        /// </summary>
        /// <param name="SelectedSaveFolder">The saveFolder containing the checkpoints to parse.</param>
        /// <param name="SelectedGame">Which game are these checkpoints from?</param>
        /// <returns></returns>
        public ObservableCollection<Checkpoint> PopulateCheckpointList(SaveFolder SelectedSaveFolder, HaloGame SelectedGame)
        {
            ObservableCollection<Checkpoint> checkpointCollection = new();


            string saveFolderPath = SelectedSaveFolder.SaveFolderPath;

            if (!Directory.Exists(saveFolderPath))
            {
                Log.Error("RefreshCheckpointList: saveFolderPath didn't exist at path " + saveFolderPath);
                return checkpointCollection;
            }

            // We only want files with ".bin" extension.
            FileInfo[] checkpointFileArray = new DirectoryInfo(saveFolderPath).GetFiles("*.bin").ToArray();

            if (!checkpointFileArray.Any()) // If no files of type .bin in directory, then we leave the checkpoint collection empty.
            {
                Log.Debug("RefreshCheckpointList: saveFolderPath had no .bin files so leaving checkpointCollection empty.");
                return checkpointCollection;
            }

            //round every LastWriteTime to seconds
            foreach (FileInfo checkpointFile in checkpointFileArray)
            {
                checkpointFile.LastWriteTime = checkpointFile.LastWriteTime.AddTicks(-(checkpointFile.LastWriteTime.Ticks % TimeSpan.TicksPerSecond));
            }


            // Need to fix files so that none of them have the exact same LastWriteTime ("last modified on" file metadata), since we use this property for sorting
            // This is done using two nested foreach loops (one here, one in local function FileHasSameTime)
            foreach (FileInfo checkpointFile in checkpointFileArray)
            {
                while (FileHasSameTime(checkpointFileArray, checkpointFile))
                {
                    checkpointFile.LastWriteTime = checkpointFile.LastWriteTime.AddSeconds(1);
                }
            }



            // Magic happens over in the decoder. Add our new checkpoint to the list
            foreach (FileInfo checkpointFile in checkpointFileArray)
            {
                Checkpoint? cp = DecodeCheckpointFile(checkpointFile, SelectedGame);
                if (cp != null)
                {
                    checkpointCollection.Add(cp);
                }
            }


            return checkpointCollection;


            // Checks if a file has the same LastWriteTime as any other file in an array (besides itself, of course)
            bool FileHasSameTime(FileInfo[] checkpointFileArray, FileInfo currentfile)
            {
                foreach (FileInfo File in checkpointFileArray)
                {
                    if (currentfile.Name != File.Name && File.LastWriteTime == currentfile.LastWriteTime)
                    {
                        return true;
                    }
                }
                return false;
            }

        }
    }
}