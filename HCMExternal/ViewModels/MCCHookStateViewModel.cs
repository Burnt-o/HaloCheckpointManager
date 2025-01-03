using HCMExternal.Models;
using HCMExternal.Services.Interproc;
using HCMExternal.ViewModels.Commands;
using Serilog;
using System;
using System.Windows.Input;

namespace HCMExternal.ViewModels
{

    public class MCCHookStateViewModel
    {
        public ErrorDialogViewModel mErrorDialogViewModel { get; init; }

        public MCCHookStateViewModel(ErrorDialogViewModel evm, IInterprocService ips)
        {
            mErrorDialogViewModel = evm;
            ips.StateMachineStatusChangedHandler += (sender, obj) => { State.State = obj.hookState; };
            ips.InterprocErrorHandler += (sender, obj) =>
            {
                evm.ErrorMessage = obj.errorMessage;
                bool dialogResult = evm.RaiseShowErrorDialogEvent(evm.ErrorMessage);

                if (dialogResult) // user wants to retry injection
                    ips.resetStateMachineEx();
            };
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
