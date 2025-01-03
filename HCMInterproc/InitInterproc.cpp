#include "pch.h"
#include "InitInterproc.h"
#include "HookStateMachine.h"
#include "SharedMemoryExports.h"

bool alreadyInitialised = false;
bool resetStateMachineFlag = false;

void stateMachineLoop()
{
	HookStateMachine stateMachine;
	while (true)
	{
		if (resetStateMachineFlag)
		{
			resetStateMachineFlag = false;
			stateMachine.reset();
		}
		stateMachine.update();
		Sleep(1000);
	}
}

uint16_t initialiseInterproc(
	bool CPnullData,
	int CPgame, const char* CPname, const char* CPpath, const char* CPlevelcode, const char* CPgameVersion, int CPdifficulty,
	int SFgame, const char* SFname, const char* SFpath
)
{
	PLOG_DEBUG << "initialising interproc";


	if (alreadyInitialised)
	{
		PLOG_ERROR << "reinitialisation? weird";
		return g_SharedMemoryExternal.operator bool();
	}

	else
		alreadyInitialised = true;


	PLOG_INFO << "Attempting to init shared memory";
	try
	{
		g_SharedMemoryExternal = std::make_unique<SharedMemoryExternal>(CPnullData,
			CPgame, CPname, CPpath, CPlevelcode, CPgameVersion, CPdifficulty,
			SFgame, SFname, SFpath);
		PLOG_INFO << "Success!";
	}
	catch (std::exception ex)
	{
		PLOG_ERROR << "Failure! " << ex.what();
		return false;
	}

	// Begin state machine loop on new thread
	CreateThread(0, 0x1000, (LPTHREAD_START_ROUTINE)stateMachineLoop, NULL, 0, NULL);
	return true;
	

}

void resetStateMachine()
{
	resetStateMachineFlag = true;
}
