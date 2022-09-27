using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows.Input;


namespace HCM3
{

    public class UIActionProperties
    {
        public UIActionProperties(Action<object?>executeMethod, Func<object?, bool>canExecuteMethod)
        { 
        // Constructor. pass 2 delegates, one for canexecute, one for execute
        _executeMethod = executeMethod;
            _canExecuteMethod = canExecuteMethod;
        }

        private Action<object?> _executeMethod;
        private Func<object?, bool> _canExecuteMethod;

        private ICommand? mCommand;
        public ICommand? Command
        {
            get
            {
                if (mCommand == null)
                    mCommand = new TestCommand(_executeMethod, _canExecuteMethod);
                return mCommand;
            }
            set
            {
                mCommand = value;
            }
        }

        public class TestCommand : ICommand
        {
            private Action<object?> _executeMethod;
            private Func<object?, bool> _canExecuteMethod;
            public TestCommand(Action<object?> executeMethod, Func<object?, bool> canExecuteMethod)
            {
                _executeMethod = executeMethod;
                _canExecuteMethod = canExecuteMethod;
            }

            public bool CanExecute(object? parameter)
            {
                return _canExecuteMethod(parameter);
            }
            public event EventHandler? CanExecuteChanged
            {
                add { CommandManager.RequerySuggested += value; }
                remove { CommandManager.RequerySuggested -= value; }
            }

            public void Execute(object? parameter)
            {
                _executeMethod(parameter);
            }
        }
    }
}
