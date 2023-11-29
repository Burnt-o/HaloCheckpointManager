#pragma once
#include "pch.h"
#include "FreeCameraData.h"
#include "ISmoother.h"

struct RelativeCameraState
{

	SimpleMath::Quaternion currentRotationTransformation;
	SimpleMath::Vector3 currentPositionTransformation;
	float currentFOVOffset = 0;



	SimpleMath::Quaternion targetRotationTransformation;
	SimpleMath::Vector3 targetPositionTransformation;
	float targetFOVOffset = 0;


	SimpleMath::Vector3 targetLookDirForward;
	SimpleMath::Vector3 targetLookDirRight;
	SimpleMath::Vector3 targetLookDirUp;

	float eulerYaw;
	float eulerPitch;
	float eulerRoll;


};


class CameraTransformer
{

public:
	std::shared_ptr<ISmoother<SimpleMath::Vector3>> positionSmoother;
	std::shared_ptr<ISmoother<SimpleMath::Quaternion>> rotationSmoother;
	std::shared_ptr<ISmoother<float>> fovSmoother;

	CameraTransformer(std::shared_ptr<ISmoother<SimpleMath::Vector3>> pos, std::shared_ptr<ISmoother<SimpleMath::Quaternion>> rot, std::shared_ptr<ISmoother<float>> fov)
		: positionSmoother(pos), rotationSmoother(rot), fovSmoother(fov) {}

	RelativeCameraState relativeCameraState;

	void transformCameraPosition(FreeCameraData& freeCameraData, float frameDelta)
	{
		positionSmoother->smooth(relativeCameraState.currentPositionTransformation, relativeCameraState.targetPositionTransformation);

		freeCameraData.currentPosition = freeCameraData.currentPosition + relativeCameraState.currentPositionTransformation;
	}

	void transformCameraRotation(FreeCameraData& freeCameraData, float frameDelta)
	{
		// interpolate currentLookQuat to targetLookQuat
		rotationSmoother->smooth(relativeCameraState.currentRotationTransformation, relativeCameraState.targetRotationTransformation);

		//relativeCameraState.currentRotationTransformation = relativeCameraState.targetRotationTransformation;

		// transform currentLookDirs by currentLookQuat
		freeCameraData.currentlookDirForward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, relativeCameraState.currentRotationTransformation);
		freeCameraData.currentlookDirForward.Normalize();
		freeCameraData.currentlookDirUp = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirUp, relativeCameraState.currentRotationTransformation);
		freeCameraData.currentlookDirUp.Normalize();
		freeCameraData.currentlookDirRight = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirRight, relativeCameraState.currentRotationTransformation);
		freeCameraData.currentlookDirRight.Normalize();

		//freeCameraData.currentlookDirRight = freeCameraData.currentlookDirUp.Cross(freeCameraData.currentlookDirForward);






	}

	void transformCameraFOV(float& currentFOVOffset, float frameDelta)
	{
		fovSmoother->smooth(relativeCameraState.currentFOVOffset, relativeCameraState.targetFOVOffset);

		currentFOVOffset = currentFOVOffset + relativeCameraState.currentFOVOffset;
	}



};