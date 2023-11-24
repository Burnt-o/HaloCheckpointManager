#pragma once
#include "pch.h"
#include "ISmoother.h"




template<typename valueType>
class SquareSmoother : public ISmoother<valueType>
{
private:
	float mSmoothRate;

public:
	virtual void setSmoothRate(float smoothRate) override { mSmoothRate = smoothRate; }
	SquareSmoother(float smoothRate) : mSmoothRate(smoothRate)
	{}

	virtual void smooth(valueType& currentValue, valueType desiredValue) override;
};

