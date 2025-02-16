#pragma once
#include "pch.h"
#include "type_safe\strong_typedef.hpp"
namespace ts = type_safe;


struct SubpixelID
	: ts::strong_typedef<SubpixelID, uint32_t>,                 // required
	ts::strong_typedef_op::equality_comparison<SubpixelID>,   // for operator==/operator!=
	ts::strong_typedef_op::relational_comparison<SubpixelID>, // for operator< etc.
	ts::strong_typedef_op::integer_arithmetic<SubpixelID> // all arithmetic operators that make sense for integers
{
	using strong_typedef::strong_typedef;

	// construct from float
	constexpr static SubpixelID fromFloat(float angle) 
	{
		// The smallest subpixel the game will represent your view angle is 0. This is the 0th subpixel.
		// the next smallest is 1.175494351E-38, which when reinterpreted as an int is 8388608. This is the 1st subpixel.
		// int interpretations are just straight increments from here to 6.28f, therefore:
		auto angleInt = std::bit_cast<uint32_t>(angle);
		if (angleInt != 0)
		{
			return SubpixelID(angleInt - 8388607);
		}
		else
		{
			return SubpixelID(0);
		}

	}

	// convert to float
	constexpr operator float() const
	{
		if (!this)
		{
			throw HCMInitException("Null ptr to SubpixelID!");
		}

		auto angleInt = *this; // copy
		if (angleInt == 0)
			return 0.f;
		else
		{
			// reverse what we did above
			angleInt += (SubpixelID)8388607;
			return std::bit_cast<float>(angleInt);
		}
	}

	operator std::string() const
	{
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}

	constexpr SubpixelID() = default;
};

// output to stream
// Will add commas as thousands seperator.
std::ostream& operator<<(std::ostream& out, const SubpixelID& d);