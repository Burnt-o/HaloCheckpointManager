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

        public SaveFolder? SelectedSaveFolder { get; set; }

        public Checkpoint? SelectedCheckpoint { get; set; }


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


        public void SwapLastWriteTimes(Checkpoint source, Checkpoint target)
        {
            DateTime? sourceLWT = source.ModifiedOn;
            DateTime? targetLWT = target.ModifiedOn;

            FileInfo? sourceFI = new(SelectedSaveFolder?.SaveFolderPath + @"\\" + source.CheckpointName + ".bin");
            FileInfo? targetFI = new(SelectedSaveFolder?.SaveFolderPath + @"\\" + target.CheckpointName + ".bin");

            Trace.WriteLine("Source: " + sourceFI.FullName);
            Trace.WriteLine("Target: " + targetFI.FullName);
            if (sourceFI != null && targetFI != null && File.Exists(sourceFI.FullName) && File.Exists(targetFI.FullName) && sourceLWT.HasValue && targetLWT.HasValue)
            {
                Trace.WriteLine("SWAPPING");
                sourceFI.LastWriteTime = (DateTime)targetLWT;
                targetFI.LastWriteTime = (DateTime)sourceLWT;
                RefreshCheckpointList();
            }

            


        }
        

    }




   

}
