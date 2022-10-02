using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModel;
using HCM3.Model;
using BurntMemory;

namespace HCM3.Model
{
    internal class MainModel
    {
        public PointerCollection PointerCollection { get; init; }
        public HaloMemory HaloMemory { get; init; }

        public MainModel(PointerCollection pcollection, HaloMemory haloMemory)
        { 
        PointerCollection = pcollection;
        HaloMemory = haloMemory;
        }

    }
}
