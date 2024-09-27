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
	const char* SFnameH1, const char* SFpathH1,
	const char* SFnameH2, const char* SFpathH2,
	const char* SFnameH3, const char* SFpathH3,
	const char* SFnameOD, const char* SFpathOD,
	const char* SFnameHR, const char* SFpathHR,
	const char* SFnameH4, const char* SFpathH4
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
			SFnameH1, SFpathH1,
			SFnameH2, SFpathH2,
			SFnameH3, SFpathH3,
			SFnameOD, SFpathOD,
			SFnameHR, SFpathHR,
			SFnameH4, SFpathH4);
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
