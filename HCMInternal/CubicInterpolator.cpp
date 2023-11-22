#include "pch.h"
#include "CubicInterpolator.h"


template <>
virtual void CubicInterpolator<float>::interpolate(float& currentValue, float desiredValue)
{
	float step = desiredValue - currentValue;
	step = step * mInterpolationRate;

	step = step * step * (3.0f - 2.0f * step);

	currentValue = std::clamp(currentValue + step, std::min(currentValue, desiredValue), std::max(currentValue, desiredValue));;

	//float out = std::clamp((mInterpolationRate - currentValue) / (desiredValue - currentValue), 0.0f, 1.0f);
	//out = out * out * (3.0f - 2.0f * out);
}

template <>
virtual void CubicInterpolator<SimpleMath::Vector3>::interpolate(SimpleMath::Vector3& currentValue, SimpleMath::Vector3 desiredValue)
{
	LOG_ONCE_CAPTURE(PLOG_DEBUG << "rate: " << r, r = mInterpolationRate);

	currentValue = SimpleMath::Vector3::SmoothStep(currentValue, desiredValue, mInterpolationRate);


	//SimpleMath::Vector3 step = desiredValue - currentValue;
	//step = step * mInterpolationRate;

	//step = step * step * (SimpleMath::Vector3{ 3.0f, 3.0f, 3.0f } - SimpleMath::Vector3{2.0f, 2.0f, 2.0f} *step);

	//currentValue = std::clamp(currentValue + step, std::min(currentValue, desiredValue), std::max(currentValue, desiredValue));;



	//float smoothRate = mInterpolationRate * mInterpolationRate * (3.f - (2.f * mInterpolationRate));

	//currentValue = SimpleMath::Vector3::Lerp(currentValue, desiredValue, smoothRate);

}

