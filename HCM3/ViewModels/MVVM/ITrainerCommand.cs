using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
namespace HCM3.ViewModels.MVVM
{
    public interface ITrainerCommand : ICommand
    {
        //internal void SetActionViewModel(ActionControlViewModel actionControlViewModel);
        public ActionControlViewModel ActionControlViewModel { get; set;}
    }
}
