
#include "includes.h"
#include <time.h>
#include <iostream>
#include "PlayerData.h"
#include <mutex>
#include <cmath>
#include <exception>


//DisplayInfoInfo struct that HCMExternal will pass information via
typedef struct
{
	UINT32 TeleportModeForward; // actually a bool
	UINT32 TeleportIgnoreZ; // actually a bool
	float TeleportX;
	float TeleportY;
	float TeleportZ;
	float TeleportLength;

}ForceTeleportArguments;






extern "C" __declspec(dllexport) BOOL ForceTeleport(ForceTeleportArguments ftargs) {
	try
	{
		std::cout << "Forcing a teleport!" << std::endl;

		if (IsPlayerDataLoaded())
		{
			throw std::runtime_error("ForceTeleport -> IsPlayerDataLoaded() failed, bailing.");
		}

		g_PlayerData_mutex.lock();

		void* pEntity = g_PlayerData->IsPlayerInVehicle() ? g_PlayerData->pVehicleEntity : g_PlayerData->pPlayerEntity;
		if (!g_PlayerData->IsEntityDataValid(pEntity))
		{
			std::cout << "ForceTeleport -> IsEntityDataValid() failed, bailing. Entity Address: " << pEntity << std::endl;
			g_PlayerData_mutex.unlock();
			return FALSE;
		}

		XMFLOAT3* pEntityPosition = g_PlayerData->pEntityPosition(pEntity);

		if (ftargs.TeleportModeForward == 1)
		{
			//these units are in radians by default in all games
			float ViewHorizontal = *g_PlayerData->PDI.pViewHorizontal;
			float ViewVertical = *g_PlayerData->PDI.pViewVertical;
			g_PlayerData_mutex.unlock(); // done with this


			XMFLOAT3 OldPosition = *pEntityPosition;
			std::cout << "Address of entity position: " << std::hex << pEntityPosition << std::endl;
			std::cout << "OldPosition.x: " << OldPosition.x << std::endl;
			std::cout << "OldPosition.y: " << OldPosition.y << std::endl;
			std::cout << "OldPosition.z: " << OldPosition.z << std::endl;
			std::cout << "TeleportIgnoreZ value: " << ftargs.TeleportIgnoreZ << std::endl;

			// Now, let's do some math to figure out what the new position should be.
			XMFLOAT3 normalisedViewAngles;
			normalisedViewAngles.x = (float)(cos(ViewVertical) * cos(ViewHorizontal));
			normalisedViewAngles.y = (float)(cos(ViewVertical) * sin(ViewHorizontal));
			normalisedViewAngles.z = (float)(sin(ViewVertical));

			if (ftargs.TeleportIgnoreZ == 1)
			{
				// Need to renormalize X and Y viewangles, without Z
				float horizontalPortion = normalisedViewAngles.x + normalisedViewAngles.y;
				float renormalisationFactor = 1 / horizontalPortion;
				normalisedViewAngles.x = normalisedViewAngles.x * renormalisationFactor;
				normalisedViewAngles.y = normalisedViewAngles.y * renormalisationFactor;
			}

			XMFLOAT3 NewPosition;
			XMStoreFloat3(&NewPosition, XMVectorAdd(XMLoadFloat3(&OldPosition), XMVectorScale(XMLoadFloat3(&normalisedViewAngles), ftargs.TeleportLength)));
			if (ftargs.TeleportIgnoreZ == 1) { NewPosition.z = OldPosition.z; }
			*pEntityPosition = NewPosition; // Write the new position
			std::cout << "ForceTeleport forward succeeded." << std::endl;
			return TRUE;
		}
		else // Teleport to absolute position
		{
			g_PlayerData_mutex.unlock(); // done with this
			XMFLOAT3* NewPosition = new XMFLOAT3(ftargs.TeleportX, ftargs.TeleportY, ftargs.TeleportZ);
				* pEntityPosition = * NewPosition;
				std::cout << "ForceTeleport to position succeeded." << std::endl;
				return TRUE;
		}

		
		return TRUE;
	}
	catch (...)
	{
		std::cout << "Exception in ForceTeleport! Ex: " << what().c_str() << std::endl;
		g_PlayerData_mutex.unlock();
	}


}




