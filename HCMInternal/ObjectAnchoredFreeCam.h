#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "FreeCameraData.h"

class ObjectAnchoredFreeCam : public IOptionalCheat
{
private:
	class ObjectAnchoredFreeCamImpl;
	std::unique_ptr<ObjectAnchoredFreeCamImpl> pimpl;

public:
	ObjectAnchoredFreeCam(GameState gameImpl, IDIContainer& dicon);
	~ObjectAnchoredFreeCam();

	void transformCameraPosition(FreeCameraData& freeCameraData);
	void transformCameraRotation(FreeCameraData& freeCameraData);
	void transformCameraFOV(FreeCameraData& freeCameraData);

	std::string_view getName() override { return nameof(ObjectAnchoredFreeCam); }

};