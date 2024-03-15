#pragma once
#include "pch.h"
#include <bitset>



class MidhookFlagInterpreter
{
public:
	enum RFlag : uint8_t
	{
		// values corrospend to which bit in the flags register
		carry= 0,
		parity = 2,
		zero = 6,
		sign = 7
	};
	RFlag mRFlag;
	bool valueToSet = false;

	explicit MidhookFlagInterpreter(RFlag in, bool boolin) : mRFlag(in), valueToSet(boolin){}


	// read value of flag
	bool readFlag(SafetyHookContext& ctx)
	{
		return ctx.rflags & (1 << mRFlag);
	}

	// set value of flag
	void setFlagManual(SafetyHookContext& ctx, bool value)
	{
		if (value)
		{
			ctx.rflags |= (1 << mRFlag); // set flag to true
		}
		else
		{
			ctx.rflags &= ~(1 << mRFlag); // set flag to false
		}
	}

	// set value of flag
	void setFlag(SafetyHookContext& ctx)
	{
		//if (!isValueToSetSet) throw HCMRuntimeException("MidhookFlagInterpreter:: valueToSet is not set!");

		if (valueToSet)
		{
			ctx.rflags |= (1 << mRFlag); // set flag to true
		}
		else
		{
			ctx.rflags &= ~(1 << mRFlag); // set flag to false
		}
	}

	friend class PointerDataStore;
	friend class PointerDataStoreImpl;
};