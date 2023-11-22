#include "pch.h"
#include "CameraInputReader.h"


void CameraInputReader::readPositionInput(RelativeCameraState& relativeCameraState, float frameDelta)
{
	// TODO: need a good way to make this based on the ACTUAL lookdir and not the targetlookdir, but still accounting for it being relative ? hurting my head. not a big deal tho.


	// Section: Translation
	auto cameraTranslationSpeed = frameDelta * mPositionSpeed;

	// add from inputs
	if (GetKeyState('W') & 0x8000)
	{
		relativeCameraState.targetPosition = relativeCameraState.targetPosition + (relativeCameraState.targetlookDirForward * cameraTranslationSpeed);
	}
	if (GetKeyState('A') & 0x8000)
	{
		relativeCameraState.targetPosition = relativeCameraState.targetPosition + (relativeCameraState.targetlookDirRight * cameraTranslationSpeed);
	}
	if (GetKeyState('S') & 0x8000)
	{
		relativeCameraState.targetPosition = relativeCameraState.targetPosition - (relativeCameraState.targetlookDirForward * cameraTranslationSpeed);
	}
	if (GetKeyState('D') & 0x8000)
	{
		relativeCameraState.targetPosition = relativeCameraState.targetPosition - (relativeCameraState.targetlookDirRight * cameraTranslationSpeed);
	}

}

void CameraInputReader::readRotationInput(RelativeCameraState& relativeCameraState, float frameDelta)
{
	// Section: Rotation
	auto cameraRotationSpeed = frameDelta * mRotationSpeed;
	// yaw
	if (GetKeyState(VK_NUMPAD4) & 0x8000)
	{
		auto rotQuat = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.currentlookDirUp, cameraRotationSpeed);
		relativeCameraState.targetLookQuat = rotQuat * relativeCameraState.targetLookQuat;

		//auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(relativeCameraState.targetlookDirUp, cameraRotationSpeed);
		//relativeCameraState.targetlookDirForward = SimpleMath::Vector3::TransformNormal(relativeCameraState.targetlookDirForward, rotMat);
		//relativeCameraState.targetlookDirRight = SimpleMath::Vector3::TransformNormal(relativeCameraState.targetlookDirRight, rotMat);
	}

	if (GetKeyState(VK_NUMPAD6) & 0x8000)
	{
		auto rotQuat = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.currentlookDirUp, cameraRotationSpeed * -1.f);
		relativeCameraState.targetLookQuat = rotQuat * relativeCameraState.targetLookQuat;

		//auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(relativeCameraState.targetlookDirUp, cameraRotationSpeed * -1.f);
		//relativeCameraState.targetlookDirForward = SimpleMath::Vector3::TransformNormal(relativeCameraState.targetlookDirForward, rotMat);
		//relativeCameraState.targetlookDirRight = SimpleMath::Vector3::TransformNormal(relativeCameraState.targetlookDirRight, rotMat);
	}

	// pitch
	if (GetKeyState(VK_NUMPAD8) & 0x8000)
	{
		auto rotQuat = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.currentlookDirRight, cameraRotationSpeed);
		relativeCameraState.targetLookQuat = rotQuat * relativeCameraState.targetLookQuat;

		//auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(relativeCameraState.targetlookDirRight, cameraRotationSpeed);
		//relativeCameraState.targetlookDirForward = SimpleMath::Vector3::TransformNormal(relativeCameraState.targetlookDirForward, rotMat);
		//relativeCameraState.targetlookDirUp = SimpleMath::Vector3::TransformNormal(relativeCameraState.targetlookDirUp, rotMat);
	}

	if (GetKeyState(VK_NUMPAD2) & 0x8000)
	{
		auto rotQuat = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.currentlookDirRight, cameraRotationSpeed * -1.f);
		relativeCameraState.targetLookQuat = rotQuat * relativeCameraState.targetLookQuat;

		//auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(relativeCameraState.targetlookDirRight, cameraRotationSpeed * -1.f);
		//relativeCameraState.targetlookDirForward = SimpleMath::Vector3::TransformNormal(relativeCameraState.targetlookDirForward, rotMat);
		//relativeCameraState.targetlookDirUp = SimpleMath::Vector3::TransformNormal(relativeCameraState.targetlookDirUp, rotMat);
	}

	// roll
	if (GetKeyState(VK_NUMPAD7) & 0x8000)
	{
		auto rotQuat = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.currentlookDirForward, cameraRotationSpeed);
		relativeCameraState.targetLookQuat = rotQuat * relativeCameraState.targetLookQuat;

		//auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(relativeCameraState.targetlookDirForward, cameraRotationSpeed);
		//relativeCameraState.targetlookDirForward = SimpleMath::Vector3::TransformNormal(relativeCameraState.targetlookDirForward, rotMat);
		//relativeCameraState.targetlookDirUp = SimpleMath::Vector3::TransformNormal(relativeCameraState.targetlookDirUp, rotMat);
	}

	if (GetKeyState(VK_NUMPAD9) & 0x8000)
	{
		auto rotQuat = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.currentlookDirForward, cameraRotationSpeed * -1.f);
		relativeCameraState.targetLookQuat = rotQuat * relativeCameraState.targetLookQuat;

		//auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(relativeCameraState.targetlookDirForward, cameraRotationSpeed * -1.f);
		//relativeCameraState.targetlookDirRight = SimpleMath::Vector3::TransformNormal(relativeCameraState.targetlookDirRight, rotMat);
		//relativeCameraState.targetlookDirUp = SimpleMath::Vector3::TransformNormal(relativeCameraState.targetlookDirUp, rotMat);
	}
}

void CameraInputReader::readFOVInput(RelativeCameraState& relativeCameraState, float frameDelta)
{
	float cameraFOVSpeed = mFOVSpeed * frameDelta;

	// Section: FOV
	// increase
	if (GetKeyState(VK_NUMPAD1) & 0x8000)
	{
		relativeCameraState.targetFOVOffset = relativeCameraState.targetFOVOffset + cameraFOVSpeed;
	}

	// decrease
	if (GetKeyState(VK_NUMPAD3) & 0x8000)
	{
		relativeCameraState.targetFOVOffset = relativeCameraState.targetFOVOffset - cameraFOVSpeed;
	}
}