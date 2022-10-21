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
    public partial class PC_ToggleInvuln : IPersistentCheatService
    {


        public PC_ToggleInvuln(HaloMemoryService haloMemoryService, DataPointersService dataPointersService, CommonServices commonServices, InternalServices internalServices)
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
            Trace.WriteLine("User commanded ToggleInvuln !!!!!!!!!!!!!!!!!!!!");
            if (IsChecked)
            {
                Trace.WriteLine("turning invuln off");
                RemoveCheat();
                IsChecked = false;
                
            }
            else
            {
                Trace.WriteLine("turning invuln on");
                IsChecked = true;
                ApplyCheat();
                if (!IsCheatApplied())
                {
                    IsChecked = false;
                    Trace.WriteLine("IsCheatApplied failed!!");
                }
            }
        }


        public void RemoveCheat()
        {
            // should check first if IsCheatApplied
            try
            {
                this.HaloMemoryService.HaloState.UpdateHaloState();
                int loadedGame = this.CommonServices.GetLoadedGame();

                string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];

                List<string> requiredPointerNames = new();
                        requiredPointerNames.Add($"{gameAs2Letters}_Invuln_OGLocation");
                        requiredPointerNames.Add($"{gameAs2Letters}_Invuln_OGCode");

                        Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

                        byte[] ogCode = (byte[])requiredPointers["Invuln_OGCode"];
                        this.HaloMemoryService.ReadWrite.WriteData((ReadWrite.Pointer)requiredPointers["Invuln_OGLocation"], ogCode, true);

            }
            catch (Exception ex)
            { 
            //stop doing this
            System.Windows.MessageBox.Show("Failed removing cheat? ex; " + ex.Message);
            }
        }

        public bool IsCheatApplied()
        {
            if (hookASM == null) return false; // hook is not applied, otherwise hookASM bytes wouldn't be null
            //use RPM to check if bytes are og or not
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
            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_Invuln_OGLocation");
            Dictionary<string, object> requiredPointers;
            try
            {
                requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);
            }
            catch
            {
                Trace.WriteLine("Didn't have required pointers so cheat can't be active");
                return false;
            }

            // Just read 3 bytes for the test
            byte[]? actualCode = this.HaloMemoryService.ReadWrite.ReadBytes((ReadWrite.Pointer)requiredPointers["Invuln_OGLocation"], 3);
            if (actualCode == null) return false; // couldn't read bytes at og code, game not loaded? this shouldn't happen, I think



            for (int i = 0; i < actualCode.Length; i++)
            {
                // If the code at hook location doesn't match the hook bytes then cheat isn't active
                if (actualCode[i] != hookASM[i]) return false;
            }
                    
            return true;


        }


        private byte[]? hookASM { get; set; }

        private byte[]? originalBytes { get; set; }

        private IntPtr? DetourHandle { get; set; } = null;

      
        public bool ApplyCheat()
        {
            Trace.WriteLine("Attempting to apply cheat");
            try
            {
                
                this.HaloMemoryService.HaloState.UpdateHaloState();
                int loadedGame = this.CommonServices.GetLoadedGame();
                string gameAs2Letters = Dictionaries.GameTo2LetterGameCode[(int)loadedGame];
                Trace.WriteLine("Attempting to apply cheat to " + gameAs2Letters);


                using (Engine keystone = new Engine(Architecture.X86, Mode.X64) { ThrowOnError = true })
                        {
                            List<string> requiredPointerNames = new();
                            requiredPointerNames.Add($"{gameAs2Letters}_Invuln_OGLocation");
                            requiredPointerNames.Add($"{gameAs2Letters}_Invuln_OGCode");
                            requiredPointerNames.Add($"{gameAs2Letters}_Invuln_DetourCode");
                            requiredPointerNames.Add($"{gameAs2Letters}_Invuln_PlayerDatum");

                            

                            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

                    //might just integrate this into all of the requiredPointers, not sure if all games will need _skipDamage or not
                    bool skipDamageOffsetNeeded = ((string)requiredPointers["Invuln_DetourCode"]).Contains("_skipDamage");
                    if (skipDamageOffsetNeeded)
                    {
                        List<string> extraPointerName = new();
                        extraPointerName.Add($"{gameAs2Letters}_Invuln_SkipDamage");
                         Dictionary<string, object> newPointers = this.CommonServices.GetRequiredPointers(extraPointerName);
                        foreach (var pointer in newPointers)
                        {
                            requiredPointers.Add(pointer.Key, pointer.Value);
                        }
                    }

                    // Get handle of original code we're going to detour
                    ReadWrite.Pointer? ogCodePointer = (ReadWrite.Pointer?)requiredPointers["Invuln_OGLocation"];
                    if (ogCodePointer == null) throw new Exception("didn't have pointer to og code");
                            IntPtr? ogCodeAddy = this.HaloMemoryService.ReadWrite.ResolvePointer(ogCodePointer);
                            if (ogCodeAddy == null) throw new Exception("couldn't read og code addy");


                            //Allocate some memory for detour
                            System.Diagnostics.Process MCCProcess = System.Diagnostics.Process.GetProcessById((int)this.HaloMemoryService.HaloState.ProcessID);

                            // we only need like 30 bytes but this also acts as the incrementer for VirtualAllocExNear; larger values are far faster.
                                // Maybe I should make the incrementer a seperate value...
                            int sizeToAlloc = 50;

                            //First deallocate previous detourHandle, if we'd enabled invuln before
                            //TODO CHECK IF THIS IS DEALLOCATING
                            if (this.DetourHandle != null)
                            {
                         Trace.WriteLine("Deallocated previous memory page at " + this.DetourHandle.Value.ToString("X"));
                                PInvokes.VirtualFreeEx(MCCProcess.Handle, this.DetourHandle.Value, sizeToAlloc, PInvokes.AllocationType.Release);
                                this.DetourHandle = null;
                            }


                            // Need to make sure the memory is close to the original code so we can fit the jmp in 32 bits relative
                            IntPtr detourHandle;
                            try
                            {
                                detourHandle = InternalServices.VirtualAllocExNear(MCCProcess.Handle, sizeToAlloc, (IntPtr)ogCodeAddy);
                                this.DetourHandle = detourHandle;
                            }
                            catch (Exception ex)
                            {
                                Trace.WriteLine("Couldn't alloc detour: " + ex.Message);
                                throw;
                            }
                            

                            Trace.WriteLine("detourHandle: " + detourHandle.ToString("X"));



                            byte[] ogCode = (byte[])requiredPointers["Invuln_OGCode"];
                            byte[] hookASMbytes = new byte[ogCode.Length];
                            Array.Fill(hookASMbytes, (byte)0x90);
                            string hookASMstring = "jmp _detourAddy";
                            hookASMstring.Replace("_detourAddy", detourHandle.ToInt64().ToString("X") + "h");

                            Dictionary<string, IntPtr> resolver1 = new();
                            resolver1.Add("_detourAddy", detourHandle);


                            byte[] temp = KeystoneAssemblerOverride.Assemble(keystone, resolver1, hookASMstring, (ulong)ogCodeAddy.Value.ToInt64(), out int bytesAssembled, out _);
                       
                            Array.Copy(temp, hookASMbytes, temp.Length);

                            Trace.WriteLine("ogCode length: " + ogCode.Length + ", hookASMlength: " + hookASMbytes.Length);
                            Trace.WriteLine("bytes assembled " + temp.Length);

                            //debug
                            Trace.WriteLine("A");
                            Trace.WriteLine("");
                            for (int i = 0; i < hookASMbytes.Length; i++)
                            {
                                Trace.Write(hookASMbytes[i].ToString("X2"));
                            }
                            Trace.WriteLine("");
                            Trace.WriteLine("");


                    this.hookASM = hookASMbytes; // used by IsCheatApplied, not sure if this is the best way to do this.

                            
                            // Let's setup our detour code now
                            IntPtr? playerDatumAddy = this.HaloMemoryService.ReadWrite.ResolvePointer((ReadWrite.Pointer?)requiredPointers["Invuln_PlayerDatum"]);
                            if (playerDatumAddy == null) throw new Exception("couldn't read player Datum addy");

                            Trace.WriteLine("playerDatum: " + ((IntPtr)playerDatumAddy).ToString("X"));


                            
                            IntPtr instructionAfterOGcode = IntPtr.Add((IntPtr)ogCodeAddy, ogCode.Length);
                            Trace.WriteLine("intptr of instruction after OG code to jump back to: " + instructionAfterOGcode.ToString("X"));



                            Dictionary<string, IntPtr> resolver2 = new();
                            resolver2.Add("_returnControl", (IntPtr)instructionAfterOGcode);

                    if (skipDamageOffsetNeeded)
                    { 
                    IntPtr? skipDamageHandle = this.HaloMemoryService.ReadWrite.ResolvePointer((ReadWrite.Pointer?)requiredPointers["Invuln_SkipDamage"]);
                        if (skipDamageHandle == null) throw new Exception("couldn't read skipDamageHandle");

                        resolver2.Add("_skipDamage", (IntPtr)skipDamageHandle);
                    }
                         
                            string playerDatumAddyString = "0x" + playerDatumAddy.Value.ToInt64().ToString("X");
                            string detourASMstring = (string)requiredPointers["Invuln_DetourCode"];
                            detourASMstring = detourASMstring.Replace("_playerDatumAddy", playerDatumAddyString);

                            Trace.WriteLine("detourASMstring: " + detourASMstring);
                            byte[] detourASMbytes = KeystoneAssemblerOverride.Assemble(keystone, resolver2, detourASMstring, (ulong)detourHandle.ToInt64(), out _, out _);


                            //debug
                            Trace.WriteLine("C");
                            Trace.WriteLine("");
                            for (int i = 0; i < detourASMbytes.Length; i++)
                            {
                                Trace.Write(detourASMbytes[i].ToString("X2"));
                            }
                            Trace.WriteLine("");
                            Trace.WriteLine("");

                            //return false; //for debug reasons
                            // Now write our detour code at allocated memory address
                            this.HaloMemoryService.ReadWrite.WriteData(new ReadWrite.Pointer(detourHandle), detourASMbytes, true);
                            Trace.WriteLine("Copy paste those bytes to: " + detourHandle.ToString("X"));
                            // Then write the hook
                            this.HaloMemoryService.ReadWrite.WriteData(new ReadWrite.Pointer(ogCodeAddy), hookASMbytes, true);


                            return true;
                        }
                       

                }
            
            catch (Exception ex)
            {
                Trace.WriteLine("Failed to enable Invulnerability! \n" + ex.Message);
                System.Windows.MessageBox.Show("Failed to enable Invulnerability! \n" + ex.Message, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
                //UnhookInvuln();
                return false;
            }



        }

       
    }
}
