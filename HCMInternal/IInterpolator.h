#pragma once

template <typename valueType>
class IInterpolator
{
public:
	virtual ~IInterpolator() = default;
	virtual void interpolate(valueType& currentValue, valueType desiredValue) = 0;
	virtual void setInterpolationRate(float interpolationRate) = 0;
};
