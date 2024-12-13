#pragma once
#include "pch.h"

static inline uint32_t ViewAngleToSubpixelID(float viewAngle)
{
	// The smallest subpixel the game will represent your view angle is 0. This is the 0th subpixel.
	// the next smallest is 1.175494351E-38, which when reinterpreted as an int is 8388608. This is the 1st subpixel.
	// int interpretations are just straight increments from here to 6.28f, therefore:
	auto subpixelID = *reinterpret_cast<const uint32_t*>(&viewAngle);
	if (subpixelID == 0)
		return 0;
	else
		return subpixelID - 8388607;
}

static inline float SubpixelIDToViewAngle(uint32_t subpixelID)
{
	if (subpixelID == 0)
		return 0.f;
	else
	{
		subpixelID += 8388607; // reverse what we did above
		return *reinterpret_cast<float*>(&subpixelID);
	}

}