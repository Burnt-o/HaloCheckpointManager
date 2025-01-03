using Serilog;
using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Input;

namespace HCMExternal.ViewModels.Commands
{

    public class RelayCommand : ICommand
    {
        private readonly Action<object?> execute;
        private readonly Func<object?, bool> canExecute;


        public event EventHandler? CanExecuteChanged
        {
            add => CommandManager.RequerySuggested += value;
            remove => CommandManager.RequerySuggested -= value;
        }

        public RelayCommand(Action<object?> execute, Func<object?, bool> canExecute = null)
        {
            this.execute = execute;
            this.canExecute = canExecute;
        }

        public bool CanExecute(object? parameter)
        {
            return canExecute == null || canExecute(parameter);
        }

        public async void Execute(object? parameter)
        {

            Log.Verbose($"Relay Command Thread : {Thread.CurrentThread.ManagedThreadId} Started");

            try
            {
                Task task = Task.Factory.StartNew(() => { execute(parameter); });
                await task;
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to execute action! \n" + ex.ToString(), "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            }
        }
    }
}
