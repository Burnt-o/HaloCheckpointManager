using HCMExternal.Models;
using HCMExternal.Services.InterprocServiceNS;
using HCMExternal.ViewModels;

using Serilog;


namespace HCMExternal.Services.MCCHookService
{
    /// <summary>
    /// Subscribes to interprocs state machine status changes, and to interproc error events. 
    /// </summary>
    public class MCCHookService
    {

        // store of current state (in MCCHookStateViewModel)
        private MCCHookState MCCHookState
        {
            get => mMCCHookStateViewModel.State;
            set => mMCCHookStateViewModel.State = value;
        }

        // last injection error
        private string? lastInjectionError;

        // injected service
        private InterprocService mInterprocService { get; init; }
        private MCCHookStateViewModel mMCCHookStateViewModel { get; init; }
        private ErrorDialogViewModel mErrorDialogViewModel { get; init; }



        // Constructor
        public MCCHookService(InterprocService ips, MCCHookStateViewModel vm, ErrorDialogViewModel errorDialogVM)
        {
            mInterprocService = ips;
            mMCCHookStateViewModel = vm;
            vm.ShowErrorEvent += ShowHCMInternalErrorDialog;

            mErrorDialogViewModel = errorDialogVM;


            ips.InterprocError += Ips_InterprocError;
            ips.StateMachineStatusChanged += Ips_StateMachineStatusChanged;


        }

        private void Ips_StateMachineStatusChanged(object? sender, InterprocService.StatusMachineStatusChangedEventArgs e)
        {
            MCCHookState.State = e.state;
        }

        private void Ips_InterprocError(object? sender, InterprocService.InterprocErrorEventArgs e)
        {
            lastInjectionError = e.message;
            ShowHCMInternalErrorDialog();
        }




        // Dialog tells user about the error and askes them if they want to retry injection
        public void ShowHCMInternalErrorDialog()
        {
            Log.Verbose("ShowHCMInternalErrorDialog");
            Log.Error(lastInjectionError ?? "No error info to display, for some reason?!");
            bool result = mErrorDialogViewModel.RaiseShowErrorDialogEvent(lastInjectionError);
            if (result == false)
            {
                // User doesn't want to retry injection, do nothing
            }
            else
            {
                mInterprocService.resetStateMachineEx();
            }
        }







    }
}
