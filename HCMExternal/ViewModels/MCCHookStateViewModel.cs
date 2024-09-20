using HCMExternal.Models;
using HCMExternal.ViewModels.Commands;
using Serilog;
using System;
using System.Windows.Input;

namespace HCMExternal.ViewModels
{

    public class MCCHookStateViewModel
    {
        public ErrorDialogViewModel mErrorDialogViewModel { get; init; }

        public MCCHookStateViewModel(ErrorDialogViewModel evm)
        {
            mErrorDialogViewModel = evm;
        }

        // store of current state. Updated by MCCHookService.
        public MCCHookState State { get; set; } = new MCCHookState();


        // Subscribed to by MCCHookService which in turn Fires the ErrorDialogViewModels event handler
        public event Action ShowErrorEvent = delegate { Log.Information("Firing ShowErrorEvent"); };

        private ICommand? _showErrorCommand;
        public ICommand ShowErrorCommand
        {
            get => _showErrorCommand ??= new RelayCommand(o => { ShowErrorEvent(); });
            set => _showErrorCommand = value;
        }



    }

}
