﻿using HCMExternal.Services.CheckpointServiceNS;
using HCMExternal.Services.DataPointersServiceNS;
using HCMExternal.Services.InterprocServiceNS;
using HCMExternal.Services.MCCHookService;
using HCMExternal.ViewModels;
using HCMExternal.Views;
using Microsoft.Extensions.DependencyInjection;
using Serilog;
using System;
using System.Diagnostics;
using System.IO;
using System.Windows;

namespace HCMExternal
{

    public partial class App : Application
    {


        public string CurrentHCMVersion = "unset";
        private ServiceProvider _serviceProvider;

        public App()
        {
            // Logging
            string timestamp = DateTime.Now.ToString("yyyyMMdd_HHmmss");

            Log.Logger = new LoggerConfiguration()
#if HCM_DEBUG
                .MinimumLevel.Verbose().WriteTo.Debug()
#endif
                .MinimumLevel.Verbose().WriteTo.File($"HCMExternal_Logging_{timestamp}.txt",
                flushToDiskInterval: TimeSpan.FromSeconds(3))
                .CreateLogger();
            Log.Information("Logging started");

            // Services
            ServiceCollection services = new();
            ConfigureServices(services);

            _serviceProvider = services.BuildServiceProvider();
        }

        private void ConfigureServices(ServiceCollection serviceCollection)
        {
            // View
            serviceCollection.AddSingleton<MainWindow>();

            // Viewmodels
            serviceCollection.AddSingleton<CheckpointViewModel>();
            serviceCollection.AddSingleton<MainViewModel>();


            // Services
            serviceCollection.AddSingleton<MCCHookService>();
            serviceCollection.AddSingleton<CheckpointService>();
            serviceCollection.AddSingleton<DataPointersService>();
            serviceCollection.AddSingleton<InterprocService>();


            serviceCollection.AddSingleton<ErrorDialogViewModel>();
            serviceCollection.AddSingleton<ErrorDialogView>();
            serviceCollection.AddSingleton<MCCHookStateViewModel>();





            _serviceProvider = serviceCollection.BuildServiceProvider();
        }


        private void Application_Startup(object sender, StartupEventArgs e)
        {

            AppDomain.CurrentDomain.UnhandledException += (sender, args) =>
                    ShowErrorAndShutdown("An unhandled exception occured!\n" + (args.ExceptionObject as Exception).Message + "\n\nStackTrace:\n" + (args.ExceptionObject as Exception).StackTrace);

            System.Reflection.Assembly assembly = System.Reflection.Assembly.GetExecutingAssembly();
            System.Diagnostics.FileVersionInfo fvi = System.Diagnostics.FileVersionInfo.GetVersionInfo(assembly.Location);
            CurrentHCMVersion = fvi.FileVersion ?? "no version info";

            // TODO: check for required files/folders

            // Tell DataPointersService to load data
            DataPointersService? dataPointersService = _serviceProvider.GetService<DataPointersService>();
            if (dataPointersService == null) { }
            // Create collection of all our needed data and load them from the online repository
            string pointerErrors = "";
            try
            {
                dataPointersService.LoadPointerDataFromSource(out pointerErrors);
            }
            catch (Exception ex)
            {
                ShowErrorAndShutdown(ex.ToString() + "\n" + pointerErrors);
                return;
            }
            if (pointerErrors != "")
            { ShowError("Some pointers failed to load. Yell at Burnt for making typos.\n" + pointerErrors); }


            // Check if current version of HCM is obsolete
            if (dataPointersService.ObsoleteHCMVersions.Contains(CurrentHCMVersion))
            {
                //Tell the user this version of HCM is deprecated and the new version must be downloaded

                MessageBox.Show("Your version of HCM is obsolete, please update from the releases page. Shutting down.", "HaloCheckpointManager Error", MessageBoxButton.OK, MessageBoxImage.Warning);

                //Take them to the release page
                System.Diagnostics.Process.Start(new ProcessStartInfo
                {
                    FileName = "https://github.com/Burnt-o/HaloCheckpointManager/releases",
                    UseShellExecute = true
                });
                System.Windows.Application.Current.Shutdown();
                return;
            }

            // Check if there's a newer version of HCM available
            Log.Debug($"dataPointersService.LatestHCMVersions.Count: {dataPointersService.LatestHCMVersions.Count}");
            Log.Debug($"dataPointersService.LatestHCMVersions.Contains(this.CurrentHCMVersion): {dataPointersService.LatestHCMVersions.Contains(CurrentHCMVersion)}");
            Log.Debug($"this.CurrentHCMVersion: {CurrentHCMVersion}");
            if (dataPointersService.LatestHCMVersions.Count > 0 && !dataPointersService.LatestHCMVersions.Contains(CurrentHCMVersion))
            {
                //Tell the user a new HCM version exists and ask them if they would like to download it (send them to github release page)
                if (!(MessageBox.Show("A newer version of HCM exists, probably with bugfixes or new features.\nWould you like to go to the HCM releases page now?", "Download HCM update?", MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.No))
                {
                    //User clicked yes, so take them to the release page
                    System.Diagnostics.Process.Start(new ProcessStartInfo
                    {
                        FileName = "https://github.com/Burnt-o/HaloCheckpointManager/releases",
                        UseShellExecute = true
                    });
                }
            }

            try
            {
                CheckRequiredFoldersAndFiles();
            }
            catch (Exception ex)
            {
                ShowErrorAndShutdown(ex.Data.Contains("UserMessage") ? ex.Data["UserMessage"].ToString() : ex.ToString());
                return;
            }


            // Setup theming
            string appTheme = HCMExternal.Properties.Settings.Default.DarkMode ? "Dark" : "Light";
            Resources.MergedDictionaries[0].Source = new Uri($"/Themes/{appTheme}.xaml", UriKind.Relative);

            MainWindow? mainWindow = _serviceProvider.GetService<MainWindow>();
            mainWindow.DataContext = _serviceProvider.GetService<MainViewModel>();
            mainWindow.Title = "HaloCheckpointManager " + (CurrentHCMVersion.Length > 4 ? CurrentHCMVersion.Substring(0, 5) : CurrentHCMVersion);
            mainWindow.Show();

           var ips = _serviceProvider.GetService<InterprocService>();
            _serviceProvider.GetService<MCCHookService>();

            ips.initInterproc();

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
            @"Saves",
            @"Saves\Halo 1",
            @"Saves\Halo 2",
            @"Saves\Halo 3",
            @"Saves\Halo 3 ODST",
            @"Saves\Halo Reach",
            @"Saves\Halo 4",
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
