#include "pch.h"
#include "UpdateGameCameraData.h"
#include "LinearInterpolator.h"
#include "CubicInterpolator.h"

class UpdateGameCameraData::UpdateGameCameraDataImpl 
{
private:

	//CubicInterpolator<SimpleMath::Vector3> translationInterpolator{ 0.05f };
	//CubicInterpolator<SimpleMath::Vector3> rotationInterpolator{ 0.05f };
	//CubicInterpolator<float> FOVInterpolator{ 0.05f };

public:
	UpdateGameCameraDataImpl(GameState gameImpl, IDIContainer& dicon)
	{

	}

	void updateGameCameraData(GameCameraData& gameCameraData, FreeCameraData& freeCameraData, float currentFOVOffset)
	{
		//// interpolate position towards target position
		//SimpleMath::Vector3 lastFramePosition = freeCameraData.currentPosition;
		//translationInterpolator.interpolate(freeCameraData.currentPosition, freeCameraData.targetPosition);

		//// interpolate rotation towards target rotation
		//rotationInterpolator.interpolate(freeCameraData.currentlookDirForward, freeCameraData.targetlookDirForward);
		//rotationInterpolator.interpolate(freeCameraData.currentlookDirRight, freeCameraData.targetlookDirRight);
		//rotationInterpolator.interpolate(freeCameraData.currentlookDirUp, freeCameraData.targetlookDirUp);

		//// interpolate FOV towards target FOV
		//FOVInterpolator.interpolate(freeCameraData.currentFOV, freeCameraData.targetFOV);

		// write camera data back to the game
		*gameCameraData.velocity = freeCameraData.currentPosition - *gameCameraData.position;
		*gameCameraData.position = freeCameraData.currentPosition;

		*gameCameraData.lookDirForward = freeCameraData.currentlookDirForward;
		*gameCameraData.lookDirUp = freeCameraData.currentlookDirUp;

		if (std::fabsf(currentFOVOffset) > 0.000001)
			*gameCameraData.FOV = *gameCameraData.FOV + currentFOVOffset;


	}
};



UpdateGameCameraData::UpdateGameCameraData(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique< UpdateGameCameraDataImpl>(gameImpl, dicon);
}

UpdateGameCameraData::~UpdateGameCameraData()
{
	PLOG_DEBUG << "~" << getName();
}

void UpdateGameCameraData::updateGameCameraData(GameCameraData& gameCameraData, FreeCameraData& freeCameraData, float currentFOV)
{
	return pimpl->updateGameCameraData(gameCameraData, freeCameraData, currentFOV);
}