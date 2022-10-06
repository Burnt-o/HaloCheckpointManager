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


        public Checkpoint? ReadCheckpointFromFile(FileInfo checkpointFile, int gameIndex, MainModel mainModel)
        {


            // Setup checkpoint data we're going to try to populate
            string? checkpointName = null;
            string? checkpointLevelCode = null;
            int? checkpointDifficulty = null;
            int? checkpointGameTickCount = null;
            string? checkpointVersion = null;
            DateTime checkpointCreatedOn = checkpointFile.CreationTime;
            DateTime checkpointModifiedOn = checkpointFile.LastWriteTime;

            // We want to trim off the ".bin" part of the filename.
            checkpointName = checkpointFile.Name;
            checkpointName = checkpointName[..checkpointName.LastIndexOf(".")];

            // We don't allow checkpoints with empty/null filenames
            if (checkpointName == null || checkpointName == "")
            {
                return null;
            }

            // Next, check if the filesize is reasonable. If not we'll return null.
            // Valid range is 100kb to 100mb, pulled out my ass.
            if (checkpointFile.Length < (100 * 1000) || checkpointFile.Length > (100 * 1000 * 1000))
            {
                return null;
            }

            // Now load the whole damn file into memory and read some data from it
            // If it fails then we'll leave the associated values as null
            try
            {
                using (FileStream readStream = new FileStream(checkpointFile.FullName, FileMode.Open))
                {
                    using (BinaryReader readBinary = new BinaryReader(readStream))
                    {
                        // Go to the end of the checkpoint file and see if there's a valid version string written there.
                        // (Version string is 10 chars long)
                        readBinary.BaseStream.Seek(-10, SeekOrigin.End);
                        string? checkpointVersionGuess = new string(readBinary.ReadChars(10));
                        Trace.WriteLine("read ver string: " + checkpointVersionGuess);

                        // If that string started with "1." then it's probably a valid version string, so we'll set checkpointVersion to it too
                        if (checkpointVersionGuess != null && checkpointVersionGuess.StartsWith("1."))
                        {
                            checkpointVersion = checkpointVersionGuess;
                        }
                        else if (mainModel.CurrentAttachedMCCVersion != null) //otherwise set it to version of MCC that HCM is attached to
                        {
                            checkpointVersionGuess = mainModel.CurrentAttachedMCCVersion;
                            Trace.WriteLine("setting ver string to current attached version: " + checkpointVersionGuess);
                        }
                        else if (mainModel.HighestSupportMCCVersion != null) //otherwise set it the highest supported version we loaded from git
                        {
                            checkpointVersionGuess = mainModel.HighestSupportMCCVersion;
                        }
                        else
                        {
                            checkpointVersionGuess = null;
                        }

                        // Now let's try to actually read the data from the file
                        // Each will be in it's own trycatch so that one can fail without the others failing
                        if (checkpointVersionGuess != null)
                        {
                            // Need this for accessing pointercollection
                            string gameString = Dictionaries.TabIndexTo2LetterGameCode[gameIndex];
                            // Read LevelCode
                            try
                            {
                                // Grab the offset; if it exists
                                int? offsetLevelCode = (int?)mainModel.PointerCollection.GetPointer(gameString + "_CheckpointData_LevelCode", checkpointVersionGuess)?.Address;
                                if (offsetLevelCode.HasValue)
                                {
                                    // Use the offset to seek to the correct location and read the data
                                    readBinary.BaseStream.Seek((long)offsetLevelCode, SeekOrigin.Begin);
                                    checkpointLevelCode = new string(readBinary.ReadChars(64));
                                    checkpointLevelCode = checkpointLevelCode.Substring(checkpointLevelCode.LastIndexOf("\\") + 1);
                                    checkpointLevelCode = checkpointLevelCode.Substring(checkpointLevelCode.LastIndexOf("\\") + 1);
                                    char[] exceptions = new char[] { '_' };
                                    checkpointLevelCode = String.Concat(checkpointLevelCode.Where(ch => Char.IsLetterOrDigit(ch) || exceptions?.Contains(ch) == true));
                                    Trace.WriteLine("\\nlevelcode4: " + checkpointLevelCode);
                                }
                            }
                            catch { checkpointLevelCode = null; }

                            // Read GameTickCount
                            try
                            {
                                // Grab the offset; if it exists
                                int? offsetGameTickCount = (int?)mainModel.PointerCollection.GetPointer(gameString + "_CheckpointData_GameTickCount", checkpointVersionGuess)?.Address;
                                if (offsetGameTickCount.HasValue)
                                {
                                    // Use the offset to seek to the correct location and read the data
                                    readBinary.BaseStream.Seek((long)offsetGameTickCount, SeekOrigin.Begin);
                                    // read integer
                                    checkpointGameTickCount = readBinary.ReadInt32();
                                }
                            }
                            catch { checkpointGameTickCount = null; }

                            // Read Difficulty
                            try
                            {
                                // Grab the offset; if it exists
                                int? offsetDifficulty = (int?)mainModel.PointerCollection.GetPointer(gameString + "_CheckpointData_Difficulty", checkpointVersionGuess)?.Address;
                                if (offsetDifficulty.HasValue)
                                {
                                    // Use the offset to seek to the correct location and read the data
                                    readBinary.BaseStream.Seek((long)offsetDifficulty, SeekOrigin.Begin);
                                    // read 1 byte as int
                                    checkpointDifficulty = (int)readBinary.ReadBytes(1)[0];
                                }
                            }
                            catch { checkpointDifficulty = null; }
                        }
                    }
                }
            }
            catch (Exception e) // Reading from file failed, so set those checkpoint values to null
            {
                Trace.WriteLine("Failed reading checkpoint from file: " + e.Message);
                checkpointLevelCode = null;
                checkpointDifficulty = null;
                checkpointGameTickCount = null;
            }

            // Return whatever data we were able to get (all might be null except checkpointName, createdOn, and ModifiedOn)
            return new Checkpoint(checkpointName, checkpointLevelCode, checkpointDifficulty, checkpointGameTickCount, checkpointVersion, checkpointCreatedOn, checkpointModifiedOn); // Then return a new checkpoint with as many valid properties as we managed to get (checkpointName is definitely not null)
        }
    }
}
