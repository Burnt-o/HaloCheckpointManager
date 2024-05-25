#include "pch.h"
#include "BumpInput.h"
#include <numeric>


InpMask& operator+=(InpMask& lhs, Input rhs)
{
	lhs = (lhs | rhs);
	return lhs;
}

InpMask operator+(InpMask& lhs, Input rhs)
{
	return lhs += rhs;
}

InpMask& operator-=(InpMask& lhs, Input rhs)
{
	lhs = (lhs & ~InpMask(rhs));
	return lhs;
}

InpMask operator-(InpMask& lhs, Input rhs)
{
	return lhs -= rhs;
}

bool contains(const InpMask& lhs, Input rhs)
{
	return (lhs & InpMask(rhs)).operator bool();
}

InpMask flip(const InpMask& lhs, const Input rhs)
{
	auto out = lhs ^ InpMask(rhs);
	return out;
}


std::string InpMaskToString(InpMask inpMask)
{
	auto possibleInputs = magic_enum::enum_values<Input>();

	std::vector<Input> containedInputs;
	std::copy_if(std::begin(possibleInputs), std::end(possibleInputs), back_inserter(containedInputs), [inpMask](Input const value) { return contains(inpMask, value); });

	if (containedInputs.empty())
		return "None";

	std::vector<std::string> containedInputsStr;
	for (auto& in : containedInputs)
	{
		containedInputsStr.push_back(std::string(magic_enum::enum_name(in)));
	}

	auto out = std::accumulate(
		std::next(containedInputsStr.begin()),
		containedInputsStr.end(),
		containedInputsStr[0],
		[](std::string a, std::string b)
		{
			return a + ", " + b;
		}
	);

	return out;
}
