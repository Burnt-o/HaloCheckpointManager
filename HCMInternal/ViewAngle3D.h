#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

// this is a simpler copy of Waypoint3D.h/cpp

class ViewAngle3DUntemplated { public: virtual ~ViewAngle3DUntemplated() = default; };
class ViewAngle3D : public IOptionalCheat
{
private:

	std::unique_ptr<ViewAngle3DUntemplated> pimpl;

public:
	ViewAngle3D(GameState gameImpl, IDIContainer& dicon);
	~ViewAngle3D();

	std::string_view getName() override { return nameof(ViewAngle3D); }

};