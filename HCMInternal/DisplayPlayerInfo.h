#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"


class DisplayPlayerInfoImplUntemplated { public: virtual ~DisplayPlayerInfoImplUntemplated() = default; };
class DisplayPlayerInfo : public IOptionalCheat
{
private:
	std::unique_ptr<DisplayPlayerInfoImplUntemplated> pimpl;

public:
	DisplayPlayerInfo(GameState gameImpl, IDIContainer& dicon);
	~DisplayPlayerInfo();

	std::string_view getName() override { return nameof(DisplayPlayerInfo); }

};