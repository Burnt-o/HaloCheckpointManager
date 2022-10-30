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
    public partial class PC_BlockCPs : IPersistentCheat
    {


        public PC_BlockCPs(HaloMemoryService haloMemoryService, DataPointersService dataPointersService, CommonServices commonServices, InternalServices internalServices)
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
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsChecked)));
            }
        }

        public event PropertyChangedEventHandler? PropertyChanged;
        public void ToggleCheat() 
        {
            Trace.WriteLine("User commanded BlockCPs !!!!!!!!!!!!!!!!!!!!");
            if (IsChecked)
            {
                Trace.WriteLine("turning BlockCPs off");
                RemoveCheat();
                // IsChecked will only be set to false (and thus internal DLL updated) if removeCheat didn't throw
                IsChecked = false;
                
            }
            else
            {
                Trace.WriteLine("turning BlockCPs on");

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
                    ex.Message.Insert(0, "Failed to enabled BlockCPs! ");
                    IsChecked = false;
                    throw;
                }
                
                if (!IsCheatApplied())
                {
                    try { RemoveCheat(); } catch { }
                    IsChecked = false;
                    throw new Exception("Something went wrong and BlockCPs wasn't applied properly; gamestate may be corrupt.");
                    
                }
            }
        }


        public void RemoveCheat()
        {
            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

            ReadWrite.Pointer? codePointer = (ReadWrite.Pointer?)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_NaturalCheckpointCode");
            byte? actualInstruction = this.HaloMemoryService.ReadWrite.ReadByte(codePointer);

            if (actualInstruction == 0)
            {
                this.HaloMemoryService.ReadWrite.WriteByte(codePointer, (byte)1, true);
            }



          
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
            ReadWrite.Pointer codePointer;

            try
            {
                codePointer = (ReadWrite.Pointer)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_NaturalCheckpointCode");
            }
            catch
            {
                Trace.WriteLine("Didn't have required pointers so cheat can't be active");
                return false;
            }

            try
            {
                byte? actualInstruction = this.HaloMemoryService.ReadWrite.ReadByte(codePointer);
                if (actualInstruction == 0)
                {

                    ulong? testInstruction = this.HaloMemoryService.ReadWrite.ReadQword(codePointer - 9);
                    if (testInstruction != null && testInstruction != 0)
                    {
                        
                        return true;
                    }

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

                ReadWrite.Pointer? codePointer = (ReadWrite.Pointer?)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_NaturalCheckpointCode");
                byte? actualInstruction = this.HaloMemoryService.ReadWrite.ReadByte(codePointer);

                if (actualInstruction == 1)
                {
                    this.HaloMemoryService.ReadWrite.WriteByte(codePointer, (byte)0, true);
                    return true;
                }

                return false;
            
            }
            catch (Exception ex)
            {
                Trace.WriteLine("Failed to enable BlockCPs! \n" + ex.Message);
                System.Windows.MessageBox.Show("Failed to enable BlockCPs! \n" + ex.Message, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
                return false;
            }



        }

       
    }
}
