using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using System.Diagnostics;


namespace HCM3.Commands
{
    public partial class Commands
    {

        private readonly PointerCollection _pcollection;
        private readonly HCMTasks _HCMTasks;
        public Commands()
        { }

        public Commands(PointerCollection pcollection, HCMTasks hCMTasks)
        { 
            _HCMTasks = hCMTasks;
        _pcollection = pcollection;

            H1CheckpointProp = new H1Checkpoint(this);
        }




        private bool HavePointers(string[] PointerNames, string version)
        {
            if (PointerNames.Any())
            {
                foreach (string pointerName in PointerNames)
                {
                    if (!_pcollection.PointerExists(pointerName, version))
                    {
                        return false;
                    }
                }
            }
            return true;
        }

    }
}
