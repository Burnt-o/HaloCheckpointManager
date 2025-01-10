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
using Serilog;

namespace HCMExternal.Services.External.Impl
{
    internal partial class ExternalService : IExternalService
    {

        // MCC stores checkpoints in its process memory; read/write is as simple as Read(Write)ProcessMemory https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-readprocessmemory
        private void DumpCheckpointMCC(SaveFolder saveFolder, HaloProcessInfo haloProcess)
        {
            // get inject requirements
            InjectRequirements injectRequirements = PointerData.GetGameProcessData<InjectRequirements>(haloProcess.processType, haloProcess.haloGame, "InjectRequirements", haloProcess.processVersion);

            throw new NotImplementedException("MCC external dumping not implemented. Use internal overlay instead.");
        }

        // Cartographer stores checkpoints on disk with non-shared file handles. We need to duplicate the handles then map them to our memory so we can read/write to them without messing with the games filestream pointer.
        private void DumpCheckpointCartographer(SaveFolder saveFolder, HaloProcessInfo haloProcess)
        {

            // force a checkpoint. TODO: make this optional.
            ForceCheckpoint();
            System.Threading.Thread.Sleep(100);

            // get pointer to double revert flag
            IMultilevelPointer doubleRevertFlagPointer = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, "DoubleRevertFlag", haloProcess.processVersion);

            // use it to figure which file handle we want
            string desiredCheckpointFileHandle = doubleRevertFlagPointer.readData(haloProcess.processHandle, 1)[0] == 0 ?
                "CheckpointFileHandleA" :
                "CheckpointFileHandleB";

            // get pointer to checkpoint file handle
            IMultilevelPointer CheckpointFileHandlePointer = PointerData.GetGameProcessData<IMultilevelPointer>(haloProcess.processType, haloProcess.haloGame, desiredCheckpointFileHandle, haloProcess.processVersion);

            // read it
            UInt32 CheckpointFileHandle = BitConverter.ToUInt32(CheckpointFileHandlePointer.readData(haloProcess.processHandle, sizeof(UInt32)));

            // get length of checkpoint data
            UInt32 checkpointLength = (UInt32)PointerData.GetGameProcessData<int>(haloProcess.processType, haloProcess.haloGame, "CheckpointLength", haloProcess.processVersion);

            // map checkpoint file handle to memory
            var unmanagedCheckpointData = MappedCartographerSaveFactory.make(CheckpointFileHandle, haloProcess.processHandle, checkpointLength);

            // copy data
            byte[] managedBuffer = new byte[checkpointLength];
            Marshal.Copy(unmanagedCheckpointData.data(), managedBuffer, 0, (int)checkpointLength);

            // write game version metadata to (eof - 10)
            byte[] versionStringChars = Encoding.ASCII.GetBytes(haloProcess.processVersion);
            Array.Copy(versionStringChars, 0, managedBuffer, checkpointLength - 10, versionStringChars.Length);

            // Ask user what they want to name the checkpoint file
            string checkpointFileName = Microsoft.VisualBasic.Interaction.InputBox(@"Must be unique, no fancy characters",
                                                       "Name your dumped checkpoint",
                                                       "",
                                                       -1, -1);

            if (checkpointFileName == "") // user clicked cancel button
                return;

            string checkpointFilePath = (saveFolder.SaveFolderPath + "\\" +  checkpointFileName + ".bin");
            Log.Verbose("Dumping checkpoint to path: " + checkpointFilePath);


            if (File.Exists(checkpointFilePath) &&
                System.Windows.MessageBox.Show("Checkpoint file of name '" + checkpointFileName + "' already exists. Overwrite file?",
                    "Overwrite file?",
                    System.Windows.MessageBoxButton.YesNo,
                    System.Windows.MessageBoxImage.Warning
                    ) == System.Windows.MessageBoxResult.No)
                return;


            File.WriteAllBytes(checkpointFilePath, managedBuffer); // write the checkpoint to disk

        }


        public void DumpCheckpoint(SaveFolder saveFolder)
        {
            // get process
            HaloProcessInfo haloProcess = GetHaloProcessInfo();

            if (haloProcess.processType == HaloProcessType.ProjectCartographer)
                DumpCheckpointCartographer(saveFolder, haloProcess);
            else
                DumpCheckpointMCC(saveFolder, haloProcess);

        }

    }
}
