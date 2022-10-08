using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using System.Diagnostics;

namespace HCM3.Model
{
    public class HaloState : BurntMemory.AttachState
    {


        internal HaloState(MainModel mainModel)
        {
            _currentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
            MainModel = mainModel;

            // Need to subscribe to AttachStates onAttach/onDetach events to update Halo State
            HaloStateEvents.ATTACH_EVENT += HaloStateEvents_ATTACH_EVENT;
            HaloStateEvents.DEATTACH_EVENT += HaloStateEvents_DEATTACH_EVENT;
        }

        private void HaloStateEvents_DEATTACH_EVENT(object? sender, EventArgs e)
        {
            MCCType = null;
            UpdateHaloState();
        }

        private void HaloStateEvents_ATTACH_EVENT(object? sender, Events.AttachedEventArgs e)
        {
            Trace.WriteLine("name of attached processsssss: " + nameOfAttachedProcess);
            if (nameOfAttachedProcess == "MCC-Win64-Shipping")
            {
                MCCType = "Steam";
            }
            else
            {
                MCCType = "WinStore";
            }
            UpdateHaloState();
        }

        internal MainModel MainModel { get; init; }

        public string? MCCType { get; set; }


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

        public void UpdateHaloState()
        {
            // If not attached.
            if (!this.Attached || MCCType == null)
            {
                Trace.WriteLine("1");
                CurrentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
                return;
            }

            
            ReadWrite.Pointer? gameIndicatorPointer = MainModel.DataPointers.GetPointer($"MCC_GameIndicator_{MCCType}", MainModel.CurrentAttachedMCCVersion);
            ReadWrite.Pointer? stateIndicatorPointer = MainModel.DataPointers.GetPointer($"MCC_StateIndicator_{MCCType}", MainModel.CurrentAttachedMCCVersion);
            ReadWrite.Pointer? menuIndicatorPointer = MainModel.DataPointers.GetPointer($"MCC_MenuIndicator_{MCCType}", MainModel.CurrentAttachedMCCVersion);
            if (gameIndicatorPointer == null || stateIndicatorPointer == null || menuIndicatorPointer == null)
            {
                CurrentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
            }

            byte? gameIndicator = MainModel.HaloMemory.ReadWrite.ReadByte(gameIndicatorPointer);
            byte? stateIndicator = MainModel.HaloMemory.ReadWrite.ReadByte(stateIndicatorPointer);
            byte? menuIndicator = MainModel.HaloMemory.ReadWrite.ReadByte(menuIndicatorPointer);

            if (menuIndicator != 07 && stateIndicator != 44)
            {
                CurrentHaloState = (int)Dictionaries.HaloStateEnum.Menu;
            }
            else
            {
                switch (gameIndicator)
                {
                    case 0:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo1;
                        break;

                    case 1:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo2;
                        break;

                    case 2:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo3;
                        break;

                    case 3:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo4;
                        break;

                    case 4: // I think this is the mp version of h2 (specifically h2a, not h2c)
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo2;
                        break;

                    case 5:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Halo3ODST;
                        break;

                    case 6:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.HaloReach;
                        break;

                    default:
                        CurrentHaloState = (int)Dictionaries.HaloStateEnum.Unattached;
                        break;

                }
                return;
            }

        }


    }
}

