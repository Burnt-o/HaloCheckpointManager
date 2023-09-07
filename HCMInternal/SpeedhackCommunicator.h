#pragma once
#include "pch.h"
namespace SpeedhackCommunicator
{
	namespace  //anonynmous
	{
		typedef void(*SetSpeed)(double);
	}

	static inline HMODULE speedhackModule = nullptr;

	static inline void SetSpeedhackSpeed(double input)
	{
		if (!speedhackModule) throw HCMRuntimeException("speedhackModule not found");

		auto pSetSpeed = (SetSpeed)GetProcAddress(speedhackModule, "SetSpeed");

		if (!pSetSpeed) throw HCMRuntimeException("could not find SetSpeed function");

		pSetSpeed(input);
	}

}

