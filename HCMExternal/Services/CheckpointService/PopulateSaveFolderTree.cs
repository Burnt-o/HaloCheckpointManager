using HCMExternal.Helpers.DictionariesNS;
using HCMExternal.Models;
using Serilog;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Windows;

namespace HCMExternal.Services.CheckpointServiceNS
{
    public partial class CheckpointService
    {

        /// <summary>
        /// Populates the saveFolder tree for a specified game -that's the root Folder and all folders underneath- into the saveFolderCollection observed by CheckpointViewModel.
        /// </summary>
        /// <param name="RootFolder">Out parameter: the root folder for this game.</param>
        /// <param name="SelectedGame">Which game is this for?</param>
        /// <returns></returns>
        public ObservableCollection<SaveFolder> PopulateSaveFolderTree(out SaveFolder RootFolder, HaloTabEnum SelectedGame)
        {
            ObservableCollection<SaveFolder> saveFolderCollection = new();


            // Get the "root" save folder for the game
            string? rootSaveFolderName = Dictionaries.GameToRootFolderPath.GetValueOrDefault(SelectedGame);
            string? rootSaveFolderPath = @"Saves\" + rootSaveFolderName;
            Log.Verbose("RefreshSaveFolderTree: rootsavefolderpath = " + rootSaveFolderPath);

            // Since we force-created the root folders on HCM start, the only way this can happen is if the user deleted the root folder *after* booting HCM.
            if (!Directory.Exists(rootSaveFolderPath))
            {
                //yell at the user not to delete the root folder. Naughty user.
                MessageBox.Show("OH GOD you deleted a root save folder at path " + rootSaveFolderPath + "\nDON'T DO THAT DEAR GOD WHY ARHHHH\n(You need to restart HCM, it'll autocreate them on startup\n(don't do that again))");
                Log.Error("Somehow the rootSaveFolder didn't exist at path " + rootSaveFolderPath);
                RootFolder = new SaveFolder("ERROR: no root folder", "ERROR: no root folder", saveFolderCollection, "ERROR: no root folder", null);
                return saveFolderCollection;
            }

            // Recursive function that creates nested subfolder SaveFolder objects and turns it all into a Root SaveFolder
            DirectoryInfo rootFolderInfo = new DirectoryInfo(rootSaveFolderPath);
            RootFolder = GetAllFoldersUnder(rootFolderInfo, null);
            saveFolderCollection.Add(RootFolder);

            return saveFolderCollection;

            // The recursive function that gets nested subfolders
            SaveFolder GetAllFoldersUnder(DirectoryInfo folderInfo, string? parentPath)
            {
                List<DirectoryInfo> subFolderInfo = new(folderInfo.EnumerateDirectories());
                List<SaveFolder> subFolders = new();
                string thisPath = folderInfo.FullName;
                foreach (DirectoryInfo subFolderInfoNested in subFolderInfo)
                {
                    // Recursion woo
                    subFolders.Add(GetAllFoldersUnder(subFolderInfoNested, thisPath));
                }
                string folderName = folderInfo.Name;
                string folderPath = folderInfo.FullName;
                DateTime? createdDateTime = folderInfo.CreationTime;
                //Convert List to OC
                ObservableCollection<SaveFolder> subFoldersOC = new(subFolders);
                subFoldersOC = new(subFoldersOC.OrderBy(c => c.CreatedOn));
                return new SaveFolder(folderPath, folderName, subFoldersOC, parentPath, createdDateTime);
            }

        }

        public SaveFolder GetRootFolder(HaloTabEnum SelectedGame)
        {
            PopulateSaveFolderTree(out SaveFolder rootFolder, SelectedGame);
            return rootFolder;
        }



    }




}
