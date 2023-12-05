#pragma once
#include "pch.h"
#include "ISmoother.h"




template<typename valueType>
class LinearSmoother : public ISmoother<valueType>
{
private:
	float mSmoothRate;

public:
	void setSmoothRate(float smoothRate) 
	{ 
		mSmoothRate = smoothRate;
	}
	LinearSmoother(float smoothRate) : mSmoothRate(smoothRate)
	{}

	virtual void smooth(valueType& currentValue, valueType desiredValue) override;
};

