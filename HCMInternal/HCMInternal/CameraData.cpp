#include "CameraData.h"
#include <iostream>
//CameraDataInfo g_CameraDataInfo;
//std::mutex g_CameraDataInfoMutex;

CameraData g_CameraData;
std::mutex g_CameraDataMutex;
const extern DirectX::XMFLOAT3 g_UpVec = { 0.0f, 0.0f, 1.0f };


extern "C" __declspec(dllexport) BOOL LoadCameraData(CameraDataInfo cdi);



float fixCEHorizontalFOV = 1.00f;

bool firstDebugFrameCameraData = true;
bool CameraData::UpdateCameraData()// Called once per frame, reads data from memory and pre-calculates common data for WorldToScreen
{
	// Read camera position
	void* pCamera = g_CameraData.CDI.pCamera;
	float* camPos = (float*)((uintptr_t)pCamera + g_CameraData.CDI.OffsetCameraPosition);
	this->cameraPositionDXf = { *camPos, *(camPos + 1), *(camPos + 2) };

	if (firstDebugFrameCameraData) std::cout << "cameraPositionDXf: " << this->cameraPositionDXf.x << ", " << this->cameraPositionDXf.y << ", " << this->cameraPositionDXf.z << std::endl;

	// Read camera direction
	float* camDir = (float*)((uintptr_t)pCamera + g_CameraData.CDI.OffsetCameraDirection);
	this->cameraDirectionDXf = { *camDir, *(camDir + 1), *(camDir + 2) };
	if (firstDebugFrameCameraData) std::cout << "cameraDirectionDXf: " << this->cameraDirectionDXf.x << ", " << this->cameraDirectionDXf.y << ", " << this->cameraDirectionDXf.z << std::endl;
	
	// Read camera vertical FOV
	this->cameraVerticalFOV = *(float*)((uintptr_t)pCamera + g_CameraData.CDI.OffsetCameraVerticalFOV); // need to multiply to get from vfov to hfov
	if (firstDebugFrameCameraData) std::cout << "cameraVerticalFOV: " << this->cameraVerticalFOV << std::endl;

	// Calculate horizontal FOV
	this->cameraHorizontalFOV = 2 * atan(tan(this->cameraVerticalFOV / 2) * g_aspectRatio) * fixCEHorizontalFOV;
	if (firstDebugFrameCameraData) std::cout << "cameraHorizontalFOV: " << this->cameraHorizontalFOV << std::endl;

	// Calculate camera lookAtPoint (position + direction)
	XMVECTOR cameraLookat = XMVectorAdd(XMLoadFloat3(&this->cameraPositionDXf), XMLoadFloat3(&this->cameraDirectionDXf));
	XMStoreFloat3(&this->cameraLookatDXf, cameraLookat);
	if (firstDebugFrameCameraData) std::cout << "cameraLookatDXv: " << this->cameraLookatDXf.x << ", " << this->cameraLookatDXf.y << ", " << this->cameraLookatDXf.z << std::endl;

	// Calculate Projection Matrix
	XMMATRIX proj = DirectX::XMMatrixPerspectiveFovRH(this->cameraHorizontalFOV / 2, g_aspectRatio, 0.1f, 1.0f);
	XMStoreFloat4x4(&this->projectionMatrix, proj);

	// Calculate View Matrix
	DirectX::XMFLOAT3 vUpVec(0.0f, 0.0f, 1.0f);
	XMMATRIX view = DirectX::XMMatrixLookAtRH(DirectX::XMLoadFloat3(&this->cameraPositionDXf), cameraLookat, DirectX::XMLoadFloat3(&vUpVec));
	XMStoreFloat4x4(&this->viewMatrix, view);

	// Calculate View-Projection Matrix (just multiply them together)
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMStoreFloat4x4(&this->viewProjectionMatrix, viewProj);


	if (firstDebugFrameCameraData) firstDebugFrameCameraData = false;
	return true;
}


extern "C" __declspec(dllexport) BOOL IsCameraDataLoaded()
{
	g_CameraDataMutex.lock();
	void* pCamera = g_CameraData.CDI.pCamera;
	if (pCamera == nullptr || IsBadReadPtr(pCamera, 4))
	{
		g_CameraDataMutex.unlock();
		return FALSE;
	}

	void* pTickcounter = g_CameraData.CDI.pTickCounter;
	if (pTickcounter == nullptr || IsBadReadPtr(pTickcounter, 4))
	{
		g_CameraDataMutex.unlock();
		return FALSE;
	}

	UINT64* pTestValue = (UINT64*)((uintptr_t)pCamera + g_CameraData.CDI.OffsetCameraTestValue);
	if (IsBadReadPtr(pTestValue, 4))
	{
		g_CameraDataMutex.unlock();
		return FALSE;
	}
	UINT64 TestValueObserved = *pTestValue;
	UINT64 TestValueExpected = g_CameraData.CDI.CameraTestValue;

	g_CameraDataMutex.unlock();
	return (TestValueObserved == TestValueExpected);
}


extern "C" __declspec(dllexport) BOOL LoadCameraData(CameraDataInfo cdi)
{
	g_CameraDataMutex.lock();
	g_CameraData.CDI = cdi;
	g_CameraDataMutex.unlock();



	return IsCameraDataLoaded();
}