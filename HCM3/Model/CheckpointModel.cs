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
        public ObservableCollection<Checkpoint> CheckpointCollection { get; set; }
        public MainModel MainModel { get; init; }

        public CheckpointModel(MainModel mainModel)
        { 
            MainModel = mainModel;
            CheckpointCollection = new();
        }
        public void PrintText()
        {
            Trace.WriteLine("Hello from CheckpointModel!");
        }




        

    }




   

}
