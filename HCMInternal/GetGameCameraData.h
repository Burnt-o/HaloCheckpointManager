#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "GameCameraData.h"


class GetGameCameraData : public IOptionalCheat
{
private:
	class GetGameCameraDataImpl;
	std::unique_ptr<GetGameCameraDataImpl> pimpl;

public:
	GetGameCameraData(GameState gameImpl, IDIContainer& dicon);
	~GetGameCameraData();

	GameCameraData getGameCameraData();

	std::string_view getName() override { return nameof(GetGameCameraData); }

};