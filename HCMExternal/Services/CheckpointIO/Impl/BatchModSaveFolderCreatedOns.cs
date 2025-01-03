using HCMExternal.Models;
using Serilog;
using System;
using System.Collections.Generic;
using System.IO;

namespace HCMExternal.Services.CheckpointIO.Impl
{
    public partial class CheckpointIOService
    {
        /// <summary>
        /// Rewrites the createdOnTimes dates of a list of save folders according to a specified order. CreatedOnTimes are used to order saveFolders with a shared parent.
        /// </summary>
        /// <param name="listSaveFolders">List (in order) of saveFolders (with a shared parent) whose createdOnTimes will be modified.</param>
        /// <param name="listCreatedOns">List (in order) of the new createdOnTimes.</param>
        public void BatchModSaveFolderCreatedOns(List<SaveFolder> listSaveFolders, List<DateTime?> listCreatedOns)
        {

            for (int i = 0; i < listSaveFolders.Count; i++)
            {
                string saveFolderPath = listSaveFolders[i].SaveFolderPath;
                DirectoryInfo saveFolderInfo = new(saveFolderPath);
                if (!saveFolderInfo.Exists)
                {
                    Log.Error("BatchModSaveFolderCreatedOns failed, dir didn't exist at path: " + saveFolderPath);
                }
                else if (listCreatedOns[i] == null)
                {
                    Log.Error("BatchModSaveFolderCreatedOns failed, listCreatedOns[i] was null at index " + i);
                }
                else
                {
                    saveFolderInfo.CreationTime = listCreatedOns[i].Value;

                }
            }
        }
    }
}
