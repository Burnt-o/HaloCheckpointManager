using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Windows.Input;
using System.Threading;

namespace HCM3.Commands
{
    public partial class Commands
    {
        public ICommand H1CheckpointProp { get; init; }
        public class H1Checkpoint : ICommand
        {
            private Commands _commands;
            public H1Checkpoint(Commands commands)
            { 
            _commands = commands;
            }

            private string[] _requiredPointers =
            {
                "H1Checkpoint",
            };


            public bool CanExecute(object? parameter)
            {
                //return (_commands.HavePointers(_requiredPointers, _commands._HCMTasks.HaloState.CurrentMCCVersion));
                return true;
            }

            public void Execute(object? parameter)
            {
                Trace.WriteLine("EEEEEEEEE");
                //need to setup async tasks here
                //_commands._HCMTasks.H1Checkpoint();
                
            }


        #region Generic ICommand stuff
       

        public event EventHandler? CanExecuteChanged
            {
                add { CommandManager.RequerySuggested += value; }
                remove { CommandManager.RequerySuggested -= value; }
            }

        } 
        #endregion

    }
}
