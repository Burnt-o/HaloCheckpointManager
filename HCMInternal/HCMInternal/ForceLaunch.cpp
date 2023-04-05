
#include "includes.h"
#include <time.h>
#include <iostream>
#include "PlayerData.h"
#include <mutex>
#include <cmath>
#include <exception>
#include <stdexcept>
//#include <directx.h>


extern "C" __declspec(dllexport) BOOL ForceLaunch(float* speedInput) {

	try
	{

		float speed = *speedInput;
		std::cout << "Forcing a launch! Speed: " << speed << std::endl;

		if (IsPlayerDataLoaded())
		{
			throw std::runtime_error("ForceLaunch -> IsPlayerDataLoaded() failed, bailing.");
		}

		g_PlayerData_mutex.lock();

		void* pEntity = g_PlayerData->IsPlayerInVehicle() ? g_PlayerData->pVehicleEntity : g_PlayerData->pPlayerEntity;
		if (!g_PlayerData->IsEntityDataValid(pEntity))
		{
			std::cout << "ForceLaunch -> IsEntityDataValid() failed, bailing. Entity Address: " << pEntity << std::endl;
			g_PlayerData_mutex.unlock();
			return FALSE;
		}

		XMFLOAT3* pEntityVelocity = g_PlayerData->pEntityVelocity(pEntity);

		//these units are in radians by default in all games
		float ViewHorizontal = *g_PlayerData->PDI.pViewHorizontal;
		float ViewVertical = *g_PlayerData->PDI.pViewVertical;

		g_PlayerData_mutex.unlock(); // done with this

		XMFLOAT3 OldVelocity = *pEntityVelocity;
		std::cout << "Address of entity velocity: " << std::hex << pEntityVelocity << std::endl;
		std::cout << "OldVelocity.x: " << OldVelocity.x << std::endl;
		std::cout << "OldVelocity.y: " << OldVelocity.y << std::endl;
		std::cout << "OldVelocity.z: " << OldVelocity.z << std::endl;

		// Now, let's do some math to figure out what the new velocities should be.
		XMFLOAT3 normalisedViewAngles;
		normalisedViewAngles.x = (float)(cos(ViewVertical) * cos(ViewHorizontal));
		normalisedViewAngles.y = (float)(cos(ViewVertical) * sin(ViewHorizontal));
		normalisedViewAngles.z = (float)(sin(ViewVertical));

		XMFLOAT3 NewVelocity;
		 XMStoreFloat3(&NewVelocity, XMVectorAdd(XMLoadFloat3(&OldVelocity), XMVectorScale(XMLoadFloat3(&normalisedViewAngles), speed)));
		*pEntityVelocity = NewVelocity; // Write the new velocity
		std::cout << "ForceLaunch succeeded." << std::endl;
		return TRUE;
	}
	catch (...)
	{
		std::cout << "Exception in ForceLaunch! Ex: " << what().c_str() << std::endl;
		g_PlayerData_mutex.unlock();
	}


}




