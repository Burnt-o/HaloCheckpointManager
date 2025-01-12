using HCMExternal.Services.CheckpointIO.Impl;
using HCMExternal.Services.External.Impl;
using HCMExternal.Services.Interproc.Impl;
using HCMExternal.Services.PointerData.Impl;
using HCMExternal.Services.CheckpointIO;
using HCMExternal.Services.External;
using HCMExternal.Services.Interproc;
using HCMExternal.Services.PointerData;
using HCMExternal.Services.Hotkeys;
using HCMExternal.Services.Hotkeys.Impl;
using HCMExternal.ViewModels;
using HCMExternal.Views;

using Serilog;
using System;
using System.Diagnostics;
using System.IO;
using System.Windows;
using HCMExternal.Services.Hotkeys;

namespace HCMExternal
{

    public partial class App : Application
    {


        public string CurrentHCMVersion = "unset";

        public App()
        {
            // Logging
            string timestamp = DateTime.Now.ToString("yyyyMMdd_HHmmss");

            Log.Logger = new LoggerConfiguration()
#if HCM_DEBUG
                .MinimumLevel.Verbose().WriteTo.Debug()
#endif
                .MinimumLevel.Verbose().WriteTo.File($"Logs\\HCMExternal_Logging_{timestamp}.txt",
                flushToDiskInterval: TimeSpan.FromSeconds(3))
                .CreateLogger();
            Log.Information("Logging started");

        }



        private void Application_Startup(object sender, StartupEventArgs e)
        {

            AppDomain.CurrentDomain.UnhandledException += (sender, args) =>
                    ShowErrorAndShutdown("An unhandled exception occured!\n" + (args.ExceptionObject as Exception).Message + "\n\nStackTrace:\n" + (args.ExceptionObject as Exception).StackTrace);

            System.Reflection.Assembly assembly = System.Reflection.Assembly.GetExecutingAssembly();
            System.Diagnostics.FileVersionInfo fvi = System.Diagnostics.FileVersionInfo.GetVersionInfo(assembly.Location);
            CurrentHCMVersion = fvi.FileVersion ?? "no version info";

            try
            {
                CheckRequiredFoldersAndFiles();
            }
            catch (Exception ex)
            {
                ShowErrorAndShutdown(ex.Data.Contains("UserMessage") ? ex.Data["UserMessage"].ToString() : ex.ToString());
                return;
            }


            // Set up services and view models

            // Tell DataPointersService to load data
            IPointerDataService pointerData;
            try
            {
                (pointerData, string pointerErrors) = PointerDataServiceFactory.MakePointerDataService();
                if (pointerErrors != "")
                { 
                    ShowError("Some pointers failed to load. Yell at Burnt for making typos.\n" + pointerErrors); 
                }
            }
            catch (Exception ex)
            {
                ShowErrorAndShutdown(ex.ToString() + "\n" + ex.Message + "\n" + ex.Source);
                return;
            }


            ICheckpointIOService checkpointIO = new CheckpointIOService(pointerData);
            IExternalService external = new ExternalService(pointerData);
            IInterprocService interproc = new InterprocService();
            IHotkeyManager hotkey = new HotkeyManager();

            // main view model will construct lower view models too
            MainViewModel mainViewModel = new MainViewModel(checkpointIO, external, interproc, hotkey);

         


            // Setup theming
            string appTheme = HCMExternal.Properties.Settings.Default.DarkMode ? "Dark" : "Light";
            Resources.MergedDictionaries[0].Source = new Uri($"/Themes/{appTheme}.xaml", UriKind.Relative);

            MainWindow mainWindow = new();
            mainWindow.DataContext = mainViewModel;
            mainWindow.Title = "HaloCheckpointManager " + (CurrentHCMVersion.Length > 4 ? CurrentHCMVersion.Substring(0, 5) : CurrentHCMVersion);
            mainWindow.Show();

            interproc.initializeSharedMemory(mainViewModel.FileViewModel.SelectedCheckpoint, mainViewModel.FileViewModel.SelectedSaveFolder, mainViewModel.FileViewModel.SelectedGame);

        }


