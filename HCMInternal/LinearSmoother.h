#pragma once
#include "pch.h"
#include "ISmoother.h"




template<typename valueType>
class LinearSmoother : public ISmoother<valueType>
{
private:
	float mSmoothRate;

public:
	virtual void setSmoothRate(float inertia) override 
	{ 
		mSmoothRate = inertia;
	}
	LinearSmoother(float smoothRate) : mSmoothRate(smoothRate)
	{}

	virtual void smooth(valueType& currentValue, valueType desiredValue) override;
};

