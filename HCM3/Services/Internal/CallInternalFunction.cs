using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using BurntMemory;
using System.Runtime.InteropServices;
using HCM3.Helpers;


namespace HCM3.Services.Internal
{
    // Error codes for CallInternalFunction
    public enum InternalReturn
    {
        False = 0,
        True = 1,
        RemoteThreadTimedOut = -1,
        RemoteThreadFailed = -2,
        MissingFunctionPointer = -3,
        CallInternalFunctionException = -4,
        ProcessIDNull = -5

    }


    public partial class InternalServices
    {


        private object lockCallInternalFunction = new object();
        public InternalReturn CallInternalFunction(string functionName, object? param)
        {
            lock (lockCallInternalFunction)
            {
                Trace.WriteLine("CallInternalFunction called, params: " + functionName + ", " + param?.ToString());

                Process? MCCProcess = null;
                IntPtr? paramMemAddress = null;
                int? paramSizeToAlloc = null;
                IntPtr? remoteThreadHandle = null;
                // Try-Finally so we can free the above resources if something goes wrong
                try
                {
                    if (this.HaloMemoryService.HaloMemoryManager.ProcessID == null)
                    {
                        Trace.WriteLine("ProcessID was null, returning");
                        return InternalReturn.ProcessIDNull;
                    }
                        
                        
                        
                    MCCProcess = Process.GetProcessById((int)this.HaloMemoryService.HaloMemoryManager.ProcessID);
                    if (MCCProcess == null) throw new Exception("MCCProcess was null. This shouldn't happen after the processID check, processID must be out of date.");

                    IntPtr functionPointer;
                    if (!HCMInternalFunctionPointers.TryGetValue(functionName, out functionPointer))
                    {
                        Trace.WriteLine("Couldn't find functionPointer of name: " + functionName);
                        return InternalReturn.MissingFunctionPointer;
                    }

                    int size = 0;
                    IntPtr ptr = IntPtr.Zero;

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

                        case IntPtr:
                        case ulong:
                            if (param.GetType() == typeof(IntPtr))
                                param = (ulong)((IntPtr)param).ToInt64();

                            paramData = BitConverter.GetBytes((ulong)param);
                            break;


                        case byte[]:
                            paramData = (byte[])param;
                            Trace.WriteLine("Length of byte[] param" + paramData.Length);
                            break;

                            //Structs
                        case ForceTeleportArguments:
                        case DisplayInfoArguments:
                        case PlayerDataInfo:
                        case TriggerDataInfo:
                        case CameraDataInfo:
                            size = System.Runtime.InteropServices.Marshal.SizeOf(param.GetType());
                            paramData = new byte[size];

                            ptr = IntPtr.Zero;
                            try
                            {
                                ptr = Marshal.AllocHGlobal(size);
                                Marshal.StructureToPtr(param, ptr, true);
                                Marshal.Copy(ptr, paramData, 0, size);
                            }
                            finally
                            {
                                Marshal.FreeHGlobal(ptr);
                            }
                            Trace.WriteLine("Size of DisplayInfoInfo byte array:" + size);
                            break;

                        case null:
                            break;

                        

                            break;

                        

                        default:
                            throw new Exception("CallInternalFunction was passed an invalid parameter type: " + param.GetType());
                    }




                    // Now we need to write the param bytes somewhere in MCC memory
                    if (paramData != null)
                    {

                        paramSizeToAlloc = (paramData.Length + 1) * Marshal.SizeOf(typeof(char));
                        //Trace.WriteLine("paramData is not null, length is: " + paramSizeToAlloc);

                        //foreach (byte paramByte in paramData)
                        //{
                        //    Trace.WriteLine("paramByte: " + paramByte.ToString("X2"));
                        //}

                        paramMemAddress = PInvokes.VirtualAllocEx(MCCProcess.Handle, IntPtr.Zero, (uint)paramSizeToAlloc, PInvokes.ALLOC_FLAGS.MEM_COMMIT | PInvokes.ALLOC_FLAGS.MEM_RESERVE, PInvokes.ALLOC_FLAGS.PAGE_READWRITE);
                        //Trace.WriteLine("paramMemAddress: " + paramMemAddress);
                        bool success = this.HaloMemoryService.ReadWrite.WriteBytes(new ReadWrite.Pointer(paramMemAddress), (byte[])paramData, true);
                        //Trace.WriteLine("successfully wrote bytes? " + success.ToString());
                    }
                    else
                    {
                        // No parameter
                        //Trace.WriteLine("paramData is null!");
                        paramMemAddress = IntPtr.Zero;
                    }

                    // Now create a remote thread at the target function address, with paramMemAddress as the param to read
                    // Note: We can only send one parameter with this method. If we want to send "more", we need to use a struct containing the param
                    //Trace.WriteLine("function Pointer we're trying to call: " + functionPointer);
                    //Trace.WriteLine("function parameter addy we're trying to pass: " + paramMemAddress);

                    //TODO: Handle error of process closing mid-request
                    remoteThreadHandle = PInvokes.CreateRemoteThread(MCCProcess.Handle, IntPtr.Zero, 0, functionPointer, (IntPtr)paramMemAddress, 0, IntPtr.Zero);

                    // Wait for the thread to finish executing (or some timeout)
                    uint waitFor = PInvokes.WaitForSingleObject((IntPtr)remoteThreadHandle, 1000);



                    // Check if the thread completed
                    if (waitFor == 0x00000080) // WAIT_ABANDONED
                    {
                        Trace.WriteLine("Remote thread failed unexpectedly");
                        return InternalReturn.RemoteThreadFailed;
                    }
                    else if (waitFor == 0x00000102) // WAIT_TIMEOUT
                    {
                        Trace.WriteLine("Remote thread timed out");
                        return InternalReturn.RemoteThreadTimedOut;
                    }

                    // Looks like the thread completed, let's get it's exit code
                    PInvokes.GetExitCodeThread((IntPtr)remoteThreadHandle, out uint exitCode);

                    return (InternalReturn)exitCode;


                }
                catch (Exception ex)
                {
                    Trace.WriteLine("CallInternalFunction failed unexpectedly, functionName: " + functionName + ", param: " + param + ", ex: " + ex.ToString());
                    return InternalReturn.CallInternalFunctionException;
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

