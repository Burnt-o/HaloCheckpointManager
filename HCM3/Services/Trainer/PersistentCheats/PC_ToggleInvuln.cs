using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Diagnostics;
using BurntMemory;

namespace HCM3.Services.Trainer
{
    public class PC_ToggleInvuln : IPersistentCheatService
    {


        public PC_ToggleInvuln(HaloMemoryService haloMemoryService, DataPointersService dataPointersService, CommonServices commonServices)
        {
            this.HaloMemoryService = haloMemoryService;
            this.DataPointersService = dataPointersService;
            this.CommonServices = commonServices;

            PlayerAddress = 0;
            ListOfSetBreakpoints = new();
        }

        public HaloMemoryService HaloMemoryService { get; init; }
        public DataPointersService DataPointersService { get; init; }

        public CommonServices CommonServices { get; init; }

        private List<string> ListOfSetBreakpoints { get; set; }

        public UInt64 PlayerAddress { get; set; }

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
                UnhookInvuln();
                IsChecked = false;
                
            }
            else
            {
                Trace.WriteLine("turning invuln on");
                IsChecked = HookInvuln();
            }
        }

        public bool HookInvuln()
        {
            try
            {
                UnhookInvuln();
                this.HaloMemoryService.HaloState.UpdateHaloState();
                int game = this.CommonServices.GetLoadedGame();

                switch (game)
                {
                    case 0:
                        List<string> requiredPointerNames = new();
                        requiredPointerNames.Add($"H1_BP_PlayerAddy");
                        requiredPointerNames.Add($"H1_BP_ShieldBreak");
                        requiredPointerNames.Add($"H1_BP_ShieldChip");
                        requiredPointerNames.Add($"H1_BP_Health");

                        Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);
                        ListOfSetBreakpoints.AddRange(requiredPointerNames);

                        //Note to self; will need to later implement dynamically loading the onBreakpoint funcs from DataPointers
                        //this will require Roslyn scripting, more info here:
                        // https://www.strathweb.com/2018/01/easy-way-to-create-a-c-lambda-expression-from-a-string-with-roslyn/
                        // and here
                        // https://stackoverflow.com/questions/10655761/convert-string-into-func

                        Func<PInvokes.CONTEXT64, PInvokes.CONTEXT64> onBreakpoint;

                        onBreakpoint = context =>
                        {
                            this.PlayerAddress = (UInt64)context.R15;
                            return context;
                        };
                        this.HaloMemoryService.DebugManager.SetBreakpoint("H1_BP_PlayerAddy", (ReadWrite.Pointer)requiredPointers["BP_PlayerAddy"], onBreakpoint);

                        // Breakpoint on "Shield Broken" damage function, if the damaged entity (RDI) has the same Address as our player, set a flag (RCX) that will cause the rest of the function to not apply the damage
                        onBreakpoint = context =>
                        {
                            if (context.Rdi == (this.PlayerAddress + 0xA0))
                            {
                                context.Rcx = 0;
                            }
                            return context;
                        };
                        this.HaloMemoryService.DebugManager.SetBreakpoint("H1_BP_ShieldBreak", (ReadWrite.Pointer)requiredPointers["BP_ShieldBreak"], onBreakpoint);

                        // Breakpoint on "Shield Chipped" damage function, if the damaged entity (RDI) has the same Address as our player, set a flag (R9) that will cause the rest of the function to not apply the damage
                        onBreakpoint = context =>
                        {
                            if (context.Rdi == (this.PlayerAddress + 0xA0))
                            {
                                context.R9 = 0x0800;
                            }
                            return context;
                        };
                        this.HaloMemoryService.DebugManager.SetBreakpoint("H1_BP_ShieldChip", (ReadWrite.Pointer)requiredPointers["BP_ShieldChip"], onBreakpoint);

                        // Breakpoint on "Health" damage function, if the damaged entity (RBX) has the same Address as our player, set a flag (RBP) that will cause the rest of the function to not apply the damage
                        onBreakpoint = context =>
                        {
                            if (context.Rbx == this.PlayerAddress)
                            {
                                context.Rbp = 0;
                            }
                            return context;
                        };
                        this.HaloMemoryService.DebugManager.SetBreakpoint("H1_BP_Health", (ReadWrite.Pointer)requiredPointers["BP_Health"], onBreakpoint);

                        break;

                    default:
                        throw new Exception("Invulnerability not implemented for this game yet");

                }
            }
            catch (Exception ex)
            {
                Trace.WriteLine("Failed to enable Invulnerability! \n" + ex.Message);
                System.Windows.MessageBox.Show("Failed to enable Invulnerability! \n" + ex.Message, "HaloCheckpointManager Error", System.Windows.MessageBoxButton.OK);
                UnhookInvuln();
                return false;
            }


            return true;
        }

        public void UnhookInvuln()
        {
            if (ListOfSetBreakpoints.Any())
            {

                foreach (string setBreakpoint in ListOfSetBreakpoints)
                {
                    Trace.WriteLine("found breakpoint, removing: " + setBreakpoint);
                    this.HaloMemoryService.DebugManager.RemoveBreakpoint(setBreakpoint);

                }
                ListOfSetBreakpoints.Clear();
            }
            else
            {
                Trace.WriteLine("didn't find any breakpoints to remove on UnHook. ListOfSetBreakpoints was empty");
            }
            
        }
    }
}
