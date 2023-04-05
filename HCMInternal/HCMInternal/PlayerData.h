#pragma once
#include <Windows.h>
#include <mutex>
#include <cmath>
extern "C" __declspec(dllexport) BOOL IsPlayerDataLoaded();
#include <DirectXMath.h>


using namespace DirectX;









//PlayerDataStruct that HCMExternal will pass to us
typedef struct
{
	UINT32* pPlayerDatum;
	UINT32* pPlayerVehiDatum;
	UINT32* pTickCounter;
	float* pViewHorizontal;
	float* pViewVertical;

	UINT32* pTestValue;
	UINT32 TestValue;

	INT64 CurrentGame;


  // How to read the data at the PlayerVehiAddress
  // Is populated by PlayerDataOffsetData via PointerData
   INT64 OffsetEntityTestValue;
   UINT32 EntityTestValue;


	INT64 OffsetEntityPosition;
	INT64 OffsetEntityVelocity;
	INT64 OffsetEntityHealth;
	INT64 OffsetEntityShield;
	INT64 OffsetPlayerVehicleDatum; // H1 use only
	INT64 OffsetEntityTrigPosition;


}PlayerDataInfo;




class PlayerData
{
private:



public:
	PlayerDataInfo PDI;
	bool Initialized;

	//Set by hkEntityLoop
	void* pPlayerEntity = NULL;
	void* pVehicleEntity = NULL;
	UINT32 H1_VehicleDatum = 0;


	// Main members
	XMFLOAT3* pEntityPosition(void* const& pEntity)
	{
		return (XMFLOAT3*)((uintptr_t)pEntity + PDI.OffsetEntityPosition);
	}
	XMFLOAT3* pEntityVelocity(void* const& pEntity)
	{
		return (XMFLOAT3*)((uintptr_t)pEntity + PDI.OffsetEntityVelocity);
	}

	float* pEntityHealth(void* const& pEntity)
	{
		return (float*)((uintptr_t)pEntity + PDI.OffsetEntityHealth);
	}

	float* pEntityShield(void* const& pEntity)
	{
		return (float*)((uintptr_t)pEntity + PDI.OffsetEntityShield);
	}

	UINT32* pPlayerVehicleDatum() // H1 use only
	{
		return (UINT32*)((uintptr_t)pPlayerEntity + PDI.OffsetPlayerVehicleDatum);
	}

	XMFLOAT3* pEntityTrigPosition(void* const& pEntity) // the "position" that triggers check for. eg for CE, this is located in Chiefs ass.
	{
		return (XMFLOAT3*)((uintptr_t)pEntity + PDI.OffsetEntityTrigPosition);
	} 


	// Tests for others to call
	bool IsPlayerInVehicle()
	{
		return (pVehicleEntity != NULL);
	}

	bool IsEntityDataValid(void* const& pEntity)
	{
		if (!IsPlayerDataLoaded()) return false;
		if (pEntity == nullptr || IsBadReadPtr(pEntity, 4)) return false;
		UINT32 ActualTestValue = *(UINT32*)((uintptr_t)pEntity + PDI.OffsetEntityTestValue);
		return PDI.EntityTestValue != 0 && ActualTestValue == PDI.EntityTestValue;
	}

	// Constructor
	PlayerData(PlayerDataInfo pdi)
	{
		PDI = pdi;
		Initialized = true;
	}

	PlayerData()
	{
		Initialized = false;
	}

	bool IsPlayerDataLoaded()
	{
		if (PDI.pTestValue == nullptr || IsBadReadPtr(PDI.pTestValue, 4)) return false;
		UINT32 ObservedTestValue = *PDI.pTestValue;
		return ObservedTestValue == PDI.TestValue;
	}

};


extern PlayerData* g_PlayerData;
extern std::mutex g_PlayerData_mutex; // mutex so Loading it and accessing it are exclusive

