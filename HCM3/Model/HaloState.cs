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
            _currentHaloState = (int)HaloStateEnum.Unattached;
            MainModel = mainModel;

        }

        internal MainModel MainModel { get; init; }

        public enum HaloStateEnum
        {
            Unattached = -2,
            Menu = -1,
            Halo1 = 0,
            Halo2 = 1,
            Halo3 = 2,
            Halo3ODST = 5,
            Halo4 = 3,
            HaloReach = 6,
            Halo2MP = 4,


        }


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
                CurrentHaloState = (int)HaloStateEnum.Unattached;
                return;
            }

            ReadWrite.Pointer? gameIndicatorPointer = MainModel.PointerCollection.GetPointer("MCC_GameIndicator", MainModel.CurrentAttachedMCCVersion);

            if (gameIndicatorPointer == null)
            {
                CurrentHaloState = (int)HaloStateEnum.Unattached;
            }
        }


    }
}

