#include "pch.h"
#include "PlayerControlledFreeCamera.h"
#include "LinearInterpolator.h"
#include "CubicInterpolator.h"
#include "PointerManager.h"
#include "MultilevelPointer.h"



class PlayerControlledFreeCamera::PlayerControlledFreeCameraImpl
{
private:
	CubicInterpolator<SimpleMath::Vector3> accelerator{ 0.05f };
	CubicInterpolator<SimpleMath::Vector3> acceleratorRot{ 0.05f };

	std::shared_ptr<MultilevelPointer> playerViewAngle;


	bool cameraDataInitialised = false;

	struct MyCameraData
	{
		// data that will be written back to the game camera
		SimpleMath::Vector3 currentPosition;
		SimpleMath::Vector3 currentVelocity;
		float currentFOV;
		SimpleMath::Vector3 currentlookDirForward;
		SimpleMath::Vector3 currentlookDirRight; // not actually written back but handy
		SimpleMath::Vector3 currentlookDirUp;

		// private data for math
		SimpleMath::Vector3 targetlookDirForward;
		SimpleMath::Vector3 targetlookDirRight; 
		SimpleMath::Vector3 targetlookDirUp;
		SimpleMath::Vector3 targetPosition;
		float targetFOV;

	}mCameraData;

	void processMyCameraPosition(float frameDelta)
	{

		float cameraSpeed = 5.f * frameDelta;


		// Section: Translation
		SimpleMath::Vector3 desiredPositionOffset = SimpleMath::Vector3{ 0.f, 0.f, 0.f }; // reset to zero each frame
		// add from inputs
		if (GetKeyState('W') & 0x8000)
		{
			desiredPositionOffset = desiredPositionOffset + (mCameraData.currentlookDirForward * cameraSpeed);
		}
		if (GetKeyState('A') & 0x8000)
		{
			// calculate right vector
			desiredPositionOffset = desiredPositionOffset + (mCameraData.currentlookDirRight * cameraSpeed);
		}
		if (GetKeyState('S') & 0x8000)
		{
			desiredPositionOffset = desiredPositionOffset - (mCameraData.currentlookDirForward * cameraSpeed);
		}
		if (GetKeyState('D') & 0x8000)
		{
			// calculate right vector
			desiredPositionOffset = desiredPositionOffset - (mCameraData.currentlookDirRight * cameraSpeed);
		}

		// convert desiredPositionOffset to worldspace
		mCameraData.targetPosition = mCameraData.targetPosition + desiredPositionOffset;

		// interpolate camera towards target position
		accelerator.interpolate(mCameraData.currentPosition, mCameraData.targetPosition);

	}

	static inline int opSetup = 0;

