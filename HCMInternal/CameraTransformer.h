#pragma once
#include "pch.h"
#include "FreeCameraData.h"
#include "ISmoother.h"

struct RelativeCameraState
{
	SimpleMath::Quaternion currentLookQuat;
	SimpleMath::Vector3 currentlookDirForward	{1, 0, 0};
	SimpleMath::Vector3 currentlookDirRight		{0, 1, 0};
	SimpleMath::Vector3 currentlookDirUp		{0, 0, 1};
	SimpleMath::Vector3 currentPosition			{0, 0, 0};
	float currentFOVOffset						= 0;

	SimpleMath::Quaternion targetLookQuat;
	SimpleMath::Vector3 targetlookDirForward	{1, 0, 0};
	SimpleMath::Vector3 targetlookDirRight		{0, 1, 0};
	SimpleMath::Vector3 targetlookDirUp			{0, 0, 1};
	SimpleMath::Vector3 targetPosition			{0, 0, 0};
	float targetFOVOffset						= 0;
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
		positionSmoother->smooth(relativeCameraState.currentPosition, relativeCameraState.targetPosition);

		freeCameraData.currentPosition = freeCameraData.currentPosition + relativeCameraState.currentPosition;
	}

	void transformCameraRotation(FreeCameraData& freeCameraData, float frameDelta)
	{
		// interpolate currentLookQuat to targetLookQuat
		rotationSmoother->smooth(relativeCameraState.currentLookQuat, relativeCameraState.targetLookQuat);

		// transform currentLookDirs by currentLookQuat
		freeCameraData.currentlookDirForward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, relativeCameraState.currentLookQuat);
		freeCameraData.currentlookDirRight = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirRight, relativeCameraState.currentLookQuat);
		freeCameraData.currentlookDirUp = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirUp, relativeCameraState.currentLookQuat);


	}

	void transformCameraFOV(float& currentFOVOffset, float frameDelta)
	{
		fovSmoother->smooth(relativeCameraState.currentFOVOffset, relativeCameraState.targetFOVOffset);

		currentFOVOffset = currentFOVOffset + relativeCameraState.currentFOVOffset;
	}



};