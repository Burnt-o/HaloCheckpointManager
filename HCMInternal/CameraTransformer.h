#pragma once
#include "pch.h"
#include "FreeCameraData.h"
#include "IInterpolator.h"

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
	std::shared_ptr<IInterpolator<SimpleMath::Vector3>> positionInterpolator;
	std::shared_ptr<IInterpolator<SimpleMath::Vector3>> rotationInterpolator;
	std::shared_ptr<IInterpolator<float>> fovInterpolator;

	CameraTransformer(std::shared_ptr<IInterpolator<SimpleMath::Vector3>> pos, std::shared_ptr<IInterpolator<SimpleMath::Vector3>> rot, std::shared_ptr<IInterpolator<float>> fov)
		: positionInterpolator(pos), rotationInterpolator(rot), fovInterpolator(fov) {}

	RelativeCameraState relativeCameraState;

	void transformCameraPosition(FreeCameraData& freeCameraData, float frameDelta)
	{
		positionInterpolator->interpolate(relativeCameraState.currentPosition, relativeCameraState.targetPosition);

		freeCameraData.currentPosition = freeCameraData.currentPosition + relativeCameraState.currentPosition;
	}

	void transformCameraRotation(FreeCameraData& freeCameraData, float frameDelta)
	{

		// how to interpolate a quaternion? eh skip it for now
		freeCameraData.currentlookDirForward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, relativeCameraState.targetLookQuat);
		freeCameraData.currentlookDirRight = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirRight, relativeCameraState.targetLookQuat);
		freeCameraData.currentlookDirUp = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirUp, relativeCameraState.targetLookQuat);


		//rotationInterpolator->interpolate(relativeCameraState.currentlookDirForward, relativeCameraState.targetlookDirForward);
		//rotationInterpolator->interpolate(relativeCameraState.currentlookDirRight, relativeCameraState.targetlookDirRight);
		//rotationInterpolator->interpolate(relativeCameraState.currentlookDirUp, relativeCameraState.targetlookDirUp);

		//// This works but we have some gimbal lock issues. How do we clamp the pitch to hide it? do it in input?
		//auto rotMat = SimpleMath::Matrix::CreateLookAt(SimpleMath::Vector3::Zero, relativeCameraState.currentlookDirForward, relativeCameraState.currentlookDirUp);
		//freeCameraData.currentlookDirForward = SimpleMath::Vector3::TransformNormal(freeCameraData.currentlookDirForward, rotMat);
		//freeCameraData.currentlookDirRight = SimpleMath::Vector3::TransformNormal(freeCameraData.currentlookDirRight, rotMat);
		//freeCameraData.currentlookDirUp = SimpleMath::Vector3::TransformNormal(freeCameraData.currentlookDirUp, rotMat);
	}

	void transformCameraFOV(float& currentFOVOffset, float frameDelta)
	{
		fovInterpolator->interpolate(relativeCameraState.currentFOVOffset, relativeCameraState.targetFOVOffset);

		currentFOVOffset = currentFOVOffset + relativeCameraState.currentFOVOffset;
	}



};