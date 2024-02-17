#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"



class Waypoint3DUntemplated { public: virtual ~Waypoint3DUntemplated() = default; };
class Waypoint3D : public IOptionalCheat
{
private:

	std::unique_ptr<Waypoint3DUntemplated> pimpl;

public:
	Waypoint3D(GameState gameImpl, IDIContainer& dicon);
	~Waypoint3D();

	std::string_view getName() override { return nameof(Waypoint3D); }

};