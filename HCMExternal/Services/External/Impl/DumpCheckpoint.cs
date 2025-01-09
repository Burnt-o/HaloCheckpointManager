using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCMExternal.Models;
using HCMExternal.Services.PointerData;
using System.IO;
using System.Runtime.InteropServices;
using System.Diagnostics;
using HCMExternal.Services.External.FileMapping;

namespace HCMExternal.Services.External.Impl
{
    internal partial class ExternalService : IExternalService
    {
        /*
        [Flags]
        public enum FileMapAccess : uint
        {
            FileMapCopy = 0x0001,
            FileMapWrite = 0x0002,
            FileMapRead = 0x0004,
            FileMapAllAccess = 0x001f,
            FileMapExecute = 0x0020,
        }

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr MapViewOfFile(
            IntPtr hFileMappingObject,
            FileMapAccess dwDesiredAccess,
            UInt32 dwFileOffsetHigh,
            UInt32 dwFileOffsetLow,
            UIntPtr dwNumberOfBytesToMap);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr CreateFileMapping(IntPtr hFile,
   IntPtr lpFileMappingAttributes, uint flProtect, uint dwMaximumSizeHigh,
   uint dwMaximumSizeLow, string lpName);



        [StructLayout(LayoutKind.Explicit, Size = 20)]
        public struct OVERLAPPED
        {
            [FieldOffset(0)]
            public uint Internal;

            [FieldOffset(4)]
            public uint InternalHigh;

            [FieldOffset(8)]
            public uint Offset;

            [FieldOffset(12)]
            public uint OffsetHigh;

            [FieldOffset(8)]
            public IntPtr Pointer;

            [FieldOffset(16)]
            public IntPtr hEvent;
        }


        [Flags]
        public enum DuplicateOptions : uint
        {
            DUPLICATE_CLOSE_SOURCE = (0x00000001),// Closes the source handle. This occurs regardless of any error status returned.
            DUPLICATE_SAME_ACCESS = (0x00000002), //Ignores the dwDesiredAccess parameter. The duplicate handle has the same access as the source handle.
        }

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool DuplicateHandle(IntPtr hSourceProcessHandle,
   IntPtr hSourceHandle, IntPtr hTargetProcessHandle, out IntPtr lpTargetHandle,
   uint dwDesiredAccess, [MarshalAs(UnmanagedType.Bool)] bool bInheritHandle, DuplicateOptions dwOptions);



        [DllImport("kernel32.dll")]
        private static extern nint OpenProcess(ProcessAccess processAccessFlags, bool bInheritHandle, int dwProcessId);


        [DllImport("kernel32.dll")]
        public static extern uint GetLastError();

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool ReadFile(IntPtr hFile, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] byte[] pBuffer,
   uint nNumberOfBytesToRead, out uint lpNumberOfBytesRead, OVERLAPPED lpOverlapped);



        unsafe public void  DumpCheckpoint(SaveFolder saveFolder)
        {

            // get process
            HaloProcessInfo haloProcess = GetHaloProcessInfo();

            // get inject requirements
            InjectRequirements injectRequirements = PointerData.GetGameProcessData<InjectRequirements>(haloProcess.processType, haloProcess.haloGame, "InjectRequirements", haloProcess.processVersion);


            // shitty test code for the moment:
            // H2V keeps two open file handles to the checkpoints, handle id stored at halo2.exe+4C1508, halo2.exe+4C150C
            // so can we just duplicate the handle?
            ProcessAccess duplicationPerms = ProcessAccess.DupHandle | ProcessAccess.AllAccess;
            int checkpointFileSize = 4239360;

            nint elevatedH2VHandle = OpenProcess(duplicationPerms, false, haloProcess.processHandle.Id);
            if (elevatedH2VHandle == nint.Zero)
                throw new Exception("Failed to open handle to halo game process with access perms: " + duplicationPerms + ", Error code: " + GetLastError());


            nint ownHandle = OpenProcess(duplicationPerms, false, Environment.ProcessId);
            if (ownHandle == nint.Zero)
                throw new Exception("Failed to open handle to own process with access perms: " + duplicationPerms + ", Error code: " + GetLastError());

            bool duped = DuplicateHandle(elevatedH2VHandle, new IntPtr(0x970), ownHandle, out IntPtr duplicatedHandle, 0x80000000, false, 0);
            if (!duped)
                throw new Exception("Failed to duplicate checkpoint file handle, Error code: " + GetLastError());


            IntPtr filemaphandle = CreateFileMapping(duplicatedHandle, 0, 0x02, 0, 0, "filemappedcheckoint");
            if (filemaphandle == IntPtr.Zero)
                throw new Exception("failed to create file mapping, Error code: " + GetLastError());


            IntPtr mapviewhandle = MapViewOfFile(filemaphandle, FileMapAccess.FileMapWrite | FileMapAccess.FileMapRead, 0, 0, 0);
            if (mapviewhandle == IntPtr.Zero)
                throw new Exception("failed to create MapViewOfFile, Error code: " + GetLastError());

            // .. mapviewhandle is a ptr to the data in our memory. Can we just pump that into WriteFile?



            byte[] managedBuffer = new byte[checkpointFileSize];
            Marshal.Copy(mapviewhandle, managedBuffer, 0, checkpointFileSize);

            File.WriteAllBytes(Directory.GetCurrentDirectory() + @"\Saves\Project Cartographer\CartographerTestDump3.bin", managedBuffer);




            // okay above works! 
            // now we just need to figure out how to inject as well. 
            // and we should probably move this logic over to interproc 


            //    Dump needs to pass: 
            //    destination file. const char*
            //    process handle? or pid. dword
            //    file handle id dword

            //    Inject needs to pass:
            //    byte array (AFTER reading it off whatever the current value is)



            //    Better idea;
            //have a common function, get MappedCheckpointFile, return a byte array. 
            //this function calls into interproc to do it's work. Then we can just dump this byte array or write to it. 




        }
          */

        public void DumpCheckpoint(SaveFolder saveFolder)
        {
            // get process
            HaloProcessInfo haloProcess = GetHaloProcessInfo();
            if (haloProcess.processType != HaloProcessType.ProjectCartographer)
                throw new NotImplementedException("Only project cartographer supports external save management");

            // get inject requirements
            InjectRequirements injectRequirements = PointerData.GetGameProcessData<InjectRequirements>(haloProcess.processType, haloProcess.haloGame, "InjectRequirements", haloProcess.processVersion);

            // TODO: check which checkpoint slot it is off dr flag
            // TODO: lookup pointer of file handle in h2 exe
            UInt32 saveFileHandle = 0x970;

            // TODO: lookup file size from pointer data
            int checkpointFileSize = 4239360;

            // throws on failure
            var unmanagedCheckpointData = MappedCartographerSaveFactory.make(saveFileHandle, haloProcess.processHandle, checkpointFileSize);

            byte[] managedBuffer = new byte[checkpointFileSize];
            Marshal.Copy(unmanagedCheckpointData.data(), managedBuffer, 0, checkpointFileSize);

            // TODO: ask user where to save the checkpoint file
            File.WriteAllBytes(Directory.GetCurrentDirectory() + @"\Saves\Project Cartographer\CartographerTestDump3.bin", managedBuffer);

        }

    }
}
