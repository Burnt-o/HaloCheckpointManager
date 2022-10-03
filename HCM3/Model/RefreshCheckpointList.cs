using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Diagnostics;

namespace HCM3.Model
{
    internal sealed partial class CheckpointModel
    {
        public void RefreshCheckpointList(int selectedTabIndex)
        {
            if (selectedTabIndex == 6) // 6 is Settings tab, so no need to refresh the checkpointlist. 
            {
                Trace.WriteLine("RefreshCheckpointList: tabindex is 6 so not refreshing checkpoint collection");
                return;
            }

            // Clear the checkpoint collection. We're about to re-populate it.
            CheckpointCollection.Clear();


            string saveFolderPath = @"Saves\" + Dictionaries.TabIndexToRootFolderPath[selectedTabIndex];
            if (!Directory.Exists(saveFolderPath))
            {
                Trace.WriteLine("RefreshCheckpointList: saveFolderPath didn't exist!");
                return;
            }

            // We only want files with ".bin" extension.
            FileInfo[] checkpointFileArray = new DirectoryInfo(saveFolderPath).GetFiles("*.bin").ToArray();

            if (!checkpointFileArray.Any()) // If no files of type .bin in directory, then we leave the checkpoint collection empty.
            {
                Trace.WriteLine("RefreshCheckpointList: saveFolderPath had no .bin files so leaving checkpointCollection empty.");
                return;
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

            // Now let's create a checkpoint file decoder object. 
            CheckpointFileDecoder decoder = new();

            // Magic happens over in the decoder. Add our new checkpoint to the list
            foreach (FileInfo checkpointFile in checkpointFileArray)
            {
                Checkpoint? cp = decoder.ReadCheckpointFromFile(checkpointFile, selectedTabIndex, MainModel.PointerCollection);
                if (cp != null)
                {
                    CheckpointCollection.Add(cp);
                }
            }


        }

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