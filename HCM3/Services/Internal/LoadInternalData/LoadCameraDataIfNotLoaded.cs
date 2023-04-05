using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Services.HaloMemory;
using HCM3.Helpers;
using System.Diagnostics;
using BurntMemory;

namespace HCM3.Services.Internal
{

    public partial class InternalServices
    {

        /// <summary>
        /// Populates CameraDataInfo struct and sends it to HCMInternal.
        /// </summary>
        public void LoadCameraDataIfNotLoaded(HaloState haloState)
        {


            // Check if data already loaded
            InternalReturn returnVal = this.CallInternalFunction("IsCameraDataLoaded", null);
            if (returnVal == InternalReturn.True)
            {
                Trace.WriteLine("CameraData already loaded");
            }
            else
            {
                Trace.WriteLine("Loading camera data into internal");
                // Construct a camera data struct
                CameraDataInfo cameraDataInfo = new CameraDataInfo();

                cameraDataInfo.pCamera = ReadPointer("CameraData", haloState);
                cameraDataInfo.pTickCounter = ReadPointer("CurrentTickCount", haloState);

                cameraDataInfo.OffsetCameraPosition = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetCameraPosition", haloState) ?? throw new Exception($"Couldn't get OffsetCameraPosition");
                cameraDataInfo.OffsetCameraDirection = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetCameraDirection", haloState) ?? throw new Exception($"Couldn't get OffsetCameraDirection");
                cameraDataInfo.OffsetCameraVerticalFOV = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetCameraVerticalFOV", haloState) ?? throw new Exception($"Couldn't get OffsetCameraVerticalFOV");
                cameraDataInfo.OffsetCameraTestValue = (int?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_OffsetCameraTestValue", haloState) ?? throw new Exception($"Couldn't get OffsetCameraTestValue");

                cameraDataInfo.CameraTestValue = (uint?)this.DataPointersService.GetRequiredPointers($"{haloState.GameState.To2Letters()}_CameraTestValue", haloState) ?? throw new Exception($"Couldn't get CameraTestValue");


                returnVal = this.CallInternalFunction("LoadCameraData", cameraDataInfo);
                if (returnVal != InternalReturn.True) throw new Exception("Failed calling cameraDataInfo: " + returnVal);
            }





        }

 


    }
}
