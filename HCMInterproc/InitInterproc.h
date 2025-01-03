#pragma once
#include "pch.h"
extern "C"
{
	// Needs parameters for default selected checkpoint settings for shared memory. Then begins the HookStateMachine loop.
	__declspec(dllexport) extern uint16_t initialiseInterproc(
		bool CPnullData,
		int CPgame, const char* CPname, const char* CPpath, const char* CPlevelcode, const char* CPgameVersion, int CPdifficulty,
		int SFgame, const char* SFname, const char* SFpath
	);

	_declspec(dllexport) void resetStateMachine();


}

