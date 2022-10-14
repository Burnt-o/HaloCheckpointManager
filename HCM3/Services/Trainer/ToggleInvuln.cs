using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using HCM3.Models;
using HCM3.Helpers;
using BurntMemory;
using System.Threading;
namespace HCM3.Services.Trainer
{
    public partial class TrainerServices
    {

        InvulnManager? InvulnManagerInstance { get; set; }

        public bool ToggleInvuln(int selectedGame, bool previousToggleState)
        {
            Trace.WriteLine("ToggleInvuln called, game: " + selectedGame);

            this.CommonServices.IsGameCorrect(selectedGame);
            string gameAs2Letters = Dictionaries.TabIndexTo2LetterGameCode[(int)selectedGame];


            List<string> requiredPointerNames = new();
            requiredPointerNames.Add($"{gameAs2Letters}_BP_PlayerAddy");
            requiredPointerNames.Add($"{gameAs2Letters}_BP_ShieldBreak");
            requiredPointerNames.Add($"{gameAs2Letters}_BP_ShieldChip");
            requiredPointerNames.Add($"{gameAs2Letters}_BP_Health");

            Dictionary<string, object> requiredPointers = this.CommonServices.GetRequiredPointers(requiredPointerNames);

            if (previousToggleState)
            {
                if (InvulnManagerInstance != null) InvulnManagerInstance.Dispose();
                this.CommonServices.PrintMessage("Invulnerability Disabled", selectedGame);
                return false;
            }
            else
            {
                // Okay so this approach isn't gonna work.
                // We need STATE so that invuln breakpoints can talk to eachother. which means creating a new "InvulnManager" object.
                //  one per game? nah one per toggle (on) of button.
                InvulnManagerInstance = new(requiredPointers, this.HaloMemoryService, selectedGame);
                this.CommonServices.PrintMessage("Invulnerability Enabled", selectedGame);
                return true;
            }


        }

        public class InvulnManager : IDisposable
        {
            private HaloMemoryService HaloMemoryService { get; init; }
            private int SelectedGame { get; init; }

            ulong? PlayerAddress { get; set; }
            public InvulnManager(Dictionary<string, object> requiredPointers, HaloMemoryService haloMemoryService, int selectedGame)
            {
                this.HaloMemoryService = haloMemoryService;
                this.SelectedGame = selectedGame;


                // Remove breakpoints, in case they were already set
                this.HaloMemoryService.DebugManager.RemoveBreakpoint("InvulnPlayerAddy");
                this.HaloMemoryService.DebugManager.RemoveBreakpoint("InvulnHealth");
                this.HaloMemoryService.DebugManager.RemoveBreakpoint("InvulnShieldChip");
                this.HaloMemoryService.DebugManager.RemoveBreakpoint("InvulnShieldBreak");

                // The delegate that will be passed to SetBreakpoint
                Func<PInvokes.CONTEXT64, PInvokes.CONTEXT64> onBreakpoint;

                // Breakpoint on function that updates Player Address location, so we can store it in our InvulnManager property
                onBreakpoint = context =>
                {
                    this.PlayerAddress = (UInt64)context.R15;
                    return context;
                };
                this.HaloMemoryService.DebugManager.SetBreakpoint("InvulnPlayerAddy", (ReadWrite.Pointer)requiredPointers["BP_PlayerAddy"], onBreakpoint);

                // Breakpoint on "Shield Broken" damage function, if the damaged entity (RDI) has the same Address as our player, set a flag (RCX) that will cause the rest of the function to not apply the damage
                onBreakpoint = context =>
                {
                    if (context.Rdi == (this.PlayerAddress + 0xA0))
                    {
                        context.Rcx = 0;
                    }
                    return context;
                };
                this.HaloMemoryService.DebugManager.SetBreakpoint("InvulnShieldBreak", (ReadWrite.Pointer)requiredPointers["BP_ShieldBreak"], onBreakpoint);

                // Breakpoint on "Shield Chipped" damage function, if the damaged entity (RDI) has the same Address as our player, set a flag (R9) that will cause the rest of the function to not apply the damage
                onBreakpoint = context =>
                {
                    if (context.Rdi == (this.PlayerAddress + 0xA0))
                    {
                        context.R9 = 0x0800;
                    }
                    return context;
                };
                this.HaloMemoryService.DebugManager.SetBreakpoint("InvulnShieldChip", (ReadWrite.Pointer)requiredPointers["BP_ShieldChip"], onBreakpoint);

                // Breakpoint on "Health" damage function, if the damaged entity (RBX) has the same Address as our player, set a flag (RBP) that will cause the rest of the function to not apply the damage
                onBreakpoint = context =>
                {
                    if (context.Rbx == this.PlayerAddress)
                    {
                        context.Rbp = 0;
                    }
                    return context;
                };
                this.HaloMemoryService.DebugManager.SetBreakpoint("InvulnHealth", (ReadWrite.Pointer)requiredPointers["BP_Health"], onBreakpoint);

            }


            public void Dispose()
            {
                this.HaloMemoryService.DebugManager.RemoveBreakpoint("InvulnPlayerAddy");
                this.HaloMemoryService.DebugManager.RemoveBreakpoint("InvulnHealth");
                this.HaloMemoryService.DebugManager.RemoveBreakpoint("InvulnShieldChip");
                this.HaloMemoryService.DebugManager.RemoveBreakpoint("InvulnShieldBreak");
            }
            
        }

    }
}
