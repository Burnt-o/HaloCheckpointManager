#pragma once
#include "CameraTransformer.h"
#include "Setting.h"

class CameraInputReader
{

public:
	float mPositionSpeed, mRotationSpeed, mFOVSpeed;

	CameraInputReader(float positionSpeed, float rotationSpeed, float FOVSpeed)
		: mPositionSpeed(positionSpeed), mRotationSpeed(rotationSpeed), mFOVSpeed(FOVSpeed)
	{
		
	}
	void readPositionInput(RelativeCameraState& relativeCameraState, float frameDelta);

	void readRotationInput(RelativeCameraState& relativeCameraState, float frameDelta);

	void readFOVInput(RelativeCameraState& relativeCameraState, float frameDelta);
};

