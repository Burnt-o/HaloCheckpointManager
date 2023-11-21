#pragma once
#include "pch.h"
#include "IInterpolator.h"




template<typename valueType>
class LinearInterpolator : public IInterpolator<valueType>
{
private:
	float mInterpolationRate;

public:
	void setInterpolationRate(float interpolationRate) { mInterpolationRate = interpolationRate; }
	LinearInterpolator(float interpolationRate) : mInterpolationRate(interpolationRate)
	{}

	virtual void interpolate(valueType& currentValue, valueType desiredValue, float scalar = 1.f) override;
};

template <>
virtual void LinearInterpolator<float>::interpolate(float& currentValue, float desiredValue, float scalar)
{
	move_towards(currentValue, desiredValue, mInterpolationRate * scalar);
}

template <>
virtual void LinearInterpolator<SimpleMath::Vector3>::interpolate(SimpleMath::Vector3& currentValue, SimpleMath::Vector3 desiredValue, float scalar) 
{
	LOG_ONCE_CAPTURE(PLOG_DEBUG << "rate: " << r, r = mInterpolationRate);
	LOG_ONCE_CAPTURE(PLOG_DEBUG << "scalar: " << s, s = scalar);

	currentValue = SimpleMath::Vector3::Lerp(currentValue, desiredValue, (mInterpolationRate * scalar));

}
