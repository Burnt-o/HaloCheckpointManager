﻿using System;
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

    public class PC_OneHitKill : IPersistentCheat
    {
        private readonly object OneHitKillLock = new object();

        public PC_OneHitKill(HaloMemoryService haloMemoryService, DataPointersService dataPointersService, CommonServices commonServices, InternalServices internalServices)
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
            lock (OneHitKillLock)
            {
                if (!Properties.Settings.Default.DisableOverlay && !this.HaloMemoryService.HaloState.OverlayHooked) throw new Exception("Overlay wasn't hooked");

                Trace.WriteLine("User commanded ToggleOHK !!!!!!!!!!!!!!!!!!!!");
                if (IsChecked)
                {
                    Trace.WriteLine("turning OHK off");
                    RemoveCheat();
                    // IsChecked will only be set to false (and thus internal DLL updated) if removeCheat didn't throw
                    IsChecked = false;
                    // We might want to change this. I don't think there's any situation where removeCheat would fail AND the detour is actually still active ingame. *think* being the operative word.

                }
                else
                {
                    Trace.WriteLine("turning OHK on");

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
                        ex.ToString().Insert(0, "Failed to enabled OHK! ");
                        IsChecked = false;
                        throw;
                    }

                    if (!IsCheatApplied())
                    {
                        try { RemoveCheat(); } catch { }
                        IsChecked = false;
                        throw new Exception("Something went wrong and OHK wasn't applied properly; gamestate may be corrupt.");

                    }
                }
            }
        }


        public void RemoveCheat()
        {
            this.HaloMemoryService.HaloState.UpdateHaloState();
            int loadedGame = this.CommonServices.GetLoadedGame();
            string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];


            DetourInfoObject detourInfo = (DetourInfoObject)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_OHK_DetourInfo");
            if (Properties.Settings.Default.DisableOverlay) this.CommonServices.PrintMessage("OneHitKill mode disabled.");
            this.PersistentCheatService.DetourRemove(detourInfo, this.DetourHandle);
            // If above method throws then detour handle won't be set to null (intentional)
            this.DetourHandle = null;
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
            DetourInfoObject detourinfo;
            try
            {
                detourinfo = (DetourInfoObject)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_OHK_DetourInfo");
            }
            catch
            {
                Trace.WriteLine("Didn't have required pointers so cheat can't be active");
                return false;
            }

            // Just read 3 bytes for the test
            byte[]? actualCode = this.HaloMemoryService.ReadWrite.ReadBytes(detourinfo.OriginalCodeLocation, 3);
            if (actualCode == null) return false; // couldn't read bytes at og code, game not loaded? this shouldn't happen, I think


            for (int i = 0; i < actualCode.Length; i++)
            {
                // If the code at hook location doesn't match the original bytes and is not zero, then cheat is probably still active
                if (actualCode[i] != detourinfo.OriginalCodeBytes[i] && actualCode[i] != 0) return true;
            }
                    
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

                DetourInfoObject detourInfo = (DetourInfoObject)this.CommonServices.GetRequiredPointers($"{gameAs2Letters}_OHK_DetourInfo");

                if (Properties.Settings.Default.DisableOverlay) this.CommonServices.PrintMessage("OneHitKill mode enabled.");
                this.DetourHandle = this.PersistentCheatService.DetourApply(detourInfo);
                return true;

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