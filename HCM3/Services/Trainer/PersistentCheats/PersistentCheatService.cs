using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using BurntMemory;

namespace HCM3.Services.Trainer
{
    public partial class PersistentCheatService
    {
        public PC_Invulnerability PC_Invulnerability { get; init; }
        public PC_Speedhack PC_Speedhack { get; init; }

        public PC_BlockCPs PC_BlockCPs { get; init; }


        Dictionary<string, IPersistentCheat> listOfCheats { get; set; }

        public InternalServices InternalServices { get; init; }
        public HaloMemoryService HaloMemoryService { get; init; }

        public PersistentCheatService(InternalServices internalServices, HaloMemoryService haloMemoryService, PC_Invulnerability pC_Invulnerability, PC_Speedhack pC_Speedhack, PC_BlockCPs pC_BlockCPs)
        {
            this.PC_Invulnerability = pC_Invulnerability;
            this.PC_Speedhack = pC_Speedhack;
            this.PC_BlockCPs = pC_BlockCPs;

            this.InternalServices = internalServices;
            
            PC_Invulnerability.PersistentCheatService = (this);
            this.HaloMemoryService = haloMemoryService;

            listOfCheats = new();
            listOfCheats.Add("Invulnerability", PC_Invulnerability);
            listOfCheats.Add("Speedhack", PC_Speedhack);
            listOfCheats.Add("Block Checkpoints", PC_BlockCPs);

            foreach (IPersistentCheat cheat in listOfCheats.Values)
            { 
                //subscribe to every cheats PropertyChanged (raised by the IsChecked bool) so we can tell the internal dll
            cheat.PropertyChanged += CheatStateChanged;
            }
        }

        private void CheatStateChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            try { UpdateInternalDisplayWithActiveCheats(); }
            catch (Exception ex) 
            { 
                Trace.WriteLine("Something went wrong trying to update the internal display when " + 
                    sender?.ToString() + " changed state, ex: " + ex.Message + "\n" + ex.StackTrace); 
            }


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
            if (this.InternalServices.CheckInternalLoaded() == false || this.InternalServices.InternalFunctionsLoaded == false )
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
                activeCheatsString = " ";
            }
            else if (activeCheats.Count == 1)
            {
                activeCheatsString = "HCM Active Cheats: " + activeCheats.First();
            }
            else
            {
                activeCheatsString = "HCM Active Cheats: " + string.Join(",", activeCheats);
            }

                this.InternalServices.CallInternalFunction("ChangeDisplayText", activeCheatsString);
            Trace.WriteLine("Changed Display text, now checking if text is displaying");

            if (!this.InternalServices.CheckInternalTextDisplaying())
            {
                throw new Exception("CheckInternalTextDisplaying failed");
            }
        }






    }



    

}
