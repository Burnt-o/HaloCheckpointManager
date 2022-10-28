using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Security.Principal;
using System.Windows;
using System.Diagnostics;

namespace HCM3.Startup
{
    public class HCMSetup
    {

        #region Data
        //Required folders that HCM needs to be able to run. We can try to create these if they don't exist already.
        private static readonly string[] _requiredFolders =
            {
            @"Saves",
            @"Saves\Halo 1",
            @"Saves\Halo 2",
            @"Saves\Halo 3",
            @"Saves\Halo 3 ODST",
            @"Saves\Halo Reach",
            @"Saves\Halo 4",
            @"Pointers",
        };

        //Required files that HCM needs to be able to run.
        private static readonly string[] _requiredFiles =
    {
                        @"BurntMemory.dll",
                        @"SpeedHack.dll",
        };


        #endregion // Data



        /// <summary>
        /// Runs a few checks to see if HCM is good and setup ready-to-go. Is only run on HCM startup.
        /// </summary>
        /// <returns>Bool, true if all checks passed, false otherwise. 
        /// Also outs a string that describes what went wrong, if something went wrong.
        /// This string is intended to be presented to the user via a MessageBox</returns>
        public bool HCMSetupChecks(out string errorString)
        {
            // 1: Check that the process is elevated. HCM needs admin permissions to read/write process memory.
            // 2: Check that it has file read/write access in it's local folder.
            // 3: Check that the local folder contains certain folders. It will attempt to create them if they don't exist.
            // 4: Check that we have required assemblies that HCM needs to run.  

            // Local functions will out to this, which will be appended to the errorString
            string error;

            #region Is Elevated?
            // Check if HCM is running with elevated (Administrator) privileges.
            bool IsElevated()
            {
                return (WindowsIdentity.GetCurrent().Owner?.IsWellKnown(WellKnownSidType.BuiltinAdministratorsSid)).GetValueOrDefault();
            }

            if (!IsElevated())
            {
                errorString = "HCM needs admin privileges to operate, the application will now close." +
                    "\nTo run as admin, right click the exe and 'Run As Administrator'" +
                    "\n \nIf you're (rightfully) cautious of giving software admin privs," +
                    "\nfeel free to inspect/build the source from over at \n github.com/Burnt-o/HaloCheckpointManager";
                return false;
            }
            #endregion

            #region Have File Access?
            //Check if HCM has file access to it's local directory. We test this by making a temporary subdirectory, checking it exists, then deleting it.
            bool HaveFileAccess(out string error)
            {
                error = "";
                try
                {
                    string localFolder = Directory.GetCurrentDirectory();
                    string testFolder = localFolder + $@"\testDir";

                    var obj = Directory.CreateDirectory(testFolder);

                    if (Directory.Exists(testFolder))
                    {
                        Directory.Delete(testFolder, false);
                        return true;
                    }
                    else
                    {
                        return false;
                    }

                }
                catch (Exception ex)
                {
                    error = ex.Message;
                    return false;
                }
            }
            
            if (!HaveFileAccess(out error))
            {
                errorString = "HCM needs file read/write permissions in it's local directory." +
                    "\nDouble check the security permissions of the directory HCM is stored inside."
                    + "\n\nError: " + error;
                return false;
            }
            #endregion

            #region Have Required Folders?
            //Check if we have the required files and folders. Create them with default values if we don't, only returns false if it still failed to create them.
            bool HaveRequiredFolders(out string error)
            {
                error = "";
                foreach (var folder in _requiredFolders)
                {
                    var folderPath = Directory.GetCurrentDirectory() + $@"\{folder}";
                    try
                    {
                        Directory.CreateDirectory(folderPath);
                    }
                    catch (Exception ex)
                    {
                        error = ex.Message;
                        return false;
                    }
                }

                return true;
            }
            
            if (!HaveRequiredFolders(out error))
            {
                errorString = "HCM tried to create it's required working directories but failed." +
                    "\nDouble check the security permissions of the directory HCM is stored inside."
                    + "\n\nError: " + error;
                return false;
            }
            #endregion

            #region Have Required Files?
            //Check if we have the required assemblies, like BurntMemory.dll, etc. Get's a list of missing files if any are missing.
            bool HaveRequiredFiles(out string missingAssemblies)
            {
                missingAssemblies = "";
                foreach (var assembly in _requiredFiles)
                {
                    var filePath = Path.Combine(Directory.GetCurrentDirectory(), assembly);
                    if (!File.Exists(filePath))
                    {
                        missingAssemblies = missingAssemblies + (missingAssemblies == "" ? "" : ", ") + assembly;
                    }
                }

                return missingAssemblies == "";
            }
            
            if (!HaveRequiredFiles(out string missingFiles))
            {
                errorString = "HCM is missing required files from it's local directory!" +
                    "\nYou may have to re-download HCM to get the required files."
                    + "\n\nMissing files: " + missingFiles;
                return false;
            } 
            #endregion

            // All checks passed, return success!
            errorString = "Success!";
            return true;

        }

    }
}
