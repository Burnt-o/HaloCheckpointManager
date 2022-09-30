using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Collections.ObjectModel;


namespace HCM3.Model
{
    internal sealed class Halo1Model
    {
        
        public HaloMemory HaloMemory;
        public ObservableCollection<Checkpoint> CheckpointCollectionH1 { get; set; }
        public Halo1Model(HaloMemory haloMemory, ObservableCollection<Checkpoint> checkpointCollectionH1)
        { 
        HaloMemory = haloMemory;
            CheckpointCollectionH1 = checkpointCollectionH1;
        }
        public void PrintText()
        {
            Trace.WriteLine("Hello from Halo1Model!");
        }
    }
}
