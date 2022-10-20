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
            if (IsChecked)
            {
                Trace.WriteLine("turning invuln off");
                RemoveCheat();
                IsChecked = false;
                
            }
            else
            {
                Trace.WriteLine("turning invuln on");
                IsChecked = ApplyCheat();
            }
        }


        public void RemoveCheat()
        {

            try
            {
                this.HaloMemoryService.HaloState.UpdateHaloState();
                int game = this.CommonServices.GetLoadedGame();


                switch (game)
                {
                    case 0:
                        List<string> requiredPointerNames = new();
                        requiredPointerNames.Add($"H1_Invuln_OGLocation");
                        requiredPointerNames.Add($"H1_Invuln_OGCode");

                        Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

                        byte[] ogCode = (byte[])requiredPointers["Invuln_OGCode"];
                        this.HaloMemoryService.ReadWrite.WriteData((ReadWrite.Pointer)requiredPointers["Invuln_OGLocation"], ogCode, true);
                        break;

                    default:
                        break;
                }
            }
            catch (Exception ex)
            { 
            //stop doing this
            }
        }

        public bool IsCheatApplied()
        {
            if (hookASM == null) return false;
            //use RPM to check if bytes are og or not
            int game;

            try
            {
                this.HaloMemoryService.HaloState.UpdateHaloState();
                game = this.CommonServices.GetLoadedGame();
            }
            catch
            {
                return false; // ??? will this always be accurate
            }

            switch (game)
            {
                case 0:
                    List<string> requiredPointerNames = new();
                    requiredPointerNames.Add($"H1_Invuln_OGLocation");

                    Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

                    byte[]? actualCode = this.HaloMemoryService.ReadWrite.ReadBytes((ReadWrite.Pointer)requiredPointers["Invuln_OGLocation"], 3);
                    if (actualCode == null) return false; // will this always be accurate?



                    for (int i = 0; i < actualCode.Length; i++)
                    {
                        if (actualCode[i] != hookASM[i]) return false;
                    }
                    
                    return true;

                default:
                    break;
            }

                return true;
        }

        private byte[]? hookASM { get; set; }

        private IntPtr? DetourHandle { get; set; } = null;

      
        public bool ApplyCheat()
        {
            Trace.WriteLine("Attempting to apply cheat");
            try
            {
                
                this.HaloMemoryService.HaloState.UpdateHaloState();
                int game = this.CommonServices.GetLoadedGame();

                switch (game)
                {
                    case 0:
                        using (Engine keystone = new Engine(Architecture.X86, Mode.X64) { ThrowOnError = true })
                        {
                            List<string> requiredPointerNames = new();
                            requiredPointerNames.Add($"H1_Invuln_OGLocation");
                            requiredPointerNames.Add($"H1_Invuln_OGCode");
                         //   requiredPointerNames.Add($"H1_Invuln_HookCode"); 
                            requiredPointerNames.Add($"H1_Invuln_DetourCode");
                            requiredPointerNames.Add($"H1_Invuln_PlayerDatum");

                            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

                            // Get handle of original code we're going to detour
                            IntPtr? ogCodeAddy = this.HaloMemoryService.ReadWrite.ResolvePointer((ReadWrite.Pointer?)requiredPointers["Invuln_OGLocation"]);
                            if (ogCodeAddy == null) throw new Exception("couldn't read og code addy");


                            //Allocate some memory for detour
                            System.Diagnostics.Process MCCProcess = System.Diagnostics.Process.GetProcessById((int)this.HaloMemoryService.HaloState.ProcessID);

                            // we only need like 30 bytes but this also acts as the incrementer for VirtualAllocExNear; larger values are far faster.
                                // Maybe I should make the incrementer a seperate value...
                            int sizeToAlloc = 10000;

                            //First deallocate previous detourHandle, if we'd enabled invuln before
                            if (this.DetourHandle != null)
                            {
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

                            
                            // Let's setup our detour code now
                            IntPtr? playerDatumAddy = this.HaloMemoryService.ReadWrite.ResolvePointer((ReadWrite.Pointer?)requiredPointers["Invuln_PlayerDatum"]);
                            if (playerDatumAddy == null) throw new Exception("couldn't read player Datum addy");

                            Trace.WriteLine("playerDatum: " + ((IntPtr)playerDatumAddy).ToString("X"));


                            
                            IntPtr instructionAfterOGcode = IntPtr.Add((IntPtr)ogCodeAddy, ogCode.Length);
                            Trace.WriteLine("intptr of instruction after OG code to jump back to: " + instructionAfterOGcode.ToString("X"));



                            Dictionary<string, IntPtr> resolver2 = new();
                            //resolver2.Add("_playerDatumAddy", (IntPtr)playerDatumAddy);
                            resolver2.Add("_returnControl", (IntPtr)instructionAfterOGcode); //add 6 cos jmp resolution is relative to end of instruction, not star
                         
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
                        break;

                    default:
                        throw new Exception("Invulnerability not implemented for this game yet");

                }
            }
            catch (Exception ex)
            {
                Trace.WriteLine("Failed to enable Invulnerability! \n" + ex.Message);
                System.Windows.MessageBox.Show("Failed to enable Invulnerability! \n" + ex.Message, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
                //UnhookInvuln();
                return false;
            }


            return true;
        }

       
    }
}