	void processMyCameraRotation(float frameDelta)
	{

		// todo multiple by frame delta
		float cameraRotationSpeed = 3.f * frameDelta;
		constexpr float halfPiPositive = DirectX::XM_PIDIV2;
		constexpr float halfPiNegative = DirectX::XM_PIDIV2 * -1.0f;
		constexpr float epsilon = 0.0001f;

		// Section: Rotation
		// yaw
		if (GetKeyState('J') & 0x8000)
		{
			auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(mCameraData.targetlookDirUp, cameraRotationSpeed);
			mCameraData.targetlookDirForward = SimpleMath::Vector3::TransformNormal(mCameraData.targetlookDirForward, rotMat);
			mCameraData.targetlookDirRight = SimpleMath::Vector3::TransformNormal(mCameraData.targetlookDirRight, rotMat);
		}

		if (GetKeyState('K') & 0x8000)
		{
			auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(mCameraData.targetlookDirUp, cameraRotationSpeed * -1.f);
			mCameraData.targetlookDirForward = SimpleMath::Vector3::TransformNormal(mCameraData.targetlookDirForward, rotMat);
			mCameraData.targetlookDirRight = SimpleMath::Vector3::TransformNormal(mCameraData.targetlookDirRight, rotMat);
		}

		// pitch
		if (GetKeyState('U') & 0x8000)
		{
			auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(mCameraData.targetlookDirRight, cameraRotationSpeed);
			mCameraData.targetlookDirForward = SimpleMath::Vector3::TransformNormal(mCameraData.targetlookDirForward, rotMat);
			mCameraData.targetlookDirUp = SimpleMath::Vector3::TransformNormal(mCameraData.targetlookDirUp, rotMat);
		}

		if (GetKeyState('I') & 0x8000)
		{
			auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(mCameraData.targetlookDirRight, cameraRotationSpeed * -1.f);
			mCameraData.targetlookDirForward = SimpleMath::Vector3::TransformNormal(mCameraData.targetlookDirForward, rotMat);
			mCameraData.targetlookDirUp = SimpleMath::Vector3::TransformNormal(mCameraData.targetlookDirUp, rotMat);
		}

		// roll
		if (GetKeyState('B') & 0x8000)
		{
			auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(mCameraData.targetlookDirForward, cameraRotationSpeed);
			mCameraData.targetlookDirForward = SimpleMath::Vector3::TransformNormal(mCameraData.targetlookDirForward, rotMat);
			mCameraData.targetlookDirUp = SimpleMath::Vector3::TransformNormal(mCameraData.targetlookDirUp, rotMat);
		}

		if (GetKeyState('N') & 0x8000)
		{
			auto rotMat = SimpleMath::Matrix::CreateFromAxisAngle(mCameraData.targetlookDirForward, cameraRotationSpeed * -1.f);
			mCameraData.targetlookDirRight = SimpleMath::Vector3::TransformNormal(mCameraData.targetlookDirRight, rotMat);
			mCameraData.targetlookDirUp = SimpleMath::Vector3::TransformNormal(mCameraData.targetlookDirUp, rotMat);
		}

		// interpolate camera towards target rotation
		acceleratorRot.interpolate(mCameraData.currentlookDirForward, mCameraData.targetlookDirForward);
		acceleratorRot.interpolate(mCameraData.currentlookDirRight, mCameraData.targetlookDirRight);
		acceleratorRot.interpolate(mCameraData.currentlookDirUp, mCameraData.targetlookDirUp);

	}

public:
	PlayerControlledFreeCameraImpl(GameState gameImpl, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
		playerViewAngle = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(playerViewAngle), gameImpl);
	}




	void updateCameraPosition(CameraDataPtr& gameCamera, float frameDelta)
	{
		if (!cameraDataInitialised) throw HCMRuntimeException("You must call setupCamera before calling updateCamera!");

		processMyCameraPosition(frameDelta);
		*gameCamera.position = mCameraData.currentPosition;
		*gameCamera.velocity = mCameraData.currentVelocity;
		
		

	}

	void updateCameraRotation(CameraDataPtr& gameCamera, float frameDelta)
	{
		if (!cameraDataInitialised) throw HCMRuntimeException("You must call setupCamera before calling updateCamera!");

		processMyCameraRotation(frameDelta);
		*gameCamera.FOV = mCameraData.currentFOV;
		*gameCamera.lookDirForward = mCameraData.currentlookDirForward;
		*gameCamera.lookDirUp = mCameraData.currentlookDirUp;
	}


	

	void setupCamera(CameraDataPtr& gameCamera)
	{

		mCameraData.currentPosition = *gameCamera.position;
		mCameraData.targetPosition = *gameCamera.position;

		mCameraData.currentVelocity = *gameCamera.velocity;
		mCameraData.currentFOV = *gameCamera.FOV;
		mCameraData.targetFOV = *gameCamera.FOV;

		mCameraData.currentlookDirForward = *gameCamera.lookDirForward;

		mCameraData.currentlookDirUp = *gameCamera.lookDirUp;

		mCameraData.currentlookDirRight = SimpleMath::Vector3::Transform(mCameraData.currentlookDirForward, SimpleMath::Quaternion::CreateFromAxisAngle(mCameraData.currentlookDirUp, DirectX::XM_PIDIV2));

		mCameraData.targetlookDirForward = mCameraData.currentlookDirForward;
		mCameraData.targetlookDirRight = mCameraData.currentlookDirRight;
		mCameraData.targetlookDirUp = mCameraData.currentlookDirUp;

		cameraDataInitialised = true;
	}

	

};




PlayerControlledFreeCamera::PlayerControlledFreeCamera(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique< PlayerControlledFreeCameraImpl>(gameImpl, dicon);
}
PlayerControlledFreeCamera::~PlayerControlledFreeCamera()
{
	PLOG_DEBUG << "~" << getName();
}

void PlayerControlledFreeCamera::updateCameraPosition(CameraDataPtr& gameCamera, float frameDelta) { return pimpl->updateCameraPosition(gameCamera, frameDelta); }

void PlayerControlledFreeCamera::updateCameraRotation(CameraDataPtr& gameCamera, float frameDelta) { return pimpl->updateCameraRotation(gameCamera, frameDelta); }

void PlayerControlledFreeCamera::setupCamera(CameraDataPtr& gameCamera) { return pimpl->setupCamera(gameCamera); }