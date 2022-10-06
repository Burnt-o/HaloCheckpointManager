using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using HCM3.Model;
using System.IO;
using System.Collections.ObjectModel;

namespace HCM3.Model.CheckpointModels
{
    internal sealed partial class CheckpointModel
    {
        public void RefreshSaveFolderTree()
        {
            int selectedTabIndex = MainModel.SelectedTabIndex;
            if (selectedTabIndex == 6) // 6 is Settings tab, so no need to refresh the savefoldertree. 
            {
                Trace.WriteLine("RefreshSaveFolderTree: tabindex is 6 so not refreshing save folder tree");
                return;
            }

            // Get the "root" save folder for the game
            string? rootSaveFolderName = Dictionaries.TabIndexToRootFolderPath.GetValueOrDefault(selectedTabIndex);
            string? rootSaveFolderPath = @"Saves\" + rootSaveFolderName;
            Trace.WriteLine("RefreshSaveFolderTree: rootsavefolderpath = " + rootSaveFolderPath);
            // Clear the save folder hierarchy. We're about to re-populate it.
            SaveFolderHierarchy.Clear();

            // Since we force-created the root folders on HCM start, the only way this can happen is if the user deleted the root folder *after* booting HCM.
            if (!Directory.Exists(rootSaveFolderPath))
            {
                //TODO: need to put up a messagebox yelling at the user not to delete the root folder. Naughty user.
                Trace.WriteLine("Somehow the rootSaveFolder didn't exist");
                return;
            }

            // Recursive function that creates nested subfolder SaveFolder objects and turns it all into a Root SaveFolder
            DirectoryInfo rootFolderInfo = new DirectoryInfo(rootSaveFolderPath);
            SaveFolder RootFolder = GetAllFoldersUnder(rootFolderInfo);

            SaveFolderHierarchy.Add(RootFolder);


            // The recursive function that gets nested subfolders
            SaveFolder GetAllFoldersUnder(DirectoryInfo folderInfo)
            {
                //Trace.WriteLine("GetAllFoldersUnder hit, " + folderInfo.Name);
                List<DirectoryInfo> subFolderInfo = new(folderInfo.EnumerateDirectories());
                List<SaveFolder> subFolders = new();
                foreach (DirectoryInfo subFolderInfoNested in subFolderInfo)
                {
                    // Recursion woo
                    subFolders.Add(GetAllFoldersUnder(subFolderInfoNested));
                }
                string folderName = folderInfo.Name;
                string folderPath = folderInfo.FullName;
                //Convert List to OC
                ObservableCollection<SaveFolder> subFoldersOC = new(subFolders);
                return new SaveFolder(folderPath, folderName, subFoldersOC);
            }

        }



    }
}
