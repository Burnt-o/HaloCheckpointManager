#pragma once
#include "pch.h"

// A class intended to add a detection vector for naughty people who want to use HCM to splice together speedruns and pass them off as single-segment runs.
// The exact nature of this detection vector is intentionally slightly obfuscated. Anyone codey enough for that could easily achieve bad goals a myriad other ways so no biggie.


#define ResourceType(x) std::string("WA") + #x
#define resConsume(arg1, arg2) s##arg1##Play##arg2##ou##arg1

class Lapua
{
private:
	class LapuaImpl;
	std::unique_ptr<LapuaImpl> pimpl;

public:
	Lapua(HMODULE dllHandle);
	~Lapua();

	static inline bool lapuaGood = true;
	static inline bool lapuaGood2 = true;
	static inline bool shouldRunForBypass = false;
	static inline bool shouldRunForWatermark = false;

	static bool shouldRun() { return shouldRunForBypass || shouldRunForWatermark; }
};