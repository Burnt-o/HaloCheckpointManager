using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using HCM3.ViewModels.MVVM;

namespace HCM3.ViewModels.Commands
{

    public class RelayCommand : ICommand
    {
        private Action<object?> execute;
        private Func<object?, bool> canExecute;


        public event EventHandler? CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }
            remove { CommandManager.RequerySuggested -= value; }
        }

        public RelayCommand(Action<object?> execute, Func<object?, bool> canExecute = null)
        {
            this.execute = execute;
            this.canExecute = canExecute;
        }

        public bool CanExecute(object? parameter)
        {
            return this.canExecute == null || this.canExecute(parameter);
        }

        public void Execute(object? parameter)
        {
            try
            {
                this.execute(parameter);
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show("Failed to execute action! \n" + ex.Message, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            }
        }
    }
}
