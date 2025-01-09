using Serilog;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Diagnostics;
using System.Linq;
using System.Runtime.ConstrainedExecution;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using HCMExternal.Services.External.Impl;

namespace HCMExternal.Services.External.FileMapping.Impl
{

    // Responsible for duplicating the save file handle, mapping it to our address space, then cleaning up the handle to that map
    public class MappedCartographerSave : IMappedCartographerSave
    {
        private IntPtr _data;
        private int _size;
        private WinHandle _fileMappingHandle;

        public IntPtr data() { return _data; }

        public int size() { return _size; }

        // RAII of handle returned by PInvokes
        private class WinHandle
        {
            private IntPtr _handle;
            public WinHandle(IntPtr handle) { _handle = handle; }
            ~WinHandle() { CloseHandle(_handle); }
            public static implicit operator IntPtr(WinHandle w) => w._handle;
            public static implicit operator WinHandle(IntPtr p) => new WinHandle(p);
        }


        public MappedCartographerSave(UInt32 saveFileHandle, Process cartographerProcess, int expectedSize)
        {

            ProcessAccess desiredDuplicationRights = ProcessAccess.DupHandle | ProcessAccess.AllAccess;

            WinHandle elevatedH2VHandle = OpenProcess(desiredDuplicationRights, false, cartographerProcess.Id);
            if (elevatedH2VHandle == nint.Zero)
                throw new Exception("Failed to open handle to halo game process with access perms: " + desiredDuplicationRights + ", Error code: " + GetLastError());

            WinHandle ownHandle = OpenProcess(desiredDuplicationRights, false, Environment.ProcessId);
            if (ownHandle == IntPtr.Zero)
                throw new Exception("Failed to open handle to own process with access perms: " + desiredDuplicationRights + ", Error code: " + GetLastError());

            bool duped = DuplicateHandle(elevatedH2VHandle, new IntPtr(0x970), ownHandle, out IntPtr duplicatedHandleRaw, ACCESS_MASK.GENERIC_READ | ACCESS_MASK.GENERIC_WRITE, false, 0);
            if (!duped)
                throw new Exception("Failed to duplicate checkpoint file handle, Error code: " + GetLastError());

            WinHandle duplicatedHandle = duplicatedHandleRaw;

            _fileMappingHandle = CreateFileMapping(duplicatedHandle, 0, PageProtection.ReadWrite, 0, 0, 0);
            if (_fileMappingHandle == IntPtr.Zero)
                throw new Exception("failed to create file mapping, Error code: " + GetLastError());

            _data = MapViewOfFile(_fileMappingHandle, FileMapAccess.FileMapWrite | FileMapAccess.FileMapRead, 0, 0, 0);
            if (_data == IntPtr.Zero)
                throw new Exception("failed to create MapViewOfFile, Error code: " + GetLastError());

            // TODO: check against expectedSize
        }


        #region PInvokes

        [DllImport("kernel32.dll", SetLastError = true)]
        [SuppressUnmanagedCodeSecurity]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool CloseHandle(IntPtr hObject);

        [DllImport("kernel32.dll")]
        public static extern uint GetLastError();

        [DllImport("kernel32.dll")]
        private static extern nint OpenProcess(ProcessAccess processAccessFlags, bool bInheritHandle, int dwProcessId);

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
   ACCESS_MASK dwDesiredAccess, [MarshalAs(UnmanagedType.Bool)] bool bInheritHandle, DuplicateOptions dwOptions);

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
   IntPtr lpFileMappingAttributes, PageProtection flProtect, uint dwMaximumSizeHigh,
   uint dwMaximumSizeLow, uint lpName);



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
        public enum ACCESS_MASK : uint
        {
            DELETE = 0x00010000,
            READ_CONTROL = 0x00020000,
            WRITE_DAC = 0x00040000,
            WRITE_OWNER = 0x00080000,
            SYNCHRONIZE = 0x00100000,

            STANDARD_RIGHTS_REQUIRED = 0x000F0000,

            STANDARD_RIGHTS_READ = 0x00020000,
            STANDARD_RIGHTS_WRITE = 0x00020000,
            STANDARD_RIGHTS_EXECUTE = 0x00020000,

            STANDARD_RIGHTS_ALL = 0x001F0000,

            SPECIFIC_RIGHTS_ALL = 0x0000FFFF,

            ACCESS_SYSTEM_SECURITY = 0x01000000,

            MAXIMUM_ALLOWED = 0x02000000,

            GENERIC_READ = 0x80000000,
            GENERIC_WRITE = 0x40000000,
            GENERIC_EXECUTE = 0x20000000,
            GENERIC_ALL = 0x10000000,

            DESKTOP_READOBJECTS = 0x00000001,
            DESKTOP_CREATEWINDOW = 0x00000002,
            DESKTOP_CREATEMENU = 0x00000004,
            DESKTOP_HOOKCONTROL = 0x00000008,
            DESKTOP_JOURNALRECORD = 0x00000010,
            DESKTOP_JOURNALPLAYBACK = 0x00000020,
            DESKTOP_ENUMERATE = 0x00000040,
            DESKTOP_WRITEOBJECTS = 0x00000080,
            DESKTOP_SWITCHDESKTOP = 0x00000100,

            WINSTA_ENUMDESKTOPS = 0x00000001,
            WINSTA_READATTRIBUTES = 0x00000002,
            WINSTA_ACCESSCLIPBOARD = 0x00000004,
            WINSTA_CREATEDESKTOP = 0x00000008,
            WINSTA_WRITEATTRIBUTES = 0x00000010,
            WINSTA_ACCESSGLOBALATOMS = 0x00000020,
            WINSTA_EXITWINDOWS = 0x00000040,
            WINSTA_ENUMERATE = 0x00000100,
            WINSTA_READSCREEN = 0x00000200,

            WINSTA_ALL_ACCESS = 0x0000037F
        }

        [Flags]
        enum PageProtection : uint
        {
            NoAccess = 0x01,
            Readonly = 0x02,
            ReadWrite = 0x04,
            WriteCopy = 0x08,
            Execute = 0x10,
            ExecuteRead = 0x20,
            ExecuteReadWrite = 0x40,
            ExecuteWriteCopy = 0x80,
            Guard = 0x100,
            NoCache = 0x200,
            WriteCombine = 0x400,
        }
        #endregion PInvokes
    }
}
