using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Diagnostics;
using System.Collections.ObjectModel;
namespace HCM3.Model.CheckpointModels
{
    internal sealed partial class CheckpointModel
    {

        // TODO: make this static
        public ObservableCollection<Checkpoint> PopulateCheckpointList(SaveFolder? SelectedSaveFolder, int SelectedGame)
        {
            ObservableCollection<Checkpoint> checkpointCollection = new();




                string? saveFolderPath = SelectedSaveFolder?.SaveFolderPath;
            if (saveFolderPath == null)
            { 
                // Set it to root folder
            saveFolderPath = @"Saves\" + Dictionaries.TabIndexToRootFolderPath[SelectedGame];
            }
                
                if (!Directory.Exists(saveFolderPath))
                {
                    Trace.WriteLine("RefreshCheckpointList: saveFolderPath didn't exist!");
                    return checkpointCollection;
                }

                // We only want files with ".bin" extension.
                FileInfo[] checkpointFileArray = new DirectoryInfo(saveFolderPath).GetFiles("*.bin").ToArray();

                if (!checkpointFileArray.Any()) // If no files of type .bin in directory, then we leave the checkpoint collection empty.
                {
                    Trace.WriteLine("RefreshCheckpointList: saveFolderPath had no .bin files so leaving checkpointCollection empty.");
                    return checkpointCollection;
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
                    Checkpoint? cp = CheckpointFileDecoder.ReadCheckpointFromFile(checkpointFile, SelectedGame, MainModel);
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