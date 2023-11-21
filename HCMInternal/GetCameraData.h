#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "CameraDataPtr.h"


class GetCameraData : public IOptionalCheat
{
private:
	class GetCameraDataImpl;
	std::unique_ptr<GetCameraDataImpl> pimpl;

public:
	GetCameraData(GameState gameImpl, IDIContainer& dicon);
	~GetCameraData();

	CameraDataPtr getCameraData();

	std::string_view getName() override { return nameof(GetCameraData); }

};