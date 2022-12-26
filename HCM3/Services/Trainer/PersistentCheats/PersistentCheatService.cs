using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using BurntMemory;
using HCM3.Helpers;

namespace HCM3.Services.Trainer
{
    public partial class PersistentCheatService
    {
        public PC_Invulnerability PC_Invulnerability { get; init; }
        public PC_Speedhack PC_Speedhack { get; init; }

        public PC_BlockCPs PC_BlockCPs { get; init; }
        public PC_Medusa PC_Medusa { get; init; }
        public PC_OneHitKill PC_OneHitKill { get; init;  }
        public PC_DisplayInfo PC_DisplayInfo { get; init; }


        Dictionary<string, IPersistentCheat> listOfCheats { get; set; }

        public InternalServices InternalServices { get; init; }
        public HaloMemoryService HaloMemoryService { get; init; }

        public PersistentCheatService(InternalServices internalServices, HaloMemoryService haloMemoryService, PC_Invulnerability pC_Invulnerability, PC_Speedhack pC_Speedhack, PC_BlockCPs pC_BlockCPs, PC_Medusa pC_medusa, PC_OneHitKill pC_OneHitKill, PC_DisplayInfo pC_DisplayInfo)
        {
            this.PC_Invulnerability = pC_Invulnerability;
            this.PC_Speedhack = pC_Speedhack;
            this.PC_BlockCPs = pC_BlockCPs;
            this.PC_Medusa = pC_medusa;
            this.PC_OneHitKill = pC_OneHitKill;
            this.PC_DisplayInfo = pC_DisplayInfo;

            this.InternalServices = internalServices;
            
            PC_Invulnerability.PersistentCheatService = (this);
            PC_OneHitKill.PersistentCheatService = (this);
            PC_DisplayInfo.PersistentCheatService =(this);

            //TODO: clean up above

            this.HaloMemoryService = haloMemoryService;

            listOfCheats = new();
            listOfCheats.Add("Invulnerability", PC_Invulnerability);
            listOfCheats.Add("Speedhack", PC_Speedhack);
            listOfCheats.Add("Block Checkpoints", PC_BlockCPs);
            listOfCheats.Add("Medusa", PC_Medusa);
            listOfCheats.Add("Display Player Data", PC_DisplayInfo);

            foreach (IPersistentCheat cheat in listOfCheats.Values)
            { 
                //subscribe to every cheats PropertyChanged (raised by the IsChecked bool) so we can tell the internal dll
            cheat.PropertyChanged += CheatStateChanged;
            }

            //Subscribe to halostatechanged so we can update which cheats are still enabled or not
            //HaloStateEvents.HALOSTATECHANGED_EVENT += CheckCheats;

            // And also a timer
         System.Timers.Timer UpdateCurrentHaloStateTimer = new System.Timers.Timer();
            UpdateCurrentHaloStateTimer.Elapsed += new System.Timers.ElapsedEventHandler(this.CheckCheats);
            UpdateCurrentHaloStateTimer.Interval = 3000;
            UpdateCurrentHaloStateTimer.Enabled = true;

            

        }



        private void CheckCheats(object? sender, EventArgs? e)
        {
            Trace.WriteLine("Checking Persistent Cheats");
            int currentHaloState = this.HaloMemoryService.HaloState.CurrentHaloState;
            if (currentHaloState == (int)Dictionaries.HaloStateEnum.Unattached) return;

            foreach (KeyValuePair<string, IPersistentCheat> kv in listOfCheats)
            {
                    kv.Value.IsChecked = kv.Value.IsCheatApplied();
                   
            }
        }


        private void CheatStateChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            try { UpdateInternalDisplayWithActiveCheats();
                this.HaloMemoryService.HaloState.OverlayHooked = true;
            }
            catch (Exception ex) 
            {
                this.HaloMemoryService.HaloState.OverlayHooked = false;
                Trace.WriteLine("Something went wrong trying to update the internal display when " + 
                    sender?.ToString() + " changed state, ex: " + ex.Message + "\n" + ex.StackTrace); 
            }


        }


        public void RemoveAllCheats()
        {
            foreach (KeyValuePair<string, IPersistentCheat> kv in listOfCheats)
            {
                if (kv.Value.IsChecked)
                {
                    try
                    {
                        kv.Value.RemoveCheat();
                    }
                    catch (Exception ex){ Trace.WriteLine("Shutdown error removing cheat: " + kv.Key);  }

                }
            }

            this.CheckCheats(this, EventArgs.Empty);
        }

        public bool AnyCheatsEnabled()
        {
            if (listOfCheats.Count == 0) return false;

            foreach (IPersistentCheat cheat in listOfCheats.Values)
            {
                if (cheat.IsChecked == true) return true;
            }
            return false;
        }

        public void UpdateInternalDisplayWithActiveCheats()
        {
            

            Trace.WriteLine("Updating internal display with active cheats");
            if (this.InternalServices.CheckInternalLoaded() == false || this.InternalServices.InternalFunctionsLoaded == false || !this.HaloMemoryService.HaloState.OverlayHooked)
            {
                if (!this.InternalServices.InjectInternal()) throw new Exception("Internal dll wasn't loaded, then failed injecting internal dll");
            }

            Trace.WriteLine("Checking if listOfCheats is valid");
            if (listOfCheats == null) throw new Exception("listOfCheats was null");
            if (!listOfCheats.Any()) throw new Exception("listOfCheats wasn't null but had no entries");

            Trace.WriteLine("Iterating over antive cheats");
            List<string> activeCheats = new List<string>();
            foreach (KeyValuePair<string, IPersistentCheat> entry in listOfCheats)
            {
                if (entry.Value.IsChecked == true)
                {
                    activeCheats.Add((string)entry.Key);
                }
            }

            Trace.WriteLine("ActiveCheats count: " + activeCheats.Count);

            string activeCheatsString;
            if (activeCheats.Count == 0)
            {
                activeCheatsString = "HCM";
            }
            else if (activeCheats.Count == 1)
            {
                activeCheatsString = "HCM Active Cheats: " + activeCheats.First();
            }
            else
            {
                activeCheatsString = "HCM Active Cheats: " + string.Join(", ", activeCheats);
            }

                this.InternalServices.CallInternalFunction("ChangeDisplayText", activeCheatsString);
            Trace.WriteLine("Changed Display text, now checking if text is displaying");

            if (!this.InternalServices.CheckInternalTextDisplaying())
            {
                throw new Exception("CheckInternalTextDisplaying failed");
            }

            
        }

        public void RemoveInternalText()
        {
            if (this.InternalServices.CheckInternalLoaded() == true && this.InternalServices.InternalFunctionsLoaded == true)
            {
                this.InternalServices.CallInternalFunction("ChangeDisplayText", " ");
            }
        }






    }



    

}
