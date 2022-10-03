using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;

namespace HCM3.Model.CheckpointModels
{
    public class SaveFolder
    {
        public string SaveFolderPath { get; set; }
        public string SaveFolderName { get; set; }

        public ObservableCollection<SaveFolder> Children { get; set; }

        //So uh should the savefolder objects store info about their hierarchy in the tree view?
        public SaveFolder(string saveFolderPath, string saveFolderName, ObservableCollection<SaveFolder> children)
        {
            SaveFolderPath = saveFolderPath;
            SaveFolderName = saveFolderName;
            Children = children;
        }

    }
}
