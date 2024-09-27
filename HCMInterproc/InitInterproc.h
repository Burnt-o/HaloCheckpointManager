#pragma once
#include "pch.h"
extern "C"
{
	// Needs parameters for default selected checkpoint settings for shared memory. Then begins the HookStateMachine loop.
	__declspec(dllexport) extern uint16_t initialiseInterproc(
		bool CPnullData,
		int CPgame, const char* CPname, const char* CPpath, const char* CPlevelcode, const char* CPgameVersion, int CPdifficulty,
		const char* SFnameH1, const char* SFpathH1,
		const char* SFnameH2, const char* SFpathH2,
		const char* SFnameH3, const char* SFpathH3,
		const char* SFnameOD, const char* SFpathOD,
		const char* SFnameHR, const char* SFpathHR,
		const char* SFnameH4, const char* SFpathH4
	);

	_declspec(dllexport) void resetStateMachine();


}

