using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;

namespace HCM3.Model
{
    public class HaloState : BurntMemory.AttachState
    {


        internal HaloState(MainModel mainModel)
        {
            _currentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
            MainModel = mainModel;

        }

        internal MainModel MainModel { get; init; }




        private int _currentHaloState;
        public int CurrentHaloState
        {
            get
            {
                return _currentHaloState;
            }
            set
            {
                if (_currentHaloState != value)
                {
                    _currentHaloState = value;
                    // Raise event
                    HaloStateEvents.HALOSTATECHANGED_EVENT_INVOKE(this, new HaloStateEvents.HaloStateChangedEventArgs(_currentHaloState));
                }

            }
        }



        public int GetHaloState(bool updateBeforeGet = true)
        {
            if (updateBeforeGet)
            {
                UpdateHaloState();
            }
            return CurrentHaloState;
        }

        private void UpdateHaloState()
        {
            // If not attached.
            if (!this.Attached)
            {
                CurrentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
                return;
            }

            ReadWrite.Pointer? gameIndicatorPointer = MainModel.DataPointers.GetPointer("MCC_GameIndicator", MainModel.CurrentAttachedMCCVersion);

            if (gameIndicatorPointer == null)
            {
                CurrentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
            }
        }


    }
}

