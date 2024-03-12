#pragma once
#include "pch.h"

class BitBoolPointer
{
private:
	uintptr_t mpData;
	bitOffsetT mBitOffset;
public:
	BitBoolPointer(uintptr_t pData, bitOffsetT bitOffset) : mpData(pData), mBitOffset(bitOffset) {}
	explicit operator bool const()
	{
		return ((*(byte*)mpData) >> mBitOffset) & 1;
	}

	void set(bool newValue)
	{
		if (newValue)
			*(byte*)mpData = *(byte*)mpData | (1 << mBitOffset);
		else
			*(byte*)mpData = *(byte*)mpData & ~(1 << mBitOffset);
	}

	void updateAddress(uintptr_t pData)
	{
		mpData = pData;
	}

};