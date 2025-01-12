using HCMExternal.Services.Hotkeys;
using HCMExternal.ViewModels.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCMExternal.ViewModels
{
    public class ToggleViewModel : Presenter
    {
        public string EffectText { get; init; }


        private bool _toggleValue { get; set; }
        public bool ToggleValue
        {
            get => _toggleValue;
            set
            {
                _toggleValue = value;
                OnPropertyChanged(nameof(_toggleValue)); // update ui
            }
        }

        public ToggleViewModel(bool initialValue, string effectText)
        {
            _toggleValue = initialValue;
            EffectText = effectText;

        }

    }
}
