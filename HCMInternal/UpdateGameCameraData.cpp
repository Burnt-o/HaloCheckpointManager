#include "pch.h"
#include "UpdateGameCameraData.h"

class UpdateGameCameraData::UpdateGameCameraDataImpl 
{
private:


public:
	UpdateGameCameraDataImpl(GameState gameImpl, IDIContainer& dicon)
	{

	}

	void updateGameCameraData(GameCameraData& gameCameraData, FreeCameraData& freeCameraData, float currentFOVOffset)
	{
		// write camera data back to the game
		*gameCameraData.velocity = freeCameraData.currentPosition - *gameCameraData.position;
		*gameCameraData.position = freeCameraData.currentPosition;

		*gameCameraData.lookDirForward = freeCameraData.currentlookDirForward;
		*gameCameraData.lookDirUp = freeCameraData.currentlookDirUp;

		if (std::fabsf(currentFOVOffset) > 0.000001)
			*gameCameraData.FOV = std::clamp(*gameCameraData.FOV + currentFOVOffset, 0.f, 2.f);
		// TODO: check what the actual max is, it's not 2

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