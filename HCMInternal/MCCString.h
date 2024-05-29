#pragma once
#include "pch.h"

// Basically a std::string with short-string-optimization. Always 20 bytes.
struct MCCString {
	union
	{
		char shortString[0x10];
		char* longString;
	};

	size_t stringLength;
	size_t bufferLength;


	std::string copy() const
	{
		if (stringLength < 0x10)
		{
			return std::string(&shortString[0], stringLength);
		}
		else
		{
			if (IsBadReadPtr(longString, stringLength))
			{
				throw HCMRuntimeException(std::format("Bad MCCString copy at {:X}", (uintptr_t)longString));
			}
			return std::string(longString, stringLength);
		}
	}

	std::string_view view() const
	{
		if (stringLength < 0x10)
		{
			return std::string_view(&shortString[0], stringLength);
		}
		else
		{
			if (IsBadReadPtr(longString, stringLength))
			{
				throw HCMRuntimeException(std::format("Bad MCCString read at {:X}", (uintptr_t)longString));
			}
			return std::string_view(longString, stringLength);
		}
	}
};
static_assert(sizeof(MCCString) == 0x20);