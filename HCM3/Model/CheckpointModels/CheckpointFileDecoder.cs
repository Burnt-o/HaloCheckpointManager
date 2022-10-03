using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Diagnostics;


namespace HCM3.Model.CheckpointModels
{
    internal class CheckpointFileDecoder
    {

        public Checkpoint? ReadCheckpointFromFile(FileInfo checkpointFile, int gameIndex, PointerCollection pcollection)
        {
            // Need this for accessing pointercollection later
            string gameString = Dictionaries.TabIndexToRootFolderPath[gameIndex];

            
            string? checkpointName = checkpointFile.Name;
            // We want to trim off the ".bin" part of the filename.
            checkpointName = checkpointName[..checkpointName.LastIndexOf(".")];

            // We don't allow checkpoints with empty/null filenames
            if (checkpointName == null)
            {
                return null;
            }

            // Grab file metadata
            DateTime? checkpointCreatedTime = checkpointFile.CreationTime;
            DateTime? checkpointModifedTime = checkpointFile.LastWriteTime;


            // Next, check if the filesize is reasonable. If not we'll return null.
            // Valid range is 100kb to 100mb, pulled out my ass.
            if (checkpointFile.Length < (100 * 1000) || checkpointFile.Length > (100 * 1000 * 1000))
            {
                return null;
            }

            // Setup the variables we'll be trying to get from the file's data
            string? checkpointLevelName;
            int? checkpointDifficulty;
            int? checkpointGameTickCount;

            // Now load the whole damn file into memory and read some data from it
            try
            {
                using (FileStream readStream = new FileStream(checkpointFile.FullName, FileMode.Open))
                {
                    using (BinaryReader readBinary = new BinaryReader(readStream))
                    {
                        // Go to the end of the checkpoint file and see if there's a valid version string written there.
                        // (Version string is 10 chars long)
                        readBinary.BaseStream.Seek(-10, SeekOrigin.End);
                        string checkpointVersion = new string(readBinary.ReadChars(10));
                        if (!checkpointVersion.StartsWith("1."))
                        {
                            return null;    
                            //TODO
                            // need to get version from whatever version of MCC is currently running. Or latestver in pointercollection
                            // If that fails we'll leave some properties unset. Well basically only set filename.

                        }
                        int? offsetLevelName = (int?)pcollection.GetPointer(gameString + "_CheckpointData_LevelName", checkpointVersion)?.Address;
                        if (offsetLevelName.HasValue)
                        { 
                        readBinary.BaseStream.Seek((long)offsetLevelName, SeekOrigin.Begin);
                        checkpointLevelName = new string(readBinary.ReadChars(10)); //TODO: changes this to variable length
                        }
                    }
                }


            }
            catch (Exception e)
            {
                Trace.WriteLine("Failed reading checkpoint from file: " + e.Message);
                return null;
            }




                return new Checkpoint(checkpointName); // Then return a new checkpoint with as many valid properties as we managed to get (checkpointName is definitely not null)

              
            
        }


    }
}
