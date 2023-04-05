#include "includes.h"
//#include <Windows.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>   
#include <d3dx11.h>
#include <sstream>
#include <iomanip>
#include <winbase.h>
#include <Lmcons.h>
#include <cstdlib> // for random


// Forward declaration
extern "C" __declspec(dllexport) BOOL WINAPI InitHCMInternal(const TCHAR * pLogFilePath);
BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved);

bool g_hcmInitialised = false;
std::string g_HCMExternalPath;






// To get function pointers of the DLL in HCM, we need to load the DLL in both HCM and our target process (MCC)
// But InitHCMInternal is *only* called on the copy that's loaded in MCC
extern "C" __declspec(dllexport) BOOL WINAPI InitHCMInternal(const TCHAR * pHCMExternalPath)
{
	if (g_hcmInitialised) return TRUE; // Already initialised

	g_HCMExternalPath = pHCMExternalPath;
	std::string logFilePath = g_HCMExternalPath + "\\HCMInternal.log";
	std::basic_string<TCHAR> converted(logFilePath.begin(), logFilePath.end());
	const TCHAR* pLogFilePath = converted.c_str();


	bool showConsole = false;
	// If user is burnt, output to console
	TCHAR username[256 + 1];
	DWORD size = 256 + 1;
	GetUserName((TCHAR*)username, &size);
	showConsole = (std::string(username) == "mauri"); // Burnts real name is Maurice lmao
	FILE* pCout = nullptr;
	if (showConsole)
	{
		if (AllocConsole())
		{
			freopen_s(&pCout, "conout$", "w", stdout);
		}
		else // check for failure allocating console, in which case log to file instead
		{
			freopen_s(&pCout, pLogFilePath, "a", stdout);
			std::cout << "Error allocating console, logging to file instead." << std::endl;
			showConsole = false;
		}

	}
	else // else log to file (using path string provided by HCMExternal in parameter)
	{
		freopen_s(&pCout, pLogFilePath, "a", stdout);
		if (pCout == NULL) //check for failure, if so we turn the console on anyways
		{
			showConsole = true;
			AllocConsole();
			freopen_s(&pCout, "conout$", "w", stdout);
			std::cout << "Error opening file for HCMInternal logging: " << *perror << "\nOutputting to console instead (note: closing this console will close MCC)." << std::endl << "Invalid path was: " << std::string(pLogFilePath) << std::endl;
		}
	}

	// Print first message and current time - helps to sync with HCMExternal log file
	std::time_t curr_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::cout << "HCMInternal.dll injected @ " << std::to_string(curr_time) << std::endl;

	// Initialize the speedhack - is set to speed of 1.00 (ie unmodified speed) until HCMExternal demands otherwise
	LONG InitSpeedHackReturn = InitSpeedhack();
	if (InitSpeedHackReturn != NO_ERROR)
	{
		std::cout << "Failed initializing speedhack, error code :"  << std::endl;
		return FALSE;
	}
	std::cout << "Sucessfully initialised" << std::endl;



	g_hcmInitialised = true;





	return TRUE;

}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);

		break;

	case DLL_PROCESS_DETACH:
		if (g_hcmInitialised)
		{
			fclose(stdout);
			FreeConsole();
		}
		break;
	}

	return TRUE;
}
