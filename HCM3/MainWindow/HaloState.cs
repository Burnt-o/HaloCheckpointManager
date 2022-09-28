using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;

namespace HCM3
{
    public class HaloState : BurntMemory.AttachState
    {


        public HaloState()
        {
            _currentHaloState = (int)HaloStateEnum.Unattached;
            _currentMCCVersion = "none";
        }

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

        private string _currentMCCVersion;
        public string CurrentMCCVersion
        {
            get
            { 
            return _currentMCCVersion;
            }
            set
            {
                if (_currentMCCVersion != value)
                { 
                // Need to raise event
                }
            }
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
                // Need to raise event
                }
                _currentHaloState = value;
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
        
        }

    }
}
