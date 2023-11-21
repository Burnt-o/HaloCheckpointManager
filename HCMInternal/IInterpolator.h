#pragma once

template <typename valueType>
class IInterpolator
{
public:
	virtual ~IInterpolator() = default;
	virtual void interpolate(valueType& currentValue, valueType desiredValue, float scalar) = 0;
};
