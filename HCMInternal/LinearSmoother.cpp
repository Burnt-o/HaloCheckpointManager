#include "pch.h"
#include "LinearSmoother.h"



template<>
void LinearSmoother<float>::smooth(float& currentValue, float desiredValue)
{
	float distance = desiredValue - currentValue;
	currentValue = currentValue + (distance * mSmoothRate);
}


template<>
void LinearSmoother<SimpleMath::Vector3>::smooth(SimpleMath::Vector3& currentValue, SimpleMath::Vector3 desiredValue)
{
	SimpleMath::Vector3 distance = desiredValue - currentValue;
	currentValue = currentValue + (distance * mSmoothRate);
}




template<>
void LinearSmoother<SimpleMath::Quaternion>::smooth(SimpleMath::Quaternion& currentValue, SimpleMath::Quaternion desiredValue)
{
	
	
	auto distance = SimpleMath::Quaternion::Angle(desiredValue, currentValue);

	if (GetKeyState('7') & 0x8000)
	{
		PLOG_DEBUG << "linearSmoother distance: " << distance;
	}


	distance = distance / DirectX::XM_PI; // normalise distance

	if (GetKeyState('6') & 0x8000)
	{
		PLOG_DEBUG << "linearSmoother normalised distance: " << distance;
	}


	currentValue = SimpleMath::Quaternion::Slerp(currentValue, desiredValue, distance * mSmoothRate);
}

