using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using HCM3.Services;
using HCM3.Services.Trainer;
using Microsoft.Extensions.DependencyInjection;
namespace HCM3
{
    public partial class App : Application
    {
        private void Application_Exit(object? sender, ExitEventArgs? e)
        {
            this.Logger.Flush();

            var PersistentCheatService = _serviceProvider.GetService<PersistentCheatService>();
            PersistentCheatService.RemoveAllCheats();

            var TrainerServices = _serviceProvider.GetService<TrainerServices>();
            TrainerServices.RemoveAllPatches();
        }
    }
}
