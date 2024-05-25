#pragma once
#include "pch.h"
#include "EnumClass.h"
#include "BumpControlDefs.h"

enum class Input
{
	None = 0,
	Forward = 0x1 << 0,
	Backward = 0x1 << 1,
	Left = 0x1 << 2,
	Right = 0x1 << 3,
	Crouch = 0x1 << 4,
	Jump = 0x1 << 5,

};
enableEnumClassBitmask(Input); // Activate bitmask operators
typedef bitmask<Input> InpMask;

InpMask& operator+=(InpMask& lhs, Input rhs);

InpMask operator+(InpMask& lhs, Input rhs);

InpMask& operator-=(InpMask& lhs, Input rhs);

InpMask operator-(InpMask& lhs, Input rhs);

bool contains(const InpMask& lhs, Input rhs);

InpMask flip(const InpMask& lhs, const Input rhs);

std::string InpMaskToString(InpMask inpMask);