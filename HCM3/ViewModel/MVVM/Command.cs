using System;
using System.Windows.Input;

namespace HCM3.ViewModel.MVVM
{
    internal sealed class Command : ICommand
    {
        private readonly Action<object?> _action;

        public Command(Action<object?> action) => _action = action;

        public void Execute(object? parameter) => _action(parameter);

        public bool CanExecute(object? parameter) => true;

        // https://stackoverflow.com/a/6426414
        // TODO: need to manually raise InvalidateRequerySuggested on HaloState changed
        public event EventHandler? CanExecuteChanged
        {
            add
            {
                CommandManager.RequerySuggested += value;
            }
            remove
            {
                CommandManager.RequerySuggested -= value;
            }
        }
    }
}
