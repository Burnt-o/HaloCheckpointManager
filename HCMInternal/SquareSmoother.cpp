#include "pch.h"
#include "SquareSmoother.h"



constexpr float minDistance = 0.00001f;

template<>
void SquareSmoother<float>::smooth(float& currentValue, float desiredValue)
{
	float distance = desiredValue - currentValue;

	if (std::abs(distance) < minDistance)
	{
		currentValue = desiredValue;
	}
	else
	{
		bool isNeg = std::signbit(distance);
		distance = std::sqrtf(std::abs(distance));
		if (isNeg) { distance *= -1.f; }
		currentValue = currentValue + (distance * mSmoothRate);
	}

}


template<>
void SquareSmoother<SimpleMath::Vector3>::smooth(SimpleMath::Vector3& currentValue, SimpleMath::Vector3 desiredValue)
{

	float distancex = desiredValue.x - currentValue.x;

	if (std::abs(distancex) < minDistance)
	{
		currentValue.x = desiredValue.x;
	}
	else
	{
		bool isNegx = std::signbit(distancex);
		distancex = std::sqrtf(std::abs(distancex));
		if (isNegx) { distancex *= -1.f; }
		currentValue.x = currentValue.x + (distancex * mSmoothRate);
	}


	float distancey = desiredValue.y - currentValue.y;
	
	if (std::abs(distancey) < minDistance)
	{
		currentValue.y = desiredValue.y;
	}
	else
	{
		bool isNegy = std::signbit(distancey);
		distancey = std::sqrtf(std::abs(distancey));
		if (isNegy) { distancey *= -1.f; }
		currentValue.y = currentValue.y + (distancey * mSmoothRate);
	}

	float distancez = desiredValue.z - currentValue.z;
	
	if (std::abs(distancez) < minDistance)
	{
		currentValue.z = desiredValue.z;
	}
	else
	{
		bool isNegz = std::signbit(distancez);
		distancez = std::sqrtf(std::abs(distancez));
		if (isNegz) { distancez *= -1.f; }
		currentValue.z = currentValue.z + (distancez * mSmoothRate);
	}



}
