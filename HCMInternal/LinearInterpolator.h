#pragma once
#include "pch.h"
#include "IInterpolator.h"

//// std::clamp but where you don't know which edge is higher or lower
//template<typename valueType>
//valueType agnosticClamp(valueType x, valueType edge0, valueType edge1)
//{
//	valueType& smallestEdge = std::min(edge0, edge1);
//	if (x < smallestEdge) return smallestEdge;
//
//	valueType& largestEdge = std::max(edge0, edge1);
//	if (x > largestEdge) return largestEdge;
//
//	return x;
//}


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

