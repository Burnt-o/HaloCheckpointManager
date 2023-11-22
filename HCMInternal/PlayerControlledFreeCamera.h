#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "FreeCameraData.h"


struct RelativeCameraState
{
	SimpleMath::Vector3 targetlookDirForward;
	SimpleMath::Vector3 targetlookDirRight;
	SimpleMath::Vector3 targetlookDirUp;
	SimpleMath::Vector3 targetPosition;
};



class PlayerControlledFreeCamera : public IOptionalCheat
{
private:
	class PlayerControlledFreeCameraImpl;
	std::unique_ptr<PlayerControlledFreeCameraImpl> pimpl;

public:
	PlayerControlledFreeCamera(GameState gameImpl, IDIContainer& dicon);
	~PlayerControlledFreeCamera();

	void transformCameraPosition(FreeCameraData& freeCameraData, float frameDelta);
	void transformCameraRotation(FreeCameraData& freeCameraData, float frameDelta);
	void transformCameraFOV(float& currentFOV, float frameDelta);

	std::string_view getName() override { return nameof(PlayerControlledFreeCamera); }

};