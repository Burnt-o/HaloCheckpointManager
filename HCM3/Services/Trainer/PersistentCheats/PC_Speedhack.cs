using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Diagnostics;
using BurntMemory;
using Keystone;
using HCM3.Helpers;



namespace HCM3.Services.Trainer
{
    //TODO remove partial
    public partial class PC_Speedhack : IPersistentCheat
    {


        public PC_Speedhack(HaloMemoryService haloMemoryService, DataPointersService dataPointersService, CommonServices commonServices, InternalServices internalServices)
        {
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
            this.CommonServices = commonServices;
            this.InternalServices = internalServices;

            IsChecked = false;
            SpeedString = "10";
        }

        public HaloMemoryService HaloMemoryService { get; init; }

        public PersistentCheatService PersistentCheatService { get; set; }
        public DataPointersService DataPointersService { get; init; }

        public CommonServices CommonServices { get; init; }
        public InternalServices InternalServices { get; init; }



        private bool _isChecked;
        public bool IsChecked
        {
            get
            {
                return _isChecked;
            }
            set
            {
                _isChecked = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsChecked)));
            }
        }

        private string _speedString;
        public string SpeedString
        {
            get { return _speedString; }
            set 
            { 
            _speedString = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(SpeedString)));
            }

        }

        public void ApplySpeed()
        {
            if (!IsChecked)
            {
                ToggleCheat();
            }
            else // cheat is already on, just need to apply new speed value
            {

                double test;
                try
                {
                    test = Convert.ToDouble(SpeedString);
                    if (test == 1.00)
                    {
                        ToggleCheat();
                            return;
                    }; // If speedhack value is 1.00 then might as well leave it turned off.
                }
                catch
                {
                    // will catch again in applycheat
                }

                if (!this.InternalServices.CheckInternalTextDisplaying())
                {
                    IsChecked = false;
                    RemoveCheat();
                    throw new Exception("Couldn't update internal DLL with cheat info");
                }
                try
                {
                    ApplyCheat();
                }
                catch (Exception ex)
                {
                    ex.Message.Insert(0, "Failed to update Speedhack! ");
                    IsChecked = IsCheatApplied();
                    throw;
                }
                IsChecked = IsCheatApplied();
            }

        }


        public event PropertyChangedEventHandler? PropertyChanged;
        public void ToggleCheat()
        {
            Trace.WriteLine("User commanded ToggleSpeedhack !!!!!!!!!!!!!!!!!!!!");
            if (IsChecked)
            {
                Trace.WriteLine("turning speedhack off");
                RemoveCheat();

                IsChecked = IsCheatApplied();
            }
            else
            {
                double test;
                try
                {
                    test = Convert.ToDouble(SpeedString);
                    if (test == 1.00)
                    { 
                    
                    }; // If speedhack value is 1.00 then might as well leave it turned off.
                }
                catch
                { 
                // will catch again in applycheat
                }


                    Trace.WriteLine("turning speedhack on");

                    // Setting IsChecked to true will tell internal DLL to display text
                    IsChecked = true;
                    if (!this.InternalServices.CheckInternalTextDisplaying())
                    {
                    RemoveCheat();
                    IsChecked = IsCheatApplied();
                    throw new Exception("Couldn't update internal DLL with cheat info");
                    }

                    try
                    {
                        ApplyCheat();
                    }
                    catch (Exception ex)
                    {
                    IsChecked = IsCheatApplied();
                    ex.Message.Insert(0, "Failed to enabled Speedhack! ");
                        throw;
                    }

                IsChecked = IsCheatApplied();
               
            }
        }


        public void RemoveCheat()
        {
            this.InternalServices.SetSpeedHackInternal((double)1);
        }

        public bool IsCheatApplied()
        {
            try
            {
                return this.InternalServices.GetSpeedHackInternal();
            }
            catch
            {
                return true;
            }
            
        }





        public bool ApplyCheat()
        {
            double speed;
            try
            {
                speed = Convert.ToDouble(SpeedString);

            }
            catch (Exception ex)
            {
                throw new Exception("Invalid input for speedhack! " + ex.Message); // I don't care about stacktrace being ruined lol
            }
            
            this.InternalServices.SetSpeedHackInternal(speed);
            return IsCheatApplied();



        }


    }
}
