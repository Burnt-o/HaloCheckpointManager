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

        public Checkpoint? ReadCheckpointFromFile(FileInfo checkpointFile, int gameIndex, PointerCollection pcollection, string? CurrentAttachedMCCVersion, string? HighestSupportMCCVersion)
        {
            // Need this for accessing pointercollection later
            string gameString = Dictionaries.TabIndexTo2LetterGameCode[gameIndex];

            
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
            string? checkpointLevelCode = null;
            int? checkpointDifficulty = null;
            int? checkpointGameTickCount = null;
            string? checkpointVersion = null;

            // This will let us know if we were able to read the versionString from the file or not
            bool ableToReadVersionString = false;

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
                        checkpointVersion = new string(readBinary.ReadChars(10));
                        Trace.WriteLine("read ver string: " + checkpointVersion);
                        checkpointVersion = EvaluateCheckpointVersion(checkpointVersion, out bool eh);
                        ableToReadVersionString = eh;
                        if (checkpointVersion != null)
                        {
                            //LEVELCODE
                            // Grab the offset; if it exists
                            try
                            {
                                int? offsetLevelCode = (int?)pcollection.GetPointer(gameString + "_CheckpointData_LevelCode", checkpointVersion)?.Address;
                                if (offsetLevelCode.HasValue)
                                {
                                    // Use the offset to seek to the correct location and read the data
                                    readBinary.BaseStream.Seek((long)offsetLevelCode, SeekOrigin.Begin);
                                    checkpointLevelCode = new string(readBinary.ReadChars(3));
                                    Trace.WriteLine("levelcode: " + checkpointLevelCode);
                                }
                            }
                            catch { }

                            //GAMETICKCOUNT
                            // Grab the offset; if it exists
                            try
                            {
                                int? offsetGameTickCount = (int?)pcollection.GetPointer(gameString + "_CheckpointData_GameTickCount", checkpointVersion)?.Address;
                                if (offsetGameTickCount.HasValue)
                                {
                                    // Use the offset to seek to the correct location and read the data
                                    readBinary.BaseStream.Seek((long)offsetGameTickCount, SeekOrigin.Begin);
                                    // read integer
                                    checkpointGameTickCount = readBinary.ReadInt32();

                                }
                            }
                            catch { }

                            //Difficulty
                            // Grab the offset; if it exists
                            try
                            {
                                int? offsetDifficulty = (int?)pcollection.GetPointer(gameString + "_CheckpointData_Difficulty", checkpointVersion)?.Address;
                                if (offsetDifficulty.HasValue)
                                {
                                    // Use the offset to seek to the correct location and read the data
                                    readBinary.BaseStream.Seek((long)offsetDifficulty, SeekOrigin.Begin);
                                    // read 1 byte as int
                                    checkpointDifficulty = (int)readBinary.ReadBytes(1)[0];
                                }
                            }
                            catch { }
                        }
                    }
                }


            }
            catch (Exception e)
            {
                Trace.WriteLine("Failed reading checkpoint from file: " + e.Message);
                checkpointLevelCode = null;
                checkpointDifficulty = null;
                checkpointGameTickCount = null;
                ableToReadVersionString = false;
            }

            //If we couldn't read the version string then set the new Checkpoint property to "Unknown" or something.
            if (!ableToReadVersionString)
            {
                checkpointVersion = null;
            }
            Trace.WriteLine("handing version string: " + checkpointVersion);

            return new Checkpoint(checkpointName, checkpointLevelCode, checkpointDifficulty, checkpointGameTickCount, checkpointVersion, null, null); // Then return a new checkpoint with as many valid properties as we managed to get (checkpointName is definitely not null)

              
            
            string? EvaluateCheckpointVersion (string? checkpointFileString, out bool checkpointFileStringWasValid)
            {
                // Takes in the string read from the end of the checkpoint file.
                // If it's null we just return null.
                if (checkpointFileString == null)
                {
                    checkpointFileStringWasValid = false;
                    return null;
                }

                

                // If that string started with "1." then it's probably a valid version string, so we'll return itself
                if (checkpointFileString.StartsWith("1."))
                {
                    checkpointFileStringWasValid = true;
                    return checkpointFileString;
                }

                // Otherwise let's grab the current version of MCC that HCM is attached to.. if we're attached.
                if (CurrentAttachedMCCVersion != null)
                {
                    checkpointFileStringWasValid = false;
                    return CurrentAttachedMCCVersion;
                }

                // OTHERWISE otherwise let's grab the highest version we have data on in the PointerCollection
                if (HighestSupportMCCVersion != null)
                {
                    checkpointFileStringWasValid = false;
                    return HighestSupportMCCVersion;
                }

                // Welp, guess we got nothing
                checkpointFileStringWasValid = false;
                return null;

            }

        }


    }
}