        private void Application_Exit(object sender, ExitEventArgs e)
        {
            Log.CloseAndFlush();
            // TODO: send shutdown message to internal, serialise any config stuff
        }


        private void ShowErrorAndShutdown(string error)
        {
            ShowError(error);
            System.Windows.Application.Current.Shutdown();
        }

        private void ShowError(string error)
        {
            Log.Error(error);
            System.Windows.MessageBox.Show(error, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
        }



        #region RequiredFoldersAndFiles
        //Required folders that HCM needs to be able to run. We can try to create these if they don't exist already.
        private static readonly string[] _requiredFolders =
            {
            @"Logs",
            @"Saves",
            @"Saves\Halo 1",
            @"Saves\Halo 2",
            @"Saves\Halo 3",
            @"Saves\Halo 3 ODST",
            @"Saves\Halo Reach",
            @"Saves\Halo 4",
            @"Saves\Project Cartographer",
        };

        //Required files that HCM needs to be able to run.
        private static readonly string[] _requiredAssemblies =
        {
                       @"HCMInternal.dll",
                       @"HCMInterproc.dll"
        };


        #endregion // RequiredFoldersAndFiles

        private void CheckRequiredFoldersAndFiles()
        {
            #region Have File Access?
            //Check if HCM has file access to it's local directory. We test this by making a temporary subdirectory, checking it exists, then deleting it.

            try
            {
                string localFolder = Directory.GetCurrentDirectory();
                string testFolder = localFolder + $@"\testDir";

                DirectoryInfo obj = Directory.CreateDirectory(testFolder);

                if (Directory.Exists(testFolder))
                {
                    Directory.Delete(testFolder, false);
                }
                else
                {
                    throw new Exception("Test directory didn't exist: " + testFolder);
                }

            }
            catch (Exception ex)
            {
                ex.Data.Add("UserMessage",
                     "HCM needs file read/write permissions in it's local directory." +
                    "\nDouble check the security permissions of the directory HCM is stored inside."
                    + "\n\nError: " + ex.ToString()
                    );
                throw;
            }

            #endregion

            #region Have Required Folders?
            //Check if we have the required files and folders. Create them with default values if we don't, only returns false if it still failed to create them.


            foreach (string folder in _requiredFolders)
            {
                string folderPath = Directory.GetCurrentDirectory() + $@"\{folder}";
                try
                {
                    Directory.CreateDirectory(folderPath);
                }
                catch (Exception ex)
                {
                    ex.Data.Add("UserMessage",
                    "HCM tried to create it's required working directories but failed." +
                    "\nDouble check the security permissions of the directory HCM is stored inside."
                    + "\n\nError: " + ex.ToString()
                    );
                    throw;
                }
            }


            #endregion

            #region Have Required Files?
            //Check if we have the required assemblies, like BurntMemory.dll, etc. Get's a list of missing files if any are missing.

            string missingAssemblies = "";
            foreach (string assembly in _requiredAssemblies)
            {
                string filePath = Path.Combine(Directory.GetCurrentDirectory(), assembly);
                if (!System.IO.File.Exists(filePath))
                {
                    missingAssemblies = missingAssemblies + (missingAssemblies == "" ? "" : ", ") + assembly;
                }
                else
                {
                    // file exists: check that it has the current version information
                    FileVersionInfo assemblyVersion = FileVersionInfo.GetVersionInfo(filePath);
                    string versionString = string.Format("{0}.{1}.{2}.{3}", assemblyVersion.FileMajorPart,
                                                                        assemblyVersion.FileMinorPart,
                                                                          assemblyVersion.FileBuildPart,
                                                                          assemblyVersion.FilePrivatePart);
                    if (versionString != CurrentHCMVersion)
                    {
                        missingAssemblies = missingAssemblies + (missingAssemblies == "" ? "" : ", ") + assembly + "(wrong file version! was " + versionString + ")";
                    }
                }


            }

            if (missingAssemblies != "")
            {
                throw new Exception("HCM is missing required files from it's local directory!" +
                    "\nYou may have to re-download HCM to get the required files."
                    + "\n\nMissing files: " + missingAssemblies);
            }



            #endregion

        }


    }
}
