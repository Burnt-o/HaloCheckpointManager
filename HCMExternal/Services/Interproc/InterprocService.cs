using Serilog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;


namespace HCMExternal.Services.InterprocServiceNS
{
    public class InterprocService
    {

        public delegate void PFN_ERRORCALLBACK(string str);
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ErrorEventSubscribe(PFN_ERRORCALLBACK callback);

        public delegate void PFN_LOGCALLBACK([MarshalAs(UnmanagedType.LPWStr)] string str);
        [DllImport("HCMInterproc.DLL", CallingConvention = CallingConvention.Cdecl)]
        public static extern void LogEventSubscribe(PFN_LOGCALLBACK callback);


        [DllImport("HCMInterproc.dll")] public static extern bool SetupInternal();

        public InterprocService()
        {
            // Subscribe to interproc events
            ErrorEventSubscribe(delegate(string str)
            {
                Log.Error("A HCMInterproc error occured: " + str);
            });
            LogEventSubscribe(delegate (string str)
            {
                Log.Verbose("HCMInterproc: " + str);
            });
        }

        public bool Setup()
        {
            return SetupInternal();
        }

        public void SendInjectCommand()
        {
            Log.Information("Sending inject command to internal");
            SetupInternal();
        }

        public void SendDumpCommand()
        {
            Log.Information("Sending dump command to internal");
            //TODO
        }

    }
}
