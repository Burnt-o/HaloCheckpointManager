#pragma once
#include "pch.h"

struct FreeCameraData
{
	// data that will be written back to the game camera
	SimpleMath::Vector3 currentPosition = SimpleMath::Vector3::Zero;
	SimpleMath::Vector3 currentVelocity = SimpleMath::Vector3::Zero;
	SimpleMath::Vector3 currentlookDirForward = SimpleMath::Vector3::UnitX;
	SimpleMath::Vector3 currentlookDirRight = SimpleMath::Vector3::UnitY; // not actually written back but handy
	SimpleMath::Vector3 currentlookDirUp = SimpleMath::Vector3::UnitZ;

	float currentFOV;

	//// private data for math
	//float frameDelta;
	//SimpleMath::Vector3 targetlookDirForward;
	//SimpleMath::Vector3 targetlookDirRight;
	//SimpleMath::Vector3 targetlookDirUp;
	//SimpleMath::Vector3 targetPosition;
	//float targetFOV;

};