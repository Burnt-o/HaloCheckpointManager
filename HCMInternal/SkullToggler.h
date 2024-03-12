#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"



class ISkullTogglerImpl { public: virtual ~ISkullTogglerImpl() = default; };
class SkullToggler : public IOptionalCheat
{
private:

	std::unique_ptr<ISkullTogglerImpl> pimpl;

public:
	SkullToggler(GameState gameImpl, IDIContainer& dicon);
	~SkullToggler();

	std::string_view getName() override { return nameof(SkullToggler); }

};