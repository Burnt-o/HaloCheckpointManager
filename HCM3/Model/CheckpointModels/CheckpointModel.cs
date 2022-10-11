using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Collections.ObjectModel;
using HCM3.ViewModel;
using System.ComponentModel;
using System.IO;


namespace HCM3.Model.CheckpointModels
{
    internal sealed partial class CheckpointModel
    {
        public ObservableCollection<Checkpoint> CheckpointCollection { get; set; }
        public ObservableCollection<SaveFolder> SaveFolderHierarchy { get; set; }
        public SaveFolder? RootSaveFolder { get; private set; }

        private CheckpointFileDecoder CheckpointFileDecoder = new();
        public MainModel MainModel { get; init; }

        //public SaveFolder? SelectedSaveFolder { get; set; }

        //public Checkpoint? SelectedCheckpoint { get; set; }


        public CheckpointModel(MainModel mainModel)
        { 
            MainModel = mainModel;
            CheckpointCollection = new();
            SaveFolderHierarchy = new();
        }
        public void PrintText()
        {
            Trace.WriteLine("Hello from CheckpointModel!");
        }


        public void BatchModCheckpointLWTs(List<Checkpoint> listCheckpoints, List<DateTime?> listLWT, SaveFolder? SelectedSaveFolder)
        {
            for (int i = 0; i < listCheckpoints.Count; i++)
            {
                string checkpointPath = SelectedSaveFolder?.SaveFolderPath + "\\" + listCheckpoints[i].CheckpointName + ".bin";
                FileInfo checkpointInfo = new(checkpointPath);
                if (checkpointInfo.Exists && listLWT[i] != null)
                {
                    checkpointInfo.LastWriteTime = (DateTime)listLWT[i];
                }
                else
                {
                    Trace.WriteLine("Something went wrong modifying CheckpointLWTs; path: " + checkpointPath);
                }
            }
        }

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
