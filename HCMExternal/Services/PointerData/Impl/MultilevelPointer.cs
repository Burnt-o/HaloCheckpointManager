using GongSolutions.Wpf.DragDrop.Utilities;
using Serilog;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Automation.Text;
using System.Xml.Linq;

namespace HCMExternal.Services.PointerData.Impl
{
    public class MultilevelPointer : IMultilevelPointer
    {
        private HaloGame Game { get; init; }
        private int[] Offsets { get; init; } = { };



        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, int nSize, out int lpNumberOfBytesWritten);

        [DllImport("kernel32.dll")]
        private static extern bool ReadProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, int dwSize, out int lpNumberOfBytesRead);

        [DllImport("kernel32.dll")]
        private static extern IntPtr OpenProcess(ProcessAccess processAccessFlags, bool bInheritHandle, int dwProcessId);

        [DllImport("kernel32.dll")]
        static extern bool VirtualProtectEx(IntPtr hProcess, IntPtr lpAddress, int dwSize, int flNewProtect, out int lpflOldProtect);

        [DllImport("kernel32.dll")]
        public static extern uint GetLastError();

        public IntPtr Resolve(IntPtr processHandle, IntPtr baseAddress)
        {
            IntPtr ptr = baseAddress;

            Log.Verbose("Resolving pointer beginning at baseAddress: 0x" + baseAddress.ToString("X"));

            if (Offsets.Length == 0)
                return ptr;
            
            ptr += Offsets[0];

            Log.Verbose("After adding first offset, ptr is: 0x" + ptr.ToString("X"));

            if (Offsets.Length == 1)
                return ptr;

            var LoopOffsets = Offsets.Skip(1).ToArray();


            foreach (int offset in Offsets)
            {
                byte[] buffer = new byte[8];
                if (!ReadProcessMemory(processHandle, ptr, buffer, 8, out int lpNumberOfBytesRead) || lpNumberOfBytesRead != 8)
                    throw new System.Exception("Failed to read data from halo game process while looping pointer offsets! Error code: " + GetLastError());

                Int64 baseOfNextPointer = BitConverter.ToInt64(buffer, 0);

                Log.Verbose(string.Format("baseOfNextPointer: 0x{0:X}", baseOfNextPointer));
                Log.Verbose(string.Format("next offset: 0x{0:X}", offset));

                ptr = IntPtr.Add(new IntPtr(baseOfNextPointer), offset);

                Log.Verbose(string.Format("ptr: 0x{0:X}", ptr));
            }
            return ptr;
        }


        private const ProcessAccess readFlags = ProcessAccess.VmRead;
        private const ProcessAccess writeFlags = ProcessAccess.VmWrite | ProcessAccess.VmOperation;

        public IntPtr Resolve(Process process)
        {
            IntPtr processHandle = OpenProcess(readFlags, false, process.Id);
            if (processHandle == IntPtr.Zero)
                throw new System.Exception("Failed to open handle to halo game process with access perms: " + readFlags + ", Error code: " + GetLastError());

            return Resolve(processHandle, process.MainModule?.BaseAddress ?? throw new Exception("Could not access base address of halo game process"));
        }

        public void writeData(Process process, byte[] data, bool protectedMemory)
        {
            IntPtr processHandle = OpenProcess(writeFlags, false, process.Id);
            if (processHandle == IntPtr.Zero)
                throw new System.Exception("Failed to open handle to halo game process with access perms: " + writeFlags + ", Error code: " + GetLastError());

            IntPtr address = Resolve(processHandle, process.MainModule?.BaseAddress ?? throw new Exception("Could not access base address of halo game process"));
            Log.Verbose(string.Format("Writing 0x{0:X} bytes to address: 0x{1:X}", data.Length, address));

            int oldProtect = 0;
            const int PAGE_EXECUTE_READWRITE = 0x40; 
            if (protectedMemory)
            {
                if (!VirtualProtectEx(processHandle, address, data.Length, PAGE_EXECUTE_READWRITE, out oldProtect))
                    throw new Exception(string.Format("Failed to unprotect memory page at 0x{0:X}, error code: {1}", address, GetLastError()));
            }

            if (!WriteProcessMemory(processHandle, address, data, data.Length, out int lpNumberOfBytesWritten) || lpNumberOfBytesWritten != data.Length)
            {
                if (protectedMemory)
                    VirtualProtectEx(processHandle, address, data.Length, oldProtect, out _);
                throw new System.Exception("Failed to write data to halo game process! Error code: " + GetLastError());
            }

            if (protectedMemory)
                VirtualProtectEx(processHandle, address, data.Length, oldProtect, out _);

        }


        public byte[] readData(Process process, int dataLength)
        {
            IntPtr processHandle = OpenProcess(readFlags, false, process.Id);
            if (processHandle == IntPtr.Zero)
                throw new System.Exception("Failed to open handle to halo game process with access perms: " + readFlags + ", Error code: " + GetLastError());

            IntPtr address = Resolve(processHandle, process.MainModule?.BaseAddress ?? throw new Exception("Could not access base address of halo game process"));

            byte[] buffer = new byte[dataLength];

            Log.Verbose(string.Format("Reading 0x{0:X} bytes from address: 0x{1:X}", dataLength, address));

            if (!ReadProcessMemory(processHandle, address, buffer, dataLength, out int lpNumberOfBytesRead) || lpNumberOfBytesRead != dataLength)
                throw new System.Exception("Failed to read data from halo game process! Error code: " + GetLastError());

            return buffer;
        }

        public static MultilevelPointer FromXML(XElement entry, HaloGame haloGame)
        {
            List<int> offsets = new();

            foreach(XElement offset in entry.Element("Offsets")?.Elements() ?? throw new Exception("MultilevelPointer xml missing Offsets"))
            {
                int val = Utility.ParseHexadecimalString(offset.Value);
                Log.Verbose("Adding offset to multilevelpointer: 0x" + val.ToString("X"));
                offsets.Add(val);
            }

            return new MultilevelPointer() { Game = haloGame, Offsets = offsets.ToArray() };
        }
    }



}
