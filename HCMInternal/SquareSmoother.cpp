#include "pch.h"
#include "SquareSmoother.h"


//template<>
//void SquareSmoother<float>::smooth(float& currentValue, float desiredValue)
//{
//}

template<>
void SquareSmoother<SimpleMath::Vector3>::smooth(SimpleMath::Vector3& currentValue, SimpleMath::Vector3 desiredValue)
{
	assert(false && "not implemented yet");

	// I'm bad at math. The intent of this function is to make small differences (between desired and currentValue) SMALLER, and big differences BIGGER.
	// soo like a exponential or square function or something
	// but I can't figure out how to actually implement that because I'm dumb and stupid.
	// Do I need to.. normalise the difference? then apply the square to the individual components? then multiple the original difference by that?

	SimpleMath::Vector3 difference = (desiredValue * desiredValue);
	auto differenceNormalised = difference;
	differenceNormalised.Normalize();
	differenceNormalised = differenceNormalised * differenceNormalised;
	difference = difference * differenceNormalised;



	currentValue = currentValue + (difference * mSmoothRate);
}


//template<>
//void SquareSmoother<SimpleMath::Quaternion>::smooth(SimpleMath::Quaternion& currentValue, SimpleMath::Quaternion desiredValue)
//{
//}
