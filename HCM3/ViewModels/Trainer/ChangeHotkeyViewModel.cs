using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using System.Windows.Input;
using HCM3.ViewModels.Commands;
using HCM3.Services.Trainer;
using System.Windows;
using HCM3.Views.Controls.Trainer.ButtonViews.ButtonOptions;
using System.Diagnostics;
using NonInvasiveKeyboardHookLibrary;

namespace HCM3.ViewModels
{
    internal class ChangeHotkeyViewModel : Presenter
    {



        public ChangeHotkeyViewModel()
        {
            //parameterless constructor for design view

        }

        public ICommand ApplyCommand { get; init; }
        public ICommand ClearCommand { get; init; }
        public ICommand CancelCommand { get; init; }


        public ChangeHotkeyViewModel(KeyboardHookManager keyboardHookManager)
        {



        }





    }
}
