#include "pch.h"
#include "Lapua.h"
#include "GlobalKill.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")
#include "ManagedResource.h"


class Lapua::LapuaImpl
{
private:

	std::thread loopThread;
	ManagedResource lapuaRes = ManagedResource(104, "WAVE");

	void loopFunction()
	{
		static int i = 0;
		while (!GlobalKill::isKillSet())
		{
			if (shouldRun())
			{
				Sleep(10);
				continue;
			}

			if (i == 3000)
			{
				i = 0;
#ifdef HCM_DEBUG
				PLOG_DEBUG << "lapua";
#endif
				LOG_ONCE(PLOG_DEBUG << "lapua start");
				sndPlaySoundW((LPCWSTR)lapuaRes.hData, SND_MEMORY | SND_SYNC | SND_NODEFAULT);
				LOG_ONCE(PLOG_DEBUG << "lapua end");
			}
			else
			{
				i++;
			}
			Sleep(1);
		}

	}



public:
	LapuaImpl()
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


Lapua::Lapua()
{
	pimpl = std::make_unique<LapuaImpl>();
}
Lapua::~Lapua() = default;