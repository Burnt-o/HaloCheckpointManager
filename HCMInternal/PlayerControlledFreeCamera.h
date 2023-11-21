#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "CameraDataPtr.h"

class PlayerControlledFreeCamera : public IOptionalCheat
{
private:
	class PlayerControlledFreeCameraImpl;
	std::unique_ptr<PlayerControlledFreeCameraImpl> pimpl;

public:
	PlayerControlledFreeCamera(GameState gameImpl, IDIContainer& dicon);
	~PlayerControlledFreeCamera();

	void updateCamera(CameraDataPtr& gameCamera);
	void setupCamera(CameraDataPtr& gameCamera);

	std::string_view getName() override { return nameof(PlayerControlledFreeCameraImpl); }

};