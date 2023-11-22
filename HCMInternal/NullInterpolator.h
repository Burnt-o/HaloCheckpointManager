#pragma once
#pragma once
#include "pch.h"
#include "IInterpolator.h"




template<typename valueType>
class NullInterpolator : public IInterpolator<valueType>
{
private:
	float mInterpolationRate;

public:
	virtual void setInterpolationRate(float interpolationRate) override { mInterpolationRate = interpolationRate; }
	NullInterpolator(float interpolationRate) : mInterpolationRate(interpolationRate)
	{}

	virtual void interpolate(valueType& currentValue, valueType desiredValue) override;
};

