#include "pch.h"
#include "LinearInterpolator.h"


template <>
virtual void LinearInterpolator<float>::interpolate(float& currentValue, float desiredValue)
{
	float step = desiredValue - currentValue;
	step = step * mInterpolationRate;
	currentValue = std::clamp(currentValue + step, std::min(currentValue, desiredValue), std::max(currentValue, desiredValue));;

}

template <>
virtual void LinearInterpolator<SimpleMath::Vector3>::interpolate(SimpleMath::Vector3& currentValue, SimpleMath::Vector3 desiredValue)
{
	currentValue = SimpleMath::Vector3::Lerp(currentValue, desiredValue, mInterpolationRate);
}
