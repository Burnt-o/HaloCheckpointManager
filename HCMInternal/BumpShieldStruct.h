#pragma once
#include "pch.h"
#include "BumpControlDefs.h"
#include "BumpInput.h"


#ifdef SHIELD_BUMPS
struct BumpShieldObject
{
private:
	char pad0[0x13C];
	bool shieldDisabled; 

public:
	void setShieldState(bool value)
	{

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "shield disabled flag: " << std::hex << p, p = &shieldDisabled);

		shieldDisabled = !value;
	}
};
#endif