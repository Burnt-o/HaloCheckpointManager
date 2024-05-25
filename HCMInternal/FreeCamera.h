#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

class FreeCameraImplUntemplated { public: virtual ~FreeCameraImplUntemplated() = default; };
class FreeCamera : public IOptionalCheat
{
private:

std::unique_ptr<FreeCameraImplUntemplated> pimpl;

public:
	FreeCamera(GameState gameImpl, IDIContainer& dicon);
~FreeCamera();

std::string_view getName() override { return nameof(FreeCamera); }


};