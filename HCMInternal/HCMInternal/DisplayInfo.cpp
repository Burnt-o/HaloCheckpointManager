#include "includes.h"
//#include <Windows.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <mutex>
#include "PlayerData.h"
#include <stdexcept>
#include "draw-text.h"
//#include "renderer.h"




//DisplayInfoInfo struct that HCMExternal will pass information via
typedef struct
{
	float ScreenX;
	float ScreenY;
	int SignificantDigits;
	float FontSize;

}DisplayInfoArguments;

// Forward declaration
extern "C" __declspec(dllexport) BOOL EnableDisplayInfo(DisplayInfoArguments dii);
extern "C" __declspec(dllexport) BOOL DisableDisplayInfo();
void DrawDisplayInfo();


// We only need the one instance of it
DisplayInfoArguments g_DisplayInfoInfo = { 0, 0, 0, 0 };
std::mutex g_DisplayInfoInfo_mutex; // mutex so EnableDisplayInfo and DrawDisplayInfo don't access it at the same time 

bool g_DisplayInfoFlag = false; // Control flag


// HCMExternal calls this to turn on DisplayInfo, or update it's properties
// HCMExternal passes needed info in DII struct
extern "C" __declspec(dllexport) BOOL EnableDisplayInfo(DisplayInfoArguments dii)
{
	if (!g_hcmInitialised || !g_hookEnabled) return FALSE;

	g_DisplayInfoInfo_mutex.lock();
	g_DisplayInfoInfo.FontSize = dii.FontSize;
	g_DisplayInfoInfo.SignificantDigits = dii.SignificantDigits;
	g_DisplayInfoInfo.ScreenX = dii.ScreenX;
	g_DisplayInfoInfo.ScreenY = dii.ScreenY;
	g_DisplayInfoFlag = true;
	g_DisplayInfoInfo_mutex.unlock();
	return TRUE;

}

// HCMExternal calls this to turn off DisplayInfo
extern "C" __declspec(dllexport) BOOL DisableDisplayInfo()
{
	g_DisplayInfoFlag = false;
	return TRUE;
}

// HCMExternal calls this to check if it's on or off
extern "C" __declspec(dllexport) BOOL IsDisplayInfoEnabled()
{
	return g_DisplayInfoFlag;
}

// Gather player data using DII and format the display text
// TODO: add tickcounter pointer to DII so this function can check tickcounter, so that it can skip updating the message text if the tick hasn't incremented
void DrawDisplayInfo()
{



	if (!g_DisplayInfoFlag) return; // DisplayInfo isn't enabled
	g_DisplayInfoInfo_mutex.lock();
	float ScreenX = g_DisplayInfoInfo.ScreenX;
	float ScreenY = g_DisplayInfoInfo.ScreenY;
	int SignificantDigits = g_DisplayInfoInfo.SignificantDigits;
	float FontSize = g_DisplayInfoInfo.FontSize;

	g_DisplayInfoInfo_mutex.unlock();
	g_PlayerData_mutex.lock();
	std::string currentMessageText;

	XMFLOAT3 EntityPosition;
	XMFLOAT3 EntityVelocity;
	float EntityHealth = 0;
	float EntityShield = 0;
	float ViewHorizontal = 0;
	float ViewVertical = 0;
	bool IsPlayerInVehicle = false;

	try
	{
		if (!IsPlayerDataLoaded())
		{
			throw std::runtime_error("DrawDisplayInfo: IsPlayerDataLoaded() returned false");
		}

		IsPlayerInVehicle = g_PlayerData->IsPlayerInVehicle();

		void* pEntity = IsPlayerInVehicle ? g_PlayerData->pVehicleEntity : g_PlayerData->pPlayerEntity;
		if (!g_PlayerData->IsEntityDataValid(pEntity))
		{
			std::ostringstream er;
			er << "DrawDisplayInfo: IsEntityDataValid() returned false, entity address: " << pEntity;
			throw std::runtime_error(er.str());
		}


		EntityPosition = *g_PlayerData->pEntityPosition(&pEntity);
		EntityVelocity = *g_PlayerData->pEntityVelocity(&pEntity);

		EntityHealth = *g_PlayerData->pEntityHealth(&pEntity);
		EntityShield = *g_PlayerData->pEntityShield(&pEntity);

		//these units are in radians by default in all games
		ViewHorizontal = *g_PlayerData->PDI.pViewHorizontal;
		ViewVertical = *g_PlayerData->PDI.pViewVertical;

	}
	catch (...)
	{
		std::ostringstream er;
		er << L"Exception in DrawDisplayInfo! Ex: " << what() << std::endl;
		currentMessageText = er.str();

	}
	g_PlayerData_mutex.unlock(); // done with this

		//derived values
		float temp = (EntityVelocity.x * EntityVelocity.x) + (EntityVelocity.y * EntityVelocity.y);
		float xyVel = 0;
		float xyzVel = 0;
		if (temp > 0)
		{
			xyVel = sqrt(temp);
			xyzVel = sqrt(temp + (EntityVelocity.z * EntityVelocity.z));
		}



		//start writing the display info string
		std::stringstream stream;
		stream << (IsPlayerInVehicle == true ? "Vehicle Data: \n" : "Player Data: \n");
		stream << "Position: ";
		stream << std::fixed << std::setprecision(SignificantDigits) << EntityPosition.x << ", ";
		stream << std::fixed << std::setprecision(SignificantDigits) << EntityPosition.y << ", ";
		stream << std::fixed << std::setprecision(SignificantDigits) << EntityPosition.z << "\n";

		stream << "Velocity: ";
		stream << std::fixed << std::setprecision(SignificantDigits) << EntityVelocity.x << ", ";
		stream << std::fixed << std::setprecision(SignificantDigits) << EntityVelocity.y << ", ";
		stream << std::fixed << std::setprecision(SignificantDigits) << EntityVelocity.z << "\n";
		stream << "Total XY Velocity: ";
		stream << std::fixed << std::setprecision(SignificantDigits) << xyVel << "\n";
		stream << "Total XYZ Velocity: ";
		stream << std::fixed << std::setprecision(SignificantDigits) << xyzVel << "\n";
		stream << "Health: ";
		stream << std::fixed << std::setprecision(SignificantDigits) << EntityHealth << ", ";
		stream << std::fixed << std::setprecision(SignificantDigits) << EntityShield << "\n";
		stream << "View Angle: ";
		stream << std::fixed << std::setprecision(SignificantDigits) << ViewHorizontal << ", ";
		stream << std::fixed << std::setprecision(SignificantDigits) << ViewVertical << "\n";

		currentMessageText = stream.str() + currentMessageText;

		//Draw it
		//renderer->drawOutlinedText(Vec2(ScreenX, ScreenY), currentMessageText, mainTextColor, FontSize);



	




	

}