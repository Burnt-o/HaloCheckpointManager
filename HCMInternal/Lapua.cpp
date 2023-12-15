#include "pch.h"
#include "Lapua.h"
#include "GlobalKill.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")


class Lapua::LapuaImpl
{
private:
	HMODULE HCMInternalModuleHandle;

	std::thread loopThread;

	void loopFunction()
	{
		static int i = 0;
		while (!GlobalKill::isKillSet())
		{
			if (i == 3000)
			{
				i = 0;
#ifdef HCM_DEBUG
				PLOG_DEBUG << "lapua thread running";
#endif

				BOOL RESULT = ConsummateResource("Beatrice");
				PLOG_DEBUG << RESULT;


				if (!RESULT)
				{
					PLOG_DEBUG << "lapua service failure";
					lapuaGood = false;
				}

			}
			else
			{
				i++;
			}
			Sleep(1);
		}

	}

	BOOL ConsummateResource(std::string resourceName)
	{
		LPSTR lpTypeName = _strdup((ResourceType(VE)).c_str());

		BOOL bRtn;
		LPVOID lpRes;
		HRSRC hResInfo;
		HGLOBAL hRes;
		// Find the resource. 

		hResInfo = FindResourceA(HCMInternalModuleHandle, MAKEINTRESOURCEA(104), lpTypeName);
		if (hResInfo == NULL)
		{
			PLOG_ERROR << "hResInfo null";
			return FALSE;
		}


		// Load the resource. 

		hRes = LoadResource(HCMInternalModuleHandle, hResInfo);
		if (hRes == NULL)
		{
			PLOG_ERROR << "hRes null";
			return FALSE;
		}


		// Lock the resource and consummate it. 


		lpRes = LockResource(hRes);
		if (lpRes != NULL) {
			bRtn = resConsume(nd, S)((LPCTSTR)lpRes, SND_MEMORY | SND_SYNC |
				SND_NODEFAULT);
			UnlockResource(hRes);
		}
		else
		{
			PLOG_ERROR << "lpRes null";
			bRtn = 0;
		}
			

		// Free the resource and return success or failure. 

		FreeResource(hRes);
		return bRtn;
	}

public:
	LapuaImpl(HMODULE dllHandle)
		: HCMInternalModuleHandle(dllHandle)
	{
		loopThread = std::thread([this]() { loopFunction(); });
		loopThread.detach();
	}

	~LapuaImpl()
	{
		if (loopThread.joinable())
			loopThread.join();

	}
};


Lapua::Lapua(HMODULE dllHandle)
{
	pimpl = std::make_unique<LapuaImpl>(dllHandle);
}
Lapua::~Lapua() = default;