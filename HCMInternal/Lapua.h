#pragma once
#include "pch.h"

// A class intended to add a detection vector for naughty people who want to use HCM to splice together speedruns and pass them off as single-segment runs.
// The exact nature of this detection vector is intentionally slightly obfuscated. Anyone codey enough for that could easily achieve bad goals a myriad other ways so no biggie.


class Lapua
{
private:
	class LapuaImpl;
	std::unique_ptr<LapuaImpl> pimpl;

public:
	Lapua();
	~Lapua();

	static inline bool lapuaGood = true;
	static inline bool lapuaGood2 = true;
	static inline bool shouldRunForBypass = false;
	static inline bool shouldRunForWatermark = false;

	static bool shouldRun() { return shouldRunForBypass || shouldRunForWatermark; }
};