using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using BurntMemory;
using System.Runtime.InteropServices;


namespace HCM3.Services.Trainer
{
    public partial class InternalServices
    {


        private object lockCallInternalFunction = new object();
        public uint? CallInternalFunction(string functionName, object? param)
        {
            lock (lockCallInternalFunction)
            {
                Trace.WriteLine("CallInternalFunction, params: " + functionName + ", " + param?.ToString());
                Process MCCProcess = Process.GetProcessById((int)this.HaloMemoryService.HaloState.ProcessID);
                IntPtr? paramMemAddress = null;
                int? paramSizeToAlloc = null;
                IntPtr? remoteThreadHandle = null;
                // Try-Finally so we can free the above resources if something goes wrong
                try
                {
                    IntPtr functionPointer;

                    if (functionName == "SetSpeedHack")
                    {
                        functionPointer = this.setAllToSpeed;
                    }
                    else if (functionName == "GetSpeedHack")
                    {
                        functionPointer = this.getSpeed;
                    }
                    else
                    {
                        if (!InternalFunctions.TryGetValue(functionName, out functionPointer))
                        {
                            throw new ArgumentException("Couldn't find functionPointer of name: " + functionName);
                        }
                    }


                    // Now check if there is a param. Convert it to bytes if it's a valid type.
                    byte[]? paramData = null;
                    switch (param)
                    {
                        case string:
                            paramData = Encoding.Default.GetBytes((string)param);
                            break;

                        case float:
                            paramData = BitConverter.GetBytes((float)param);
                            break;

                        case double:
                            paramData = BitConverter.GetBytes((double)param);
                            break;


                        case ulong:
                            paramData = BitConverter.GetBytes((ulong)param);
                            break;

                        case DisplayInfoInfo:
                            int size = System.Runtime.InteropServices.Marshal.SizeOf(typeof(DisplayInfoInfo));
                            paramData = new byte[size];

                            IntPtr ptr = IntPtr.Zero;
                            try
                            {
                                ptr = Marshal.AllocHGlobal(size);
                                Marshal.StructureToPtr((DisplayInfoInfo)param, ptr, true);
                                Marshal.Copy(ptr, paramData, 0, size);
                            }
                            finally
                            {
                                Marshal.FreeHGlobal(ptr);
                            }
                            Trace.WriteLine("Size of DisplayInfoInfo byte array:" + size);
                            break;

                        default:
                            break;
                    }




                    // Now we need to write the param bytes somewhere in MCC memory
                    if (paramData != null)
                    {

                        paramSizeToAlloc = (paramData.Length + 1) * Marshal.SizeOf(typeof(char));
                        Trace.WriteLine("paramData is not null, length is: " + paramSizeToAlloc);

                        foreach (byte paramByte in paramData)
                        {
                            Trace.WriteLine("paramByte: " + paramByte.ToString("X2"));
                        }

                        paramMemAddress = PInvokes.VirtualAllocEx(MCCProcess.Handle, IntPtr.Zero, (uint)paramSizeToAlloc, PInvokes.ALLOC_FLAGS.MEM_COMMIT | PInvokes.ALLOC_FLAGS.MEM_RESERVE, PInvokes.ALLOC_FLAGS.PAGE_READWRITE);
                        Trace.WriteLine("paramMemAddress: " + paramMemAddress);
                        bool success = this.HaloMemoryService.ReadWrite.WriteBytes(new ReadWrite.Pointer(paramMemAddress), (byte[])paramData, true);
                        Trace.WriteLine("successfully wrote bytes? " + success.ToString());
                    }
                    else
                    {
                        // No parameter
                        Trace.WriteLine("paramData is null!");
                        paramMemAddress = IntPtr.Zero;
                    }

                    // Now create a remote thread at the target function address, with paramMemAddress as the param to read
                    // Note: We can only send one parameter with this method. If we want to send "more", we need to use a struct containing the param
                    Trace.WriteLine("function Pointer we're trying to call: " + functionPointer);
                    Trace.WriteLine("function parameter addy we're trying to pass: " + paramMemAddress);


                    remoteThreadHandle = PInvokes.CreateRemoteThread(MCCProcess.Handle, IntPtr.Zero, 0, functionPointer, (IntPtr)paramMemAddress, 0, IntPtr.Zero);

                    // Wait for the thread to finish executing (or some timeout)
                    uint waitFor = PInvokes.WaitForSingleObject((IntPtr)remoteThreadHandle, 3000);



                    // Check if the thread completed
                    if (waitFor == 0x00000080) // WAIT_ABANDONED
                    {
                        throw new Exception("Remote thread failed unexpectedly");
                    }
                    else if (waitFor == 0x00000102) // WAIT_TIMEOUT
                    {
                        throw new Exception("Remote thread timed out");
                    }

                    // Looks like the thread completed, let's get it's exit code
                    PInvokes.GetExitCodeThread((IntPtr)remoteThreadHandle, out uint exitCode);

                    if (exitCode == 0)
                    {
                        // Null return. Either a void function was called or the function failed.
                        int lastError = Marshal.GetLastWin32Error();
                        Trace.WriteLine("Remote Thread exited zero. If failure, last error was: " + lastError);
                        return 0;
                    }
                    else
                    {
                        // IMPORTANT: can only return up to 32 bytes
                        // So I hope the return value you care about fits in that

                        return exitCode;
                    }


                }
                finally
                {
                    if (paramMemAddress != null && paramMemAddress != IntPtr.Zero && paramSizeToAlloc != null)
                    {
                        // Free allocated memory
                        PInvokes.VirtualFreeEx(MCCProcess.Handle, (IntPtr)paramMemAddress, (int)paramSizeToAlloc, PInvokes.AllocationType.Release);
                    }
                    if (remoteThreadHandle != null)
                    {
                        // Close handle to remote thread
                        PInvokes.CloseHandle((IntPtr)remoteThreadHandle);
                    }

                }

            }

         }       
        }

    }

