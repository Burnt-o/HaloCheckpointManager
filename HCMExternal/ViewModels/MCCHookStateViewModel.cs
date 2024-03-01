using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using HCMExternal.Models;
using HCMExternal.Services.MCCHookService;
using HCMExternal.ViewModels.Commands;
using Serilog;

namespace HCMExternal.ViewModels
{

   public class MCCHookStateViewModel 
    {
        // store of current state. Updated by MCCHookService.
        public MCCHookState State { get; set; } = new MCCHookState();


        // Subscribed to by MCCHookService which displays a simple winforms dialog box
        public event Action ShowErrorEvent = delegate { Log.Information("Firing ShowErrorEvent"); };

        private ICommand? _showErrorCommand;
        public ICommand ShowErrorCommand
        {
            get { return _showErrorCommand ??= new RelayCommand(o=> { ShowErrorEvent(); }); }
            set { _showErrorCommand = value; }
        }



    }

}
