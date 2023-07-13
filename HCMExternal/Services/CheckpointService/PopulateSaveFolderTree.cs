using System;
using System.Collections.Generic;
using System.Linq;
using System.Diagnostics;
using System.IO;
using System.Collections.ObjectModel;
using HCMExternal.Models;
using HCMExternal.Helpers.DictionariesNS;
using Serilog;

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
        public ObservableCollection<SaveFolder> PopulateSaveFolderTree(out SaveFolder? RootFolder, HaloTabEnum SelectedGame)
        {
            RootFolder = null;
            ObservableCollection<SaveFolder> saveFolderCollection = new();


            // Get the "root" save folder for the game
            string? rootSaveFolderName = Dictionaries.GameToRootFolderPath.GetValueOrDefault(SelectedGame);
            string? rootSaveFolderPath = @"Saves\" + rootSaveFolderName;
            Log.Verbose("RefreshSaveFolderTree: rootsavefolderpath = " + rootSaveFolderPath);

            // Since we force-created the root folders on HCM start, the only way this can happen is if the user deleted the root folder *after* booting HCM.
            if (!Directory.Exists(rootSaveFolderPath))
            {
                //TODO: need to put up a messagebox yelling at the user not to delete the root folder. Naughty user.
                Log.Error("Somehow the rootSaveFolder didn't exist");
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
                    subFolders.Add(GetAllFoldersUnder(subFolderInfoNested, thisPath) );
                }
                string folderName = folderInfo.Name;
                string folderPath = folderInfo.FullName;
                DateTime? createdDateTime = folderInfo.CreationTime;
                //Convert List to OC
                ObservableCollection<SaveFolder> subFoldersOC = new(subFolders);
                subFoldersOC = new (subFoldersOC.OrderBy(c => c.CreatedOn));
                return new SaveFolder(folderPath, folderName, subFoldersOC, parentPath, createdDateTime);
            }

        }



    }
}
