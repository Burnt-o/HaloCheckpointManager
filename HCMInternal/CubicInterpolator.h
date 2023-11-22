#pragma once
#include "pch.h"
#include "IInterpolator.h"




template<typename valueType>
class CubicInterpolator : public IInterpolator<valueType>
{
private:
	float mInterpolationRate;

public:
	virtual void setInterpolationRate(float interpolationRate) override { mInterpolationRate = interpolationRate; }
	CubicInterpolator(float interpolationRate) : mInterpolationRate(interpolationRate)
	{}

	virtual void interpolate(valueType& currentValue, valueType desiredValue) override;
};


template <>
virtual void CubicInterpolator<SimpleMath::Vector3>::interpolate(SimpleMath::Vector3& currentValue, SimpleMath::Vector3 desiredValue)
{
	LOG_ONCE_CAPTURE(PLOG_DEBUG << "rate: " << r, r = mInterpolationRate);

	float smoothRate = mInterpolationRate * mInterpolationRate * (3.f - (2.f * mInterpolationRate));

	currentValue = SimpleMath::Vector3::Lerp(currentValue, desiredValue, smoothRate);

}
