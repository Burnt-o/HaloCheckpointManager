using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Collections.ObjectModel;
using HCM3.ViewModel;
using System.ComponentModel;


namespace HCM3.Model
{
    internal sealed partial class CheckpointModel
    {
        
        public HaloMemory HaloMemory;
        public ObservableCollection<Checkpoint> CheckpointCollection { get; set; }

        private PointerCollection PointerCollection { get; init; }

        public CheckpointModel(HaloMemory haloMemory, ObservableCollection<Checkpoint> checkpointCollection, PointerCollection pcollection)
        { 
        HaloMemory = haloMemory;
            CheckpointCollection = checkpointCollection;
            PointerCollection = pcollection;
        }
        public void PrintText()
        {
            Trace.WriteLine("Hello from CheckpointModel!");
        }




        

    }




   

}
