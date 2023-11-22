#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "GameCameraData.h"
#include "FreeCameraData.h"

class UpdateGameCameraData : public IOptionalCheat
{
private:
	class UpdateGameCameraDataImpl;
	std::unique_ptr<UpdateGameCameraDataImpl> pimpl;

public:
	UpdateGameCameraData(GameState gameImpl, IDIContainer& dicon);
	~UpdateGameCameraData();

	void updateGameCameraData(GameCameraData& gameCameraData, FreeCameraData& freeCameraData, float currentFOV);

	std::string_view getName() override { return nameof(UpdateGameCameraData); }

};