#include "pch.h"
#include "NullInterpolator.h"

template <>
virtual void NullInterpolator<float>::interpolate(float& currentValue, float desiredValue)
{
	currentValue = desiredValue;
}

template <>
virtual void NullInterpolator<SimpleMath::Vector3>::interpolate(SimpleMath::Vector3& currentValue, SimpleMath::Vector3 desiredValue)
{
	currentValue = desiredValue;
}
