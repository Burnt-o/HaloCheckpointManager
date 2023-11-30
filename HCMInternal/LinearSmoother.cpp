#include "pch.h"
#include "LinearSmoother.h"



constexpr float minDistance = 0.0001f;

template<>
void LinearSmoother<float>::smooth(float& currentValue, float desiredValue)
{
	float distance = desiredValue - currentValue;
	if (std::abs(distance) < minDistance)
	{
		currentValue = desiredValue;
	}
	else
	{
		currentValue = currentValue + (distance * mSmoothRate);
	}

}


template<>
void LinearSmoother<SimpleMath::Vector3>::smooth(SimpleMath::Vector3& currentValue, SimpleMath::Vector3 desiredValue)
{

		float distancex = desiredValue.x - currentValue.x;
		if (std::abs(distancex) < minDistance)
		{
			currentValue.x = desiredValue.x;
		}
		else
		{
			currentValue.x = currentValue.x + (distancex * mSmoothRate);
		}


		float distancey = desiredValue.y - currentValue.y;
		if (std::abs(distancey) < minDistance)
		{
			currentValue.y = desiredValue.y;
		}
		else
		{
			currentValue.y = currentValue.y + (distancey * mSmoothRate);
		}

		float distancez = desiredValue.z - currentValue.z;
		if (std::abs(distancez) < minDistance)
		{
			currentValue.z = desiredValue.z;
		}
		else
		{
			currentValue.z = currentValue.z + (distancez * mSmoothRate);
		}



}
