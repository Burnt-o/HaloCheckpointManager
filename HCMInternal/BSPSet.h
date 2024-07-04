#pragma once
#include "boost\preprocessor\repetition\repeat.hpp"
#include "EnumClass.h"
#include <bitset>

// also known as zone sets, but I find BSPSet to be a bit clearer


enum class BSPIndex : uint32_t
{
	None = 0,

#define GenerateZoneSetIndex(z, n, d) index##n = 0x1 << n,
	BOOST_PP_REPEAT(32, GenerateZoneSetIndex, ~)
#undef GenerateZoneSetIndex

	// indexN = 0x1 << N,  ...
	// SBSPs are limited to max index 31

};

enableEnumClassBitmask(BSPIndex); // Activate bitmask operators
using BSPIndexMask = bitmask<BSPIndex>;

// wrapper for BSPIndexMask with some helper functions
class BSPSet
{
private:
	BSPIndexMask value;
public:
	BSPSet() : value(BSPIndex::None) {}
	constexpr BSPSet(BSPIndexMask v) : value(v) {}
	constexpr explicit BSPSet(uint32_t v) : value(BSPIndex::None) { value.value = v; }

	constexpr operator BSPIndexMask() const { return value; }

	explicit operator bool() const = delete;

	constexpr bool indexIsLoaded(uint32_t BSPindex) { return (value.value & (1 << BSPindex)) == 1; }

	std::string toBinaryString(uint8_t maxIndex) const
	{
		std::bitset<32> bitset(value.value);
		std::string out = bitset.to_string();

		// erase leading zeroes
		out.erase(out.begin(), out.end() - maxIndex);
		return out;
	}

	std::string toHexadecimalString() const
	{
		return std::format("{:X}", value.value);
	}

	uint8_t highestLoadedIndex() const
	{
		uint64_t valueCopy = value.value;
		uint8_t r = 0;
		while (valueCopy >>= 1)
			r++;
		return r;
	}

	void setIndex(uint8_t index, bool newValue)
	{
		if (newValue)
			value.value |= (1 << index);
		else
			value.value &= ~(1 << index);
	}
};


static_assert(sizeof(BSPSet) == sizeof(BSPIndexMask));