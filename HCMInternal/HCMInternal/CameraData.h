#pragma once
#include "includes.h"
//#include "3Drender.h"
#include <d3d11.h>
#include <DirectXMath.h>


using namespace DirectX;



const extern DirectX::XMFLOAT3 g_UpVec; 

//struct passed by HCMExternal
struct CameraDataInfo
{
	void* pCamera;
	UINT64* pTickCounter;
	INT64 OffsetCameraPosition;
	INT64 OffsetCameraDirection;
	INT64 OffsetCameraVerticalFOV;
	INT64 OffsetCameraTestValue;
	UINT32 CameraTestValue;

	//float MagicVertFactor;
	//float MagicAspectFactor;

};


// will be set by hookd3d on init/resize, equal to actual aspect ratio multiplied by MagicAspectFactor



// Actual loaded camera data
// Includes fields for per-frame precalculation of camera data
struct CameraData
{
		CameraDataInfo CDI;

		XMFLOAT3 cameraPositionDXf;

		XMFLOAT3 cameraDirectionDXf;

		XMFLOAT3 cameraLookatDXf;

		//XMFLOAT4X4* projectionMatrix;
		//XMFLOAT4X4* viewMatrix;

		XMFLOAT4X4 projectionMatrix;
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 viewProjectionMatrix;

		float cameraVerticalFOV; 
		float cameraHorizontalFOV;

		bool UpdateCameraData();
		
};

extern CameraData g_CameraData;
extern std::mutex g_CameraDataMutex;


extern "C" __declspec(dllexport) BOOL IsCameraDataLoaded();