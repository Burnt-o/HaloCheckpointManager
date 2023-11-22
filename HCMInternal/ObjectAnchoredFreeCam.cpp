#include "pch.h"
#include "ObjectAnchoredFreeCam.h"
#include "RelativeCameraState.h"


class ObjectAnchoredFreeCam::ObjectAnchoredFreeCamImpl
{
private:

	RelativeCameraState mRelativeCameraState;


public:
	ObjectAnchoredFreeCamImpl(GameState gameImpl, IDIContainer& dicon)
	{

	}




	void transformCameraPosition(FreeCameraData& freeCameraData, float frameDelta)
	{


		float cameraSpeed = 5.f * freeCameraData.frameDelta;


		// Section: Translation
		SimpleMath::Vector3 desiredPositionOffset = SimpleMath::Vector3{ 0.f, 0.f, 0.f }; // reset to zero each frame
		// add from inputs
		if (GetKeyState('W') & 0x8000)
		{
			desiredPositionOffset = desiredPositionOffset + (freeCameraData.currentlookDirForward * cameraSpeed);
		}
		if (GetKeyState('A') & 0x8000)
		{
			desiredPositionOffset = desiredPositionOffset + (freeCameraData.currentlookDirRight * cameraSpeed);
		}
		if (GetKeyState('S') & 0x8000)
		{
			desiredPositionOffset = desiredPositionOffset - (freeCameraData.currentlookDirForward * cameraSpeed);
		}
		if (GetKeyState('D') & 0x8000)
		{
			desiredPositionOffset = desiredPositionOffset - (freeCameraData.currentlookDirRight * cameraSpeed);
		}

		// add desiredOffset to targetPosition
		freeCameraData.targetPosition = freeCameraData.targetPosition + desiredPositionOffset;




	}

	void transformCameraRotation(FreeCameraData& freeCameraData, float frameDelta)
	{
		float cameraRotationSpeed = 3.f * freeCameraData.frameDelta;

		// Section: Rotation
		// yaw
		if (GetKeyState(VK_NUMPAD4) & 0x8000)
		{
			auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(freeCameraData.targetlookDirUp, cameraRotationSpeed);
			freeCameraData.targetlookDirForward = SimpleMath::Vector3::TransformNormal(freeCameraData.targetlookDirForward, rotMat);
			freeCameraData.targetlookDirRight = SimpleMath::Vector3::TransformNormal(freeCameraData.targetlookDirRight, rotMat);
		}

		if (GetKeyState(VK_NUMPAD6) & 0x8000)
		{
			auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(freeCameraData.targetlookDirUp, cameraRotationSpeed * -1.f);
			freeCameraData.targetlookDirForward = SimpleMath::Vector3::TransformNormal(freeCameraData.targetlookDirForward, rotMat);
			freeCameraData.targetlookDirRight = SimpleMath::Vector3::TransformNormal(freeCameraData.targetlookDirRight, rotMat);
		}

		// pitch
		if (GetKeyState(VK_NUMPAD8) & 0x8000)
		{
			auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(freeCameraData.targetlookDirRight, cameraRotationSpeed);
			freeCameraData.targetlookDirForward = SimpleMath::Vector3::TransformNormal(freeCameraData.targetlookDirForward, rotMat);
			freeCameraData.targetlookDirUp = SimpleMath::Vector3::TransformNormal(freeCameraData.targetlookDirUp, rotMat);
		}

		if (GetKeyState(VK_NUMPAD2) & 0x8000)
		{
			auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(freeCameraData.targetlookDirRight, cameraRotationSpeed * -1.f);
			freeCameraData.targetlookDirForward = SimpleMath::Vector3::TransformNormal(freeCameraData.targetlookDirForward, rotMat);
			freeCameraData.targetlookDirUp = SimpleMath::Vector3::TransformNormal(freeCameraData.targetlookDirUp, rotMat);
		}

		// roll
		if (GetKeyState(VK_NUMPAD7) & 0x8000)
		{
			auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(freeCameraData.targetlookDirForward, cameraRotationSpeed);
			freeCameraData.targetlookDirForward = SimpleMath::Vector3::TransformNormal(freeCameraData.targetlookDirForward, rotMat);
			freeCameraData.targetlookDirUp = SimpleMath::Vector3::TransformNormal(freeCameraData.targetlookDirUp, rotMat);
		}

		if (GetKeyState(VK_NUMPAD9) & 0x8000)
		{
			auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(freeCameraData.targetlookDirForward, cameraRotationSpeed * -1.f);
			freeCameraData.targetlookDirRight = SimpleMath::Vector3::TransformNormal(freeCameraData.targetlookDirRight, rotMat);
			freeCameraData.targetlookDirUp = SimpleMath::Vector3::TransformNormal(freeCameraData.targetlookDirUp, rotMat);
		}
	}


	void transformCameraFOV(FreeCameraData& freeCameraData, float frameDelta)
	{
		float cameraFOVSpeed = 3.f * freeCameraData.frameDelta;

		// Section: FOV
		// increase
		if (GetKeyState(VK_NUMPAD1) & 0x8000)
		{
			freeCameraData.targetFOV = freeCameraData.targetFOV + cameraFOVSpeed;
		}

		// decrease
		if (GetKeyState(VK_NUMPAD3) & 0x8000)
		{
			freeCameraData.targetFOV = freeCameraData.targetFOV - cameraFOVSpeed;
		}


	}




};




ObjectAnchoredFreeCam::ObjectAnchoredFreeCam(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique< ObjectAnchoredFreeCamImpl>(gameImpl, dicon);
}
ObjectAnchoredFreeCam::~ObjectAnchoredFreeCam()
{
	PLOG_DEBUG << "~" << getName();
}

void ObjectAnchoredFreeCam::transformCameraPosition(FreeCameraData& freeCameraData, float frameDelta) { return pimpl->transformCameraPosition(freeCameraData, frameDelta); }

void ObjectAnchoredFreeCam::transformCameraRotation(FreeCameraData& freeCameraData, float frameDelta) { return pimpl->transformCameraRotation(freeCameraData, frameDelta); }

void ObjectAnchoredFreeCam::transformCameraFOV(FreeCameraData& freeCameraData, float frameDelta) { return pimpl->transformCameraFOV(freeCameraData, frameDelta); }