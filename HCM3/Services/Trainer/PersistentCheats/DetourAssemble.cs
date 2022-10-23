using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Keystone;
using BurntMemory;
using System.Diagnostics;

namespace HCM3.Services.Trainer
{
    public partial class PersistentCheatService
    {

        public byte[] DetourAssemble(string asmString, IntPtr startingAddress, Dictionary<string, ReadWrite.Pointer> symbolPointers, out int bytesAssembledTotal, out int statementsAssembledTotal)
        {

            byte[] assembledBytes = new byte[0];

            // Split up the string by instruction so we can process each one at a time
            char delimiter = ';';
            List<string> splitASMstring = asmString.Split(delimiter).ToList();

            // Instruction pointer at first instruction
            ulong rip = (ulong)startingAddress;

            // Get the addresses of the symbols in the string

            Dictionary<string, IntPtr> resolveSymbolsRelative = new();
            Dictionary<string, IntPtr> resolveSymbolsAbsolute = new();

            foreach (KeyValuePair<string, ReadWrite.Pointer> kv in symbolPointers)
            {
                if (kv.Key.StartsWith("$"))
                {
                    IntPtr symbolPointer = this.HaloMemoryService.ReadWrite.ResolvePointer(kv.Value) ?? throw new Exception("Couldn't resolve pointer to symbol relative in ASM");
                    resolveSymbolsRelative.Add(kv.Key, symbolPointer);
                }
                else if (kv.Key.StartsWith("@"))
                {
                    IntPtr symbolPointer = this.HaloMemoryService.ReadWrite.ResolvePointer(kv.Value) ?? throw new Exception("Couldn't resolve pointer to symbol absolute in ASM");
                    resolveSymbolsAbsolute.Add(kv.Key, symbolPointer);
                }
                else
                {
                    throw new Exception("Symbol Pointer had invalid starting character");
                }
            }

            Trace.WriteLine("Attempting to assemble");

            bytesAssembledTotal = 0;
            statementsAssembledTotal = 0;

            using (Engine keystone = new Engine(Architecture.X86, Mode.X64) { ThrowOnError = true })
            {
                foreach (string line in splitASMstring)
                {
                    string resolvedLine = line;

                    // Resolve absolute symbols
                    foreach (KeyValuePair<string, IntPtr> entry in resolveSymbolsAbsolute)
                    {
                        if (line.Contains(entry.Key))
                        {
                            resolvedLine = resolvedLine.Replace(entry.Key, entry.Value.ToInt64().ToString("X") + "h");
                        }
                    }

                    // Resolve rip-relative symbols
                    foreach (KeyValuePair<string, IntPtr> entry in resolveSymbolsRelative)
                    {
                        if (line.Contains(entry.Key))
                        { 
                            string resolved = "0" + ((ulong)entry.Value.ToInt64() - rip).ToString("X") + "h";
                            resolvedLine = resolvedLine.Replace(entry.Key, resolved);
                        }
                    }

                    Trace.WriteLine("Assembling line: " + resolvedLine);
                    // Assemble using keystone into bytes
                    byte[] resolvedBytes = keystone.Assemble(resolvedLine, 0, out int bytesAssembled, out _);
                    // Add to all assembled bytes and increment bytesAssembledTotal, statementsAssembledTotal
                    assembledBytes = assembledBytes.Concat(resolvedBytes).ToArray();
                    bytesAssembledTotal += bytesAssembled;
                    statementsAssembledTotal++;
                    // Increment rip by bytes assembled
                    rip = rip + (ulong)bytesAssembled;

                }
            }

            if (assembledBytes.Length == 0) throw new Exception("Couldn't resolve any bytes from ASM string");

            return assembledBytes;
        }

    }
}
