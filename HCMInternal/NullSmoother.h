#pragma once
#pragma once
#include "pch.h"
#include "ISmoother.h"




template<typename valueType>
class NullSmoother : public ISmoother<valueType>
{
private:

public:
	virtual void setSmoothRate(float smoothRate) override {  }
	NullSmoother(float smoothRate)
	{}

	virtual void smooth(valueType& currentValue, valueType desiredValue) override
	{
		currentValue = desiredValue;
	}
};

