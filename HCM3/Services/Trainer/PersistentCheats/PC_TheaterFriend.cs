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

    public class PC_TheaterFriend : IPersistentCheat
    {
        private readonly object TheaterFriendLock = new object();

        public PC_TheaterFriend(HaloMemoryService haloMemoryService, DataPointersService dataPointersService, CommonServices commonServices, InternalServices internalServices)
        {
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
            this.CommonServices = commonServices;
            this.InternalServices = internalServices;

            IsChecked = false;
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
                if (_isChecked != value)
                {
                    _isChecked = value;
                    System.Windows.Application.Current.Dispatcher.Invoke((Action)delegate {
                        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsChecked)));
                    });
                }
            }
        }

        public event PropertyChangedEventHandler? PropertyChanged;
        public void ToggleCheat()
        {
            lock (TheaterFriendLock)
            {
                if (!Properties.Settings.Default.DisableOverlay && !this.HaloMemoryService.HaloState.OverlayHooked) throw new Exception("Overlay wasn't hooked");


                Trace.WriteLine("User commanded ToggleTheaterFriend !!!!!!!!!!!!!!!!!!!!");
                if (IsChecked)
                {
                    Trace.WriteLine("turning TheaterFriend off");
                    RemoveCheat();
                    // IsChecked will only be set to false (and thus internal DLL updated) if removeCheat didn't throw
                    IsChecked = false;
                    // We might want to change this. I don't think there's any situation where removeCheat would fail AND the detour is actually still active ingame. *think* being the operative word.

                }
                else
                {
                    Trace.WriteLine("turning TheaterFriend on");

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
                        ex.ToString().Insert(0, "Failed to enabled TheaterFriend! ");
                        IsChecked = false;
                        throw;
                    }

                    if (!IsCheatApplied())
                    {
                        try { RemoveCheat(); } catch { }
                        IsChecked = false;
                        throw new Exception("Something went wrong and TheaterFriend wasn't applied properly; gamestate may be corrupt.");

                    }
                }
            }
        }


        public void RemoveCheat()
        {
            try
            {
                this.HaloMemoryService.HaloState.UpdateHaloState();
                int loadedGame = this.CommonServices.GetLoadedGame();
                string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

                PatchInfo patchInfoCameraRadius = (PatchInfo)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_TheaterFriend_CameraRadius");
                PatchInfo patchInfoCameraOOB = (PatchInfo)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_TheaterFriend_CameraOOB");
                PatchInfo patchInfoOOBVisuals = (PatchInfo)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_TheaterFriend_OOBVisuals");


                byte[] currentCameraRadiusBytes = this.HaloMemoryService.ReadWrite.ReadBytes(patchInfoCameraRadius.OriginalCodeLocation, patchInfoCameraRadius.OriginalCodeBytes.Length);
                if (currentCameraRadiusBytes != null && currentCameraRadiusBytes.Length == patchInfoCameraRadius.OriginalCodeBytes.Length && currentCameraRadiusBytes.SequenceEqual(patchInfoCameraRadius.PatchedCodeBytes))
                {
                    this.HaloMemoryService.ReadWrite.WriteBytes(patchInfoCameraRadius.OriginalCodeLocation, patchInfoCameraRadius.OriginalCodeBytes, true);
                    this.HaloMemoryService.ReadWrite.WriteBytes(patchInfoCameraOOB.OriginalCodeLocation, patchInfoCameraOOB.OriginalCodeBytes, true);
                    this.HaloMemoryService.ReadWrite.WriteBytes(patchInfoOOBVisuals.OriginalCodeLocation, patchInfoOOBVisuals.OriginalCodeBytes, true);
                }

            }
            catch (Exception e)
            { 
            Trace.WriteLine("Error removing theater friend: " + e.ToString());
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
            PatchInfo patchInfoCameraRadius;
            PatchInfo patchInfoCameraOOB;
            try
            {
                patchInfoCameraRadius = (PatchInfo)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_TheaterFriend_CameraRadius");
                patchInfoCameraOOB = (PatchInfo)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_TheaterFriend_CameraOOB");
            }
            catch
            {
                Trace.WriteLine("Didn't have required pointers so cheat can't be active");
                return false;
            }

            // Just read cameraradius for the test
            byte[] currentCameraRadiusBytes = this.HaloMemoryService.ReadWrite.ReadBytes(patchInfoCameraRadius.OriginalCodeLocation, patchInfoCameraRadius.OriginalCodeBytes.Length);
            Trace.WriteLine("grabbed currentCameraRadiusBytes, length: " + currentCameraRadiusBytes.Length);    
            if (currentCameraRadiusBytes == null || currentCameraRadiusBytes.Length != patchInfoCameraRadius.OriginalCodeBytes.Length) return false; // couldn't read bytes at og code, game not loaded? this shouldn't happen, I think


            for (int i = 0; i < currentCameraRadiusBytes.Length; i++)
            {
                // If the code at hook location doesn't match the original bytes and is not zero, then cheat is probably still active
                Trace.WriteLine("currentCameraRadiusBytes[i]: " + currentCameraRadiusBytes[i].ToString("X") + ", patchInfoCameraRadius.OriginalCodeBytes[i]: " + patchInfoCameraRadius.OriginalCodeBytes[i].ToString("X"));
                if (currentCameraRadiusBytes[i] != patchInfoCameraRadius.OriginalCodeBytes[i] && currentCameraRadiusBytes[i] != 0) return true;
            }

            Trace.WriteLine("Fell through!");
            return false;


        }





        private IntPtr? DetourHandle { get; set; } = null;
      
        public bool ApplyCheat()
        {
            try
            {
                this.HaloMemoryService.HaloState.UpdateHaloState();
                int loadedGame = this.CommonServices.GetLoadedGame();

                string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

                PatchInfo patchInfoCameraRadius = (PatchInfo)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_TheaterFriend_CameraRadius");
                PatchInfo patchInfoCameraOOB = (PatchInfo)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_TheaterFriend_CameraOOB");
                PatchInfo patchInfoOOBVisuals = (PatchInfo)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_TheaterFriend_OOBVisuals");



                byte[] currentCameraRadiusBytes = this.HaloMemoryService.ReadWrite.ReadBytes(patchInfoCameraRadius.OriginalCodeLocation, patchInfoCameraRadius.OriginalCodeBytes.Length);
                Trace.WriteLine("currentCameraRadiusBytes.Length: " + currentCameraRadiusBytes.Length);
                Trace.WriteLine("patchInfoCameraRadius.OriginalCodeBytes.Length: " + patchInfoCameraRadius.OriginalCodeBytes.Length);
                if (currentCameraRadiusBytes != null && currentCameraRadiusBytes.Length == patchInfoCameraRadius.OriginalCodeBytes.Length && currentCameraRadiusBytes.SequenceEqual(patchInfoCameraRadius.OriginalCodeBytes))
                {
                    Trace.WriteLine("Attempting to enable theater friend");
                    this.HaloMemoryService.ReadWrite.WriteBytes(patchInfoCameraRadius.OriginalCodeLocation, patchInfoCameraRadius.PatchedCodeBytes, true);
                    this.HaloMemoryService.ReadWrite.WriteBytes(patchInfoCameraOOB.OriginalCodeLocation, patchInfoCameraOOB.PatchedCodeBytes, true);
                    this.HaloMemoryService.ReadWrite.WriteBytes(patchInfoOOBVisuals.OriginalCodeLocation, patchInfoOOBVisuals.PatchedCodeBytes, true);
                    return true;
                }
                return false;

            }
            catch { return false; }

            //catch (Exception ex)
            //{
            //    Trace.WriteLine("Failed to enable OHK! \n" + ex.ToString());
            //    System.Windows.MessageBox.Show("Failed to enable OHK! \n" + ex.ToString() + ex.StackTrace, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
            //    return false;
            //}



        }

       
    }
}
