using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using BurntMemory;
using System.Diagnostics;
using Reloaded.Assembler;
using HCM3.Services.Internal;

namespace HCM3.Services.HaloMemory
{ 
    public partial class HaloMemoryService
    {

        public byte[] DetourAssemble(string asmString, IntPtr startingAddress, Dictionary<string, object> symbolPointers, out int bytesAssembledTotal, out int statementsAssembledTotal)
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

            foreach (KeyValuePair<string, object> kv in symbolPointers)
            {
                if (kv.Key.StartsWith("$"))
                {
                    IntPtr symbolPointer = this.ReadWrite.ResolvePointer(kv.Value as ReadWrite.Pointer) ?? throw new Exception("Couldn't resolve pointer to symbol relative in ASM");
                    resolveSymbolsRelative.Add(kv.Key, symbolPointer);
                }
                else if (kv.Key.StartsWith("@"))
                {
                    IntPtr symbolPointer = this.ReadWrite.ResolvePointer(kv.Value as ReadWrite.Pointer) ?? throw new Exception("Couldn't resolve pointer to symbol absolute in ASM");
                    resolveSymbolsAbsolute.Add(kv.Key, symbolPointer);
                }
                else if (kv.Key.StartsWith("!"))
                {
                    if (!this.InternalServices.HCMInternalFunctionPointers.ContainsKey(kv.Key.Remove(0, 1))) throw new Exception("Couldn't resolve pointer to HCMInternal function, was missing");
                    IntPtr symbolPointer = this.InternalServices.HCMInternalFunctionPointers[kv.Key.Remove(0, 1)];
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

            using (Assembler asm = new Assembler())
            {
                foreach (string line in splitASMstring)
                {
                    string resolvedLine = line;
                    resolvedLine = resolvedLine.Replace("ptr", null); //keystone needed "ptr" mnemonic, reloaded does not. 
                    Trace.WriteLine("Assembling line (pre): " + resolvedLine);
                    // Resolve absolute symbols
                    foreach (KeyValuePair<string, IntPtr> entry in resolveSymbolsAbsolute)
                    {
                        if (line.Contains(entry.Key))
                        {
                            string newvalue = entry.Value.ToInt64().ToString("X") + "h";
                            newvalue = "qword " + newvalue; //reloaded needs qword mnemonic here

                            resolvedLine = resolvedLine.Replace(entry.Key, newvalue);

                            break;
                        }
                    }

                    bool NeedToPadTo6Bytes = false;
                    // Resolve rip-relative symbols
                    foreach (KeyValuePair<string, IntPtr> entry in resolveSymbolsRelative)
                    {
                        if (line.Contains(entry.Key))
                        {
                            string signstring = "";
                            long reljump = (entry.Value.ToInt64() - (long)rip);
                            if (reljump < 0)
                            {
                                Trace.WriteLine("Relative jump was negative: " + reljump.ToString("X"));
                                reljump = (long)rip - entry.Value.ToInt64();
                                signstring = "-";
                            } 
                            string resolved = signstring + "0" + reljump.ToString("X") + "h";
                            resolvedLine = resolvedLine.Replace(entry.Key, resolved);
                            NeedToPadTo6Bytes = true;
                            break;
                        }
                    }

                    Trace.WriteLine("Assembling line: " + resolvedLine);
                    // Assemble using keystone into bytes
                    byte[] resolvedBytes;

                    try
                    {
                        resolvedBytes = asm.Assemble(new string[] { "use64", resolvedLine });
                    }
                    catch (Reloaded.Assembler.Definitions.FasmException ex)
                    {
                        string err = "Error in FASM. Assembler result: " + ex.Result + ", line sent to assembler: " + ex.Mnemonics[1]  + ", error itself: " + ex.ErrorCode;
                        Trace.WriteLine(err);
                        throw new Exception (err);
                    }
                        int bytesAssembled = resolvedBytes.Length;

                    //for (int i = 0; i < bytesAssembled; i++)
                    //{
                    //    Trace.WriteLine(resolvedBytes[i].ToString("X"));
                    //}

                    //jmp instructions need to be always 6 bytes long for consistency
                    if (NeedToPadTo6Bytes && bytesAssembled < 6)
                    {
                        byte[] tempResolvedBytes = new byte[6];
                        Array.Fill(tempResolvedBytes, (byte)0x90);
                        Array.Copy(resolvedBytes, tempResolvedBytes, resolvedBytes.Length);
                        resolvedBytes = tempResolvedBytes;
                    }

                    // Add to all assembled bytes and increment bytesAssembledTotal, statementsAssembledTotal
                    assembledBytes = assembledBytes.Concat(resolvedBytes).ToArray();
                    bytesAssembledTotal += bytesAssembled;
                    statementsAssembledTotal++;
                    // Increment rip by bytes assembled
                    if (!NeedToPadTo6Bytes)
                    {
                        rip = rip + (ulong)bytesAssembled;
                    }
                    else
                    {
                        rip = rip + 6;
                    }

                }
            }

            if (assembledBytes.Length == 0) throw new Exception("Couldn't resolve any bytes from ASM string");


            //Keystone vs reloaded debugging
            //byte[] keystoneReturn = DetourAssembleKeystone(asmString, startingAddress, symbolPointers, out _, out _);
            //bool mismatch = false;
            //if (keystoneReturn.Length != assembledBytes.Length)
            //{
            //    Trace.WriteLine("RELOADED ASM ERROR: length of bytes was different: keystone length: " + keystoneReturn.Length + ", reloaded: " + assembledBytes.Length);
            //    mismatch = true;
            //}

            //for (int i = 0; i < assembledBytes.Length; i++)
            //{
            //    if (i >= keystoneReturn.Length) { break; }

            //    Trace.Write(assembledBytes[i].ToString("X") + " " + keystoneReturn[i].ToString("X"));
            //    if (assembledBytes[i] != keystoneReturn[i]) { Trace.Write("MISMATCH"); mismatch = true; }
            //    Trace.Write("\n");
            //}

            //if (mismatch) throw new Exception("MISMATCH BETWEEN RELOADED AND KEYSTONE ASSEMBLY");


            return assembledBytes;

        }


        //    public byte[] DetourAssembleKeystone(string asmString, IntPtr startingAddress, Dictionary<string, ReadWrite.Pointer> symbolPointers, out int bytesAssembledTotal, out int statementsAssembledTotal)
        //{

        //    byte[] assembledBytes = new byte[0];

        //    // Split up the string by instruction so we can process each one at a time
        //    char delimiter = ';';
        //    List<string> splitASMstring = asmString.Split(delimiter).ToList();

        //    // Instruction pointer at first instruction
        //    ulong rip = (ulong)startingAddress;

        //    // Get the addresses of the symbols in the string

        //    Dictionary<string, IntPtr> resolveSymbolsRelative = new();
        //    Dictionary<string, IntPtr> resolveSymbolsAbsolute = new();

        //    foreach (KeyValuePair<string, ReadWrite.Pointer> kv in symbolPointers)
        //    {
        //        if (kv.Key.StartsWith("$"))
        //        {
        //            IntPtr symbolPointer = this.HaloMemoryService.ReadWrite.ResolvePointer(kv.Value) ?? throw new Exception("Couldn't resolve pointer to symbol relative in ASM");
        //            resolveSymbolsRelative.Add(kv.Key, symbolPointer);
        //        }
        //        else if (kv.Key.StartsWith("@"))
        //        {
        //            IntPtr symbolPointer = this.HaloMemoryService.ReadWrite.ResolvePointer(kv.Value) ?? throw new Exception("Couldn't resolve pointer to symbol absolute in ASM");
        //            resolveSymbolsAbsolute.Add(kv.Key, symbolPointer);
        //        }
        //        else
        //        {
        //            throw new Exception("Symbol Pointer had invalid starting character");
        //        }
        //    }

        //    Trace.WriteLine("Attempting to assemble");

        //    bytesAssembledTotal = 0;
        //    statementsAssembledTotal = 0;

        //    using (Engine keystone = new Engine(Architecture.X86, Mode.X64) { ThrowOnError = true })
        //    {
        //        foreach (string line in splitASMstring)
        //        {
        //            string resolvedLine = line;

        //            // Resolve absolute symbols
        //            foreach (KeyValuePair<string, IntPtr> entry in resolveSymbolsAbsolute)
        //            {
        //                if (line.Contains(entry.Key))
        //                {
        //                    resolvedLine = resolvedLine.Replace(entry.Key, entry.Value.ToInt64().ToString("X") + "h");
        //                    break;
        //                }
        //            }

        //            bool NeedToPadTo6Bytes = false;
        //            // Resolve rip-relative symbols
        //            foreach (KeyValuePair<string, IntPtr> entry in resolveSymbolsRelative)
        //            {
        //                if (line.Contains(entry.Key))
        //                { 
        //                    string resolved = "0" + ((ulong)entry.Value.ToInt64() - rip).ToString("X") + "h";
        //                    resolvedLine = resolvedLine.Replace(entry.Key, resolved);
        //                    NeedToPadTo6Bytes = true;
        //                    break;
        //                }
        //            }

        //            Trace.WriteLine("Assembling line: " + resolvedLine);
        //            // Assemble using keystone into bytes
        //            byte[] resolvedBytes = keystone.Assemble(resolvedLine, 0, out int bytesAssembled, out _);
                    
        //            //jmp instructions need to be always 6 bytes long for consistency
        //            if (NeedToPadTo6Bytes && bytesAssembled < 6)
        //            {
        //                byte[] tempResolvedBytes = new byte[6];
        //                Array.Fill(tempResolvedBytes, (byte)0x90);
        //                Array.Copy(resolvedBytes, tempResolvedBytes, resolvedBytes.Length);
        //                resolvedBytes = tempResolvedBytes;
        //            }

        //            // Add to all assembled bytes and increment bytesAssembledTotal, statementsAssembledTotal
        //            assembledBytes = assembledBytes.Concat(resolvedBytes).ToArray();
        //            bytesAssembledTotal += bytesAssembled;
        //            statementsAssembledTotal++;
        //            // Increment rip by bytes assembled
        //            if (!NeedToPadTo6Bytes)
        //            {
        //                rip = rip + (ulong)bytesAssembled;
        //            }
        //            else
        //            {
        //                rip = rip + 6;
        //            }

        //        }
        //    }

        //    if (assembledBytes.Length == 0) throw new Exception("Couldn't resolve any bytes from ASM string");

        //    return assembledBytes;
        //}

    }
}
