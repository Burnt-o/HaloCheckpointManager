using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.Diagnostics;
using HCMExternal.Services.InterprocServiceNS;
using System.Windows;

namespace HCMExternal.Services.MCCStateServiceNS
{


    // Checks every second (and when asked to) if MCC process is running. Sends attach/detach event when change.
    // On attach, stores the ProductVersion of MCC
    public class MCCStateService
    {
        // I made the following two properties static to remove a circular dependency (specifically so CheckpointService didn't need instance of MCCStateService)
        private static Process? _MCCProcess = null;
        public static Process? MCCProcess
        {
            get { return _MCCProcess; }
            private set { _MCCProcess = value; }
        }

        private static FileVersionInfo? _MCCVersion = null;
        public static FileVersionInfo? MCCVersion
        {
            get { return _MCCVersion; }
            private set { _MCCVersion = value; }
        }


        private InterprocService InterprocService { get; init; }
        public MCCStateService(InterprocService ips)
        {
            InterprocService = ips;
            checkMCCStatusLoop.Elapsed += new ElapsedEventHandler(CheckMCCStatusLoop);
        }

        public static event Action AttachEvent = delegate { Log.Information("Firing AttachEvent"); };
        public static event Action DetachEvent = delegate { Log.Information("Firing DetachEvent"); };

        public void beginAttaching()
        {
            checkMCCStatusLoop.Enabled = true;
            checkMCCStatusLoop.Start();
        }


        // Check every process; if it's MCC, try to attach to it
        public Process? CheckMCCStatus()
        {
            if (processesToAttach.Any() == false) return null;

            if (MCCProcess == null)
            {
                foreach (var procName in processesToAttach)
                {
                    if (TryAttach(procName))
                    {

                        return MCCProcess;
                    }
                }
                return null;
            }
            else
            {
                return MCCProcess;
            }

        }

        private static readonly string[] processesToAttach = { "MCC-Win64-Shipping", "MCC-Win64-Winstore-Shipping" };

        // Timer that will continually try to attach to MCC until it succeeds
        private static readonly System.Timers.Timer checkMCCStatusLoop = new System.Timers.Timer()
        {
            Interval = 1000,
            Enabled = false,
        };

        private void CheckMCCStatusLoop(object? source, EventArgs e)
        {
            Log.Verbose("Checking MCC status");
            CheckMCCStatus();
        }

        private readonly object AttachLock = new object();
        private bool TryAttach(string procName)
        {
            lock (AttachLock)
            {
                try
                {
                    foreach (Process process in Process.GetProcesses())
                    {
                        if (String.Equals(process.ProcessName, procName,
                         StringComparison.OrdinalIgnoreCase) && !process.HasExited)
                        {
                            // We don't want to inject while MCC is booting up since LoadLibrary is occupied
                            Log.Verbose("Found MCC, trying attach");
                            Log.Verbose("MCC age: " + (DateTime.Now - process.StartTime));
                            if (DateTime.Now - process.StartTime < TimeSpan.FromSeconds(3)) continue; 
                            if (InterprocService.Setup())
                            {
                                MCCProcess = process;
                                MCCProcess.Exited += MCCProcess_Exited;
                                MCCVersion = process.MainModule?.FileVersionInfo; 
                                AttachEvent();
                                return true;
                            }
                            else
                            {

                                // TODO: tell user something went wrong
                                MessageBox.Show("Something went wrong injecting internal.. not sure what");
                                return false;
                            }

                        }
                    }
                    return false;
                }
                catch (Exception ex)
                {
                    Log.Error(ex.Message);
                    return false;
                }
            }
        }


        private void MCCProcess_Exited(object? sender, EventArgs e)
        {
            MCCVersion = null;
            MCCProcess = null;
            DetachEvent();
        }


    }

}

