using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Diagnostics;
using BurntMemory;
using HCM3.Helpers;



namespace HCM3.Services.Trainer
{

    public class PC_Medusa : IPersistentCheat
    {


        public PC_Medusa(HaloMemoryService haloMemoryService, DataPointersService dataPointersService, CommonServices commonServices, InternalServices internalServices)
        {
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
            this.CommonServices = commonServices;
            this.InternalServices = internalServices;

            IsChecked = false;
        }

        public HaloMemoryService HaloMemoryService { get; init; }


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
                System.Windows.Application.Current.Dispatcher.Invoke((Action)delegate {
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsChecked)));
                });
            }
        }

        public event PropertyChangedEventHandler? PropertyChanged;
        public void ToggleCheat() 
        {
            if (!Properties.Settings.Default.DisableOverlay && !this.HaloMemoryService.HaloState.OverlayHooked) throw new Exception("Overlay wasn't hooked");

            Trace.WriteLine("User commanded toggle medusa !!!!!!!!!!!!!!!!!!!!");
            if (IsChecked)
            {
                Trace.WriteLine("turning medusa off");
                RemoveCheat();
                // IsChecked will only be set to false (and thus internal DLL updated) if removeCheat didn't throw
                IsChecked = false;
                
            }
            else
            {
                Trace.WriteLine("turning medusa on");

                // Setting IsChecked to true will tell internal DLL to display text
                IsChecked = true;
                if (!this.InternalServices.CheckInternalTextDisplaying())
                {
                    IsChecked = false;
                    throw new Exception("Couldn't update internal DLL with cheat info");
                }

                try
                {
                    ApplyCheat();
                }
                catch (Exception ex)
                {
                    ex.ToString().Insert(0, "Failed to enabled medusa! ");
                    IsChecked = false;
                    throw;
                }
                
                if (!IsCheatApplied())
                {
                    IsChecked = false;
                    try { RemoveCheat(); } catch { }
                    
                    throw new Exception("Something went wrong and medusa wasn't applied properly; gamestate may be corrupt.");
                    
                }
            }
        }


        public void RemoveCheat()
        {
            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

            ReadWrite.Pointer? medusaPointer = (ReadWrite.Pointer?)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_MedusaFlag");
            byte? medusaValue = this.HaloMemoryService.ReadWrite.ReadByte(medusaPointer);

            if (medusaValue == 1)
            {
                if (Properties.Settings.Default.DisableOverlay) this.CommonServices.PrintMessage("Medusa disabled.");
                this.HaloMemoryService.ReadWrite.WriteByte(medusaPointer, (byte)0, true);
            }

            IsChecked = false;
           

        }

        public bool IsCheatApplied()
        {
            //if (DetourHandle == null) return false; // hook is not applied, otherwise hookASM bytes wouldn't be null

            int loadedGame;
            try
            {
                this.HaloMemoryService.HaloState.UpdateHaloState();
                loadedGame = this.CommonServices.GetLoadedGame();
            }
            catch
            {
                // MCC isn't inside a game right now (or unattached), so cheat probably not loaded.
                // Do we need to add an out var here to tell the internal hack not to change message? no wait I want it to disable if in menu.. but what if hook still active in game DLL then they load back in?
                return false; 
            }

            // Test loaded game only. if loaded game doesn't have pointers then it can't have had cheat anyway
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];
            ReadWrite.Pointer medusaPointer;

            try
            {
                medusaPointer = (ReadWrite.Pointer)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_MedusaFlag");
            }
            catch
            {
                Trace.WriteLine("Didn't have required pointers so cheat can't be active");
                return false;
            }

            try
            {
                byte? medusaValue = this.HaloMemoryService.ReadWrite.ReadByte(medusaPointer);
                if (medusaValue == 1)
                {
                        return true;
                }
                return false;
            }
            catch
            {
                return false;
            }

 

        }


      
        public bool ApplyCheat()
        {
            try
            {
                this.HaloMemoryService.HaloState.UpdateHaloState();
                int loadedGame = this.CommonServices.GetLoadedGame();

                string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

                ReadWrite.Pointer? medusaPointer = (ReadWrite.Pointer?)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_MedusaFlag");
                byte? medusaValue = this.HaloMemoryService.ReadWrite.ReadByte(medusaPointer);

                if (medusaValue == 0)
                {
                    if (Properties.Settings.Default.DisableOverlay) this.CommonServices.PrintMessage("Medusa enabled.");
                    this.HaloMemoryService.ReadWrite.WriteByte(medusaPointer, (byte)1, true);

                    return true;
                }

                return false;
            
            }
            catch (Exception ex)
            {
                Trace.WriteLine("Failed to enable medusa! \n" + ex.ToString());
                System.Windows.MessageBox.Show("Failed to enable medusa! \n" + ex.ToString(), "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
                return false;
            }



        }

       
    }
}
