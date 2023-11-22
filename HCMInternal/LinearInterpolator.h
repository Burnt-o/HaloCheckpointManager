#pragma once
#include "pch.h"
#include "IInterpolator.h"




template<typename valueType>
class LinearInterpolator : public IInterpolator<valueType>
{
private:
	float mInterpolationRate;

public:
	virtual void setInterpolationRate(float interpolationRate) override { mInterpolationRate = interpolationRate; } 
	LinearInterpolator(float interpolationRate) : mInterpolationRate(interpolationRate)
	{}

	virtual void interpolate(valueType& currentValue, valueType desiredValue) override;
};

template <>
virtual void LinearInterpolator<float>::interpolate(float& currentValue, float desiredValue)
{
	move_towards(currentValue, desiredValue, mInterpolationRate);
}

template <>
virtual void LinearInterpolator<SimpleMath::Vector3>::interpolate(SimpleMath::Vector3& currentValue, SimpleMath::Vector3 desiredValue) 
{
	LOG_ONCE_CAPTURE(PLOG_DEBUG << "rate: " << r, r = mInterpolationRate);

	currentValue = SimpleMath::Vector3::Lerp(currentValue, desiredValue, mInterpolationRate);

}
