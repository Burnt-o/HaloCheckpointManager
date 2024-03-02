using Serilog;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Runtime.CompilerServices;

namespace HCMExternal.ViewModels
{
    public class ErrorDialogViewModel : INotifyPropertyChanged
    {
        private string _errorMessage = "not set";
        public string ErrorMessage
        {
            get { return _errorMessage; }
            set { _errorMessage = value; OnPropertyChanged(nameof(ErrorMessage)); }
        }


        public delegate bool ReturnBoolEventHandler(object sender, EventArgs args);

        public event ReturnBoolEventHandler ShowErrorDialogAndAskRetryEvent;

        public bool RaiseShowErrorDialogEvent(string ? InErrorMessage)
        {
            ErrorMessage = InErrorMessage ?? "null";

            if (ShowErrorDialogAndAskRetryEvent != null)  // make sure at least one subscriber
            {
                Log.Verbose("ShowErrorDialogAndAskRetryEvent");

                return HCMExternal.App.Current.Dispatcher.Invoke((Func<bool>)delegate { // Need to make sure it's run on the UI thread
                    return ShowErrorDialogAndAskRetryEvent(this, new EventArgs()); // note the event is returning a bool
                });
                
            }
            else
            {
                Log.Error("Null ShowErrorDialogAndAskRetryEvent");
                return false;
            }

        }

        public event PropertyChangedEventHandler? PropertyChanged;

        protected void OnPropertyChanged([CallerMemberName] string? name = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }
    }
}
