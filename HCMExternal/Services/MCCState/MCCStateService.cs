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
        public static event Action AttachInProgressEvent = delegate { Log.Information("Firing AttachInProgressEvent"); };
        public static event Action DetachEvent = delegate { Log.Information("Firing DetachEvent"); };
        public static event Action AttachErrorEvent = delegate { Log.Information("Firing AttachErrorEvent"); };

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
                if (MCCProcess.HasExited)
                {
                    Log.Information("MCC process exited!");
                    MCCProcess = null;
                    MCCVersion = null;
                    DetachEvent();
                }
                return MCCProcess;
            }

        }

        private static readonly string[] processesToAttach = { "MCC-Win64-Shipping", "MCC-Win64-Winstore-Shipping", "MCCWinstore-Win64-Shipping" };

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

        private bool giveUpInjecting = false;
        private string lastInjectionError = "no error";
        private readonly object AttachLock = new object();
        private bool TryAttach(string procName)
        {


            lock (AttachLock)
            {
                if (giveUpInjecting) return false;
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
                            AttachInProgressEvent();

                            var internalInjectResult = InterprocService.Setup();
                            if (internalInjectResult.Item1)
                            {
                                MCCProcess = process;
                                MCCProcess.Disposed += MCCProcess_Exited;
                                MCCProcess.Exited += MCCProcess_Exited;
                                MCCVersion = process.MainModule?.FileVersionInfo; 
                                AttachEvent();
                                return true;
                            }
                            else
                            {
                                AttachErrorEvent();
                                lastInjectionError = "HCM failed to inject its internal module into the game! \nMore info in log files. Error: \n" + internalInjectResult.Item2 + "\n\nRetry injection?";
                                ShowInjectionError();
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

        public void ShowInjectionError()
        {
            var result = MessageBox.Show(lastInjectionError, "HCM Internal Error!", MessageBoxButton.YesNo, MessageBoxImage.Error);
            if (result == MessageBoxResult.No)
            {
                // User doesn't want to retry injection
                giveUpInjecting = true;
            }
            else
            {
                giveUpInjecting = false;
            }
        }

        public void unGiveUpInjection() // if the user gave up on failed injecting, can call this by clicking status button to un-giveup
        {
            giveUpInjecting = false;
        }


        private void MCCProcess_Exited(object? sender, EventArgs e)
        {
            Log.Information("MCCProcess_Exited");
            MCCVersion = null;
            MCCProcess = null;
            DetachEvent();
        }


    }

}

