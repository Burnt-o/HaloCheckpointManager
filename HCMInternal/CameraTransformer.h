#pragma once
#include "pch.h"
#include "FreeCameraData.h"
#include "ISmoother.h"

struct RelativeCameraState
{


	SimpleMath::Vector3 currentPositionTransformation;
	float currentFOVOffset = 0;

	float currentEulerYaw;
	float currentEulerPitch;
	float currentEulerRoll;

	SimpleMath::Vector3 targetPositionTransformation;
	float targetFOVOffset = 0;


	float targetEulerYaw;
	float targetEulerPitch;
	float targetEulerRoll;


};


class CameraTransformer
{

public:
	std::shared_ptr<ISmoother<SimpleMath::Vector3>> positionSmoother;
	std::shared_ptr<ISmoother<float>> rotationSmoother;
	std::shared_ptr<ISmoother<float>> fovSmoother;

	CameraTransformer(std::shared_ptr<ISmoother<SimpleMath::Vector3>> pos, std::shared_ptr<ISmoother<float>> rot, std::shared_ptr<ISmoother<float>> fov)
		: positionSmoother(pos), rotationSmoother(rot), fovSmoother(fov) {}

	RelativeCameraState relativeCameraState;

	void transformCameraPosition(FreeCameraData& freeCameraData, float frameDelta)
	{
		positionSmoother->smooth(relativeCameraState.currentPositionTransformation, relativeCameraState.targetPositionTransformation);

		freeCameraData.currentPosition = freeCameraData.currentPosition + relativeCameraState.currentPositionTransformation;
	}

	void transformCameraRotation(FreeCameraData& freeCameraData, float frameDelta)
	{
		// interpolate euler angles
		rotationSmoother->smooth(relativeCameraState.currentEulerYaw, relativeCameraState.targetEulerYaw);
		rotationSmoother->smooth(relativeCameraState.currentEulerPitch, relativeCameraState.targetEulerPitch);
		rotationSmoother->smooth(relativeCameraState.currentEulerRoll, relativeCameraState.targetEulerRoll);



		// step one: yaw
		auto quatYaw = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirUp, relativeCameraState.currentEulerYaw);

		auto totalQuat = quatYaw;

		// transform currentLookDirs by rotations
		freeCameraData.currentlookDirForward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, totalQuat);
		freeCameraData.currentlookDirForward.Normalize();
		freeCameraData.currentlookDirUp = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirUp, totalQuat);
		freeCameraData.currentlookDirUp.Normalize();
		freeCameraData.currentlookDirRight = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirRight, totalQuat);
		freeCameraData.currentlookDirRight.Normalize();

		// step two: pitch
		auto quatPitch = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirRight, relativeCameraState.currentEulerPitch);





		totalQuat = quatPitch;

		// transform currentLookDirs by rotations
		freeCameraData.currentlookDirForward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, totalQuat);
		freeCameraData.currentlookDirForward.Normalize();
		freeCameraData.currentlookDirUp = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirUp, totalQuat);
		freeCameraData.currentlookDirUp.Normalize();
		freeCameraData.currentlookDirRight = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirRight, totalQuat);
		freeCameraData.currentlookDirRight.Normalize();


		// step 3: roll
		auto quatRoll = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirForward, relativeCameraState.currentEulerRoll);

		totalQuat = quatRoll;

		// transform currentLookDirs by rotations
		freeCameraData.currentlookDirForward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, totalQuat);
		freeCameraData.currentlookDirForward.Normalize();
		freeCameraData.currentlookDirUp = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirUp, totalQuat);
		freeCameraData.currentlookDirUp.Normalize();
		freeCameraData.currentlookDirRight = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirRight, totalQuat);
		freeCameraData.currentlookDirRight.Normalize();

	}

	void transformCameraFOV(float& currentFOVOffset, float frameDelta)
	{
		fovSmoother->smooth(relativeCameraState.currentFOVOffset, relativeCameraState.targetFOVOffset);

		currentFOVOffset = currentFOVOffset + relativeCameraState.currentFOVOffset;
	}



};