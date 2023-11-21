#include "pch.h"
#include "PlayerControlledFreeCamera.h"





class PlayerControlledFreeCamera::PlayerControlledFreeCameraImpl
{
private:

	// runtime camera data
	struct CameraData
	{
		SimpleMath::Vector3 position;
		SimpleMath::Vector3 velocity;
		SimpleMath::Vector3 forwardLookDir;
		SimpleMath::Vector3 upLookDir;
		float FOV;
	}mCameraData;

	bool cameraDataInitialised = false;

	void processCameraData()
	{
		if (GetKeyState('W') & 0x8000)
		{
			mCameraData.position.x += 0.01;
		}
		if (GetKeyState('A') & 0x8000)
		{
			mCameraData.position.y += 0.01;
		}
		if (GetKeyState('S') & 0x8000)
		{
			mCameraData.position.x -= 0.01;
		}
		if (GetKeyState('D') & 0x8000)
		{
			mCameraData.position.y -= 0.01;
		}

		
	}

public:
	PlayerControlledFreeCameraImpl(GameState gameImpl, IDIContainer& dicon)
	{

	}

	void updateCamera(CameraDataPtr& cameraToUpdate)
	{
		if (!cameraDataInitialised) throw HCMRuntimeException("You must call setupCamera at least once before calling updateCamera");
		processCameraData();

		// write camera data to the game
		LOG_ONCE(PLOG_DEBUG << "writing mCameraData");
		*cameraToUpdate.position = mCameraData.position;
		*cameraToUpdate.velocity = mCameraData.velocity;
		*cameraToUpdate.forwardLookDir = mCameraData.forwardLookDir;
		*cameraToUpdate.upLookDir = mCameraData.upLookDir;
		*cameraToUpdate.FOV = mCameraData.FOV;


		
	}

	void setupCamera(CameraDataPtr& cameraToPullDataFrom)
	{
		PLOG_DEBUG << "PlayerControlledFreeCamera setting up camera";
		mCameraData.position = *cameraToPullDataFrom.position;
		mCameraData.velocity = *cameraToPullDataFrom.velocity;
		mCameraData.forwardLookDir = *cameraToPullDataFrom.forwardLookDir;
		mCameraData.upLookDir = *cameraToPullDataFrom.upLookDir;
		mCameraData.FOV = *cameraToPullDataFrom.FOV;

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

void PlayerControlledFreeCamera::updateCamera(CameraDataPtr& cameraToUpdate) { return pimpl->updateCamera(cameraToUpdate); }

void PlayerControlledFreeCamera::setupCamera(CameraDataPtr& cameraToPullDataFrom) { return pimpl->setupCamera(cameraToPullDataFrom); }