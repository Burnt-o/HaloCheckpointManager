using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using HCMExternal.Services.MCCStateServiceNS;
using HCMExternal.ViewModels.Commands;
using Serilog;

namespace HCMExternal.ViewModels
{
    public enum StatusBarState
    {
        MCCProcessNotFound,
        InjectionFailed,
        InjectionInProgress,
        InjectionSucceeded
    };

   public class StatusBar : INotifyPropertyChanged
    {
        private StatusBarState _statusBarState = StatusBarState.MCCProcessNotFound;
        public StatusBarState currentState
        {
            get
            {
                return _statusBarState;
            }
            private set
            {
                _statusBarState = value;
                OnPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler? PropertyChanged;
        protected void OnPropertyChanged([CallerMemberName] string? name = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }


        private void showError(object sender)
        {
            Log.Verbose("showError Command fired");
            MCCStateService.FireShowErrorEvent();
        }



        private ICommand _showErrorCommand;
        public ICommand ShowErrorCommand
        {
            get { return _showErrorCommand ?? (_showErrorCommand = new RelayCommand(o=> showError("ShowErrorCommand"))); }
            set { _showErrorCommand = value; }
        }


        public StatusBar()
        {
            MCCStateService.AttachEvent += () => { currentState = StatusBarState.InjectionSucceeded; };
            MCCStateService.AttachInProgressEvent += () => { currentState = StatusBarState.InjectionInProgress; };
            MCCStateService.DetachEvent += () => { currentState = StatusBarState.MCCProcessNotFound; };
            MCCStateService.AttachErrorEvent += () => { currentState = StatusBarState.InjectionFailed; };
        }



    }

}
