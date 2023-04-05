#pragma once
//#include "PlayerData.h"
//#include "imgui/imgui.h"
//#include "imgui/imgui_impl_win32.h"
//#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <string>
#include <vector>
#include <mutex>
using namespace DirectX;




struct TriggerDataInfo
{
	
	void* pFirstTrigger = nullptr; // Pointer to the first trigger
	UINT64 TriggerStride; // How many bytes between each trigger 0x60 for ce (assembly tells ya)
	UINT64 TriggerCount; // How many triggers there are


	// Offsets for reading the trigger data
	INT32 OffsetTriggerName;
	UINT64 TriggerNameLength;
	INT32 OffsetTriggerPosition;
	INT32 OffsetTriggerExtents;
	INT32 OffsetTriggerRotForward;
	INT32 OffsetTriggerRotUp;

	// For testing validity
	INT32 OffsetTriggerTestValue;
	UINT32 TriggerTestValue;

	// For trigger search hooking
	UINT32* Tickcounter = nullptr;
	void* p_oTriggerHitTest = nullptr;

};


struct Trigger
{
	uintptr_t pGameTriggerData;
	UINT32 triggerIndex;
	std::string triggerName = "Missing trigger name";
	UINT64 tickLastSearched = 0;
	bool excludedByFilter = false; // not sure if we do it like this or use another map/vector/array

	bool playerIsInsideMe = false; // stored since it only needs to be checked once per tick, not once per frame like cameraIsInsideMe is.
	bool playerWasInsideOnLastSearch = false;
	BoundingBox boundingBox; // for quick hit testing
	BoundingOrientedBox boundingOrientedBox; // for accurate hit testing

	// Useful for debug
	XMFLOAT3 Position;
	XMFLOAT3 Extents;
	XMFLOAT3 RotRight;
	XMFLOAT3 RotForward;
	XMFLOAT3 RotUp;

	// cube vertex layout: https://www.math.brown.edu/tbanchof/Beyond3d/Images/chapter8/image04.jpg
	// order we will store them:
	//	0v000, 1v100, 2v110, 3v010, 4v001, 5v101, 6v111, 7v011
	XMFLOAT3 vertexes[8];

};


extern std::vector<Trigger> g_AllTriggers;
extern std::vector<Trigger> g_IncludedTriggers; // g_AllTriggers after being filtered by triggername


extern TriggerDataInfo g_TriggerDataInfo;
extern std::mutex g_TriggerDataInfoMutex;

extern void TriggerDataOnFrameDebug();
