#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"


/*
When enabled (or revert while enabled), locks on to nearest deployable shield.
When that shield is in disabled state, prints movement input presses (relative to shield-re-enabling tick).
*/

class ShieldInputPrinter : public IOptionalCheat
{
private:
	class ShieldInputPrinterImpl;
	std::unique_ptr<ShieldInputPrinterImpl> pimpl;

public:
	ShieldInputPrinter(GameState game, IDIContainer& dicon);
	~ShieldInputPrinter();

	virtual std::string_view getName() override { return nameof(ShieldInputPrinter); }
};