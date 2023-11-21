#include "pch.h"
#include "PlayerControlledFreeCamera.h"
#include "LinearInterpolator.h"




class PlayerControlledFreeCamera::PlayerControlledFreeCameraImpl
{
private:
	LinearInterpolator<SimpleMath::Vector3> accelerator{ 0.05f };
	LinearInterpolator<SimpleMath::Vector3> acceleratorRot{ 0.5f };

	// runtime camera data
	//struct CameraData
	//{
	//	SimpleMath::Vector3 position;
	//	SimpleMath::Vector3 velocity;
	//	SimpleMath::Vector3 forwardLookDir;
	//	SimpleMath::Vector3 upLookDir;
	//	SimpleMath::Vector3 targetForwardLookDir;
	//	SimpleMath::Vector3 targetUpLookDir;
	//	float FOV;
	//}mCameraData;

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

		SimpleMath::Vector3 currentYawPitchRoll; // NOT normalised, these are in radians
		SimpleMath::Vector3 targetYawPitchRoll; // NOT normalised, these are in radians
		SimpleMath::Vector3 targetPosition;
		float targetFOV;

	}mCameraData;

	void processMyCamera()
	{

		// todo multiple by frame delta
		constexpr float cameraSpeed = 0.04f;
		constexpr float cameraRotationSpeed = 0.005f;
		constexpr float halfPiPositive = DirectX::XM_PIDIV2;
		constexpr float halfPiNegative = DirectX::XM_PIDIV2 * -1.0f;
		constexpr float epsilon = 0.0001f;

		// Section: Rotation
		if (GetKeyState('J') & 0x8000)
		{
			mCameraData.targetYawPitchRoll.x = mCameraData.targetYawPitchRoll.x + cameraRotationSpeed;
		}

		if (GetKeyState('K') & 0x8000)
		{
			mCameraData.targetYawPitchRoll.x = mCameraData.targetYawPitchRoll.x - cameraRotationSpeed;
		}

		// clamp vertical viewangle to within half-pi of zero to avoid gimbal lok
		mCameraData.targetYawPitchRoll.y = std::clamp(mCameraData.targetYawPitchRoll.y, halfPiNegative + epsilon, halfPiPositive - epsilon);

		// interpolate currentYawPitchRoll towards target
		acceleratorRot.interpolate(mCameraData.currentYawPitchRoll, mCameraData.targetYawPitchRoll);

		// convert viewangle to lookdirections
		auto rotMatrix = SimpleMath::Matrix::CreateFromYawPitchRoll(mCameraData.currentYawPitchRoll);
		mCameraData.currentlookDirForward = rotMatrix.Forward();
		mCameraData.currentlookDirRight = rotMatrix.Up();
		mCameraData.currentlookDirUp = rotMatrix.Right();


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


		// problem: the camera doesn't have any "overshoot". this is a matter of taste, but the original debug camera has overshoot.
		// problem: velocity doesn't follow your lookdir after you stop inputting velocity like the original debug camera has.
		//	really i oughta just RE how the h1 debug camera does it's math
		// problem: the vec3 lerp thing is operating on each axis seperately when it needs to work on all together. 
	}
public:
	PlayerControlledFreeCameraImpl(GameState gameImpl, IDIContainer& dicon)
	{

	}




	void updateCamera(CameraDataPtr& gameCamera)
	{
		if (!cameraDataInitialised) throw HCMRuntimeException("You must call setupCamera before calling updateCamera!");

		processMyCamera();
		*gameCamera.position = mCameraData.currentPosition;
		*gameCamera.velocity = mCameraData.currentVelocity;
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

		// calculate yawPitchRoll (ie euler angles) from lookdirections. yawPitchRoll is easier to use for calculating input.
		// https://stackoverflow.com/questions/1847394/determine-pitch-yaw-roll-from-up-front-right
		// https://stackoverflow.com/questions/35613741/convert-2-3d-points-to-directional-vectors-to-euler-angles
		mCameraData.currentYawPitchRoll.x = std::asinf(mCameraData.currentlookDirForward.z);
		mCameraData.currentYawPitchRoll.y = std::atan2f(mCameraData.currentlookDirForward.y, mCameraData.currentlookDirForward.x);
		mCameraData.currentYawPitchRoll.z = 0.f;//std::atan2f(mCameraData.currentlookDirForward.z, std::sqrtf((mCameraData.currentlookDirForward.x * mCameraData.currentlookDirForward.x) + (mCameraData.currentlookDirForward.y * mCameraData.currentlookDirForward.y)));

		mCameraData.targetYawPitchRoll.x = mCameraData.currentYawPitchRoll.x;
		mCameraData.targetYawPitchRoll.y = mCameraData.currentYawPitchRoll.y;
		mCameraData.targetYawPitchRoll.z = mCameraData.currentYawPitchRoll.z;

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "calculated yawPitchRoll: " << ypr, ypr = mCameraData.currentYawPitchRoll);

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

void PlayerControlledFreeCamera::updateCamera(CameraDataPtr& gameCamera) { return pimpl->updateCamera(gameCamera); }

void PlayerControlledFreeCamera::setupCamera(CameraDataPtr& gameCamera) { return pimpl->setupCamera(gameCamera); }