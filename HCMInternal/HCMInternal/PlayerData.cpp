#include "PlayerData.h"
#include <iostream>
#include <iomanip>

std::string what(const std::exception_ptr& eptr = std::current_exception())
{
	if (!eptr) { throw std::bad_exception(); }
	std::string str = "";
	try { std::rethrow_exception(eptr); }
	catch (const std::exception& e) { str = e.what(); }
	catch (const std::string& e) { str = e; }
	catch (const char* e) { str = e; }
	catch (...) { str = "who knows"; }
	//std::wstring wstr(str.begin(), str.end());
	return str;
}




PlayerData* g_PlayerData = new PlayerData();
std::mutex g_PlayerData_mutex;




// So HCMExternal can check if the data is good or if it needs to load it again
extern "C" __declspec(dllexport) BOOL IsPlayerDataLoaded()
{
	return g_PlayerData->IsPlayerDataLoaded();
}


// HCMExternal passes in PlayerDataInfo to help load a new instance of PlayerData
extern "C" __declspec(dllexport) BOOL LoadPlayerData(PlayerDataInfo pdi)
{
	g_PlayerData_mutex.lock();
	g_PlayerData->PDI = pdi;
	g_PlayerData->Initialized = true;
	g_PlayerData_mutex.unlock();
	return TRUE;

}


bool hkEntityLoopDebugFlag = true;
/// <summary>
/// HCMExternal applies a detour to the games EntityLoop, which calls this hkEntityLoop with the current-evaluated-entities address & datum as parameters.
/// Here we store the entity address, if it is the player (or players' vehicle) entity that we care about.
/// NOTE -- this hook is very likely to be called multiple times by different game threads, the mutex is super important, even besides the interactions with other HCMInternal threads.
/// </summary>
/// <param name="entityAddress">Address of the currently evaluated entity, passed via RCX register.</param>
/// <param name="entityDatumExpanded">Address of the currently evaluated entity, passed via RDX register.</param>
/// <returns></returns>
extern "C" __declspec(dllexport) void WINAPI hkEntityLoop(void* entityAddress, UINT64 entityDatumExpanded)
{


	g_PlayerData_mutex.lock();
	if (!g_PlayerData->IsPlayerDataLoaded())
	{
		g_PlayerData_mutex.unlock();
		return;
	}

	UINT32 entityDatum = entityDatumExpanded; // We only need the lower 4 bytes

	switch (g_PlayerData->PDI.CurrentGame)
	{
	case 0: // Halo 1

		if (entityDatum == *g_PlayerData->PDI.pPlayerDatum && g_PlayerData->IsEntityDataValid(entityAddress)) // Current entity is our player?
		{

			if (hkEntityLoopDebugFlag)
			{
				std::cout << "Matching PlayerDatum: " << std::hex << *g_PlayerData->PDI.pPlayerDatum << std::endl;
				std::cout << "entityAddress: " << std::hex << entityAddress << std::endl;
				hkEntityLoopDebugFlag = false;
			}
			// Then store the address, and check if player is in vehicle (if so, store the datum for another loop)
			g_PlayerData->pPlayerEntity = *(&entityAddress) ;
			
			//g_PlayerData->pPlayerEntity = (uintptr_t)entityAddress;
			UINT32 playerVehicleDatum = *g_PlayerData->pPlayerVehicleDatum();
			if (playerVehicleDatum != 0 && playerVehicleDatum != 0xFFFFFFFF)
			{
				g_PlayerData->H1_VehicleDatum = playerVehicleDatum;
			}
			else
			{
				g_PlayerData->H1_VehicleDatum = NULL;
				g_PlayerData->pVehicleEntity = NULL;
			}
		}
		else if (g_PlayerData->H1_VehicleDatum != NULL && entityDatum == g_PlayerData->H1_VehicleDatum && g_PlayerData->IsEntityDataValid(entityAddress)) // Check if the current entity is our players vehicle
		{
			// If so, store it's address
			g_PlayerData->pVehicleEntity = entityAddress;
		}
		break;


	case 1: // Halo 2
		if (entityDatum == *g_PlayerData->PDI.pPlayerDatum) // Current entity is our player?
		{
			// Then store the address
			g_PlayerData->pPlayerEntity = entityAddress;
			// TODO: how to check if h2 player is in vehicle? for now we can just manip player entity
		}
		break;

	case 2: // Halo 3
	case 3: // Halo 3: ODST
		break;

	case 4: // Halo Reach;
		break;

	case 5: // Halo 4;
		break;

	default:
		g_PlayerData_mutex.unlock();
		return;
	}

	g_PlayerData_mutex.unlock();

}

