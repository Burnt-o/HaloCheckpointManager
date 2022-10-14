using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Models;
using System.IO;
using System.Diagnostics;

namespace HCM3.Services
{
    public partial class CheckpointServices
    {

        public void BatchModSaveFolderCreatedOns(List<SaveFolder> listSaveFolders, List<DateTime?> listCreatedOns)
        {
            for (int i = 0; i < listSaveFolders.Count; i++)
            {
                string saveFolderPath = listSaveFolders[i].SaveFolderPath;
                DirectoryInfo saveFolderInfo = new(saveFolderPath);
                if (saveFolderInfo.Exists && listCreatedOns[i] != null)
                {
                    saveFolderInfo.CreationTime = (DateTime)listCreatedOns[i];
                }
                else
                {
                    Trace.WriteLine("Something went wrong modifying SaveFolderCreatedOns; path: " + saveFolderPath);
                }
            }
        }
    }
}
