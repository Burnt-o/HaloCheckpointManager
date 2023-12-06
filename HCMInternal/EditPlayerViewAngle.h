#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"

class EditPlayerViewAngle : public IOptionalCheat
{
private:
	class EditPlayerViewAngleImpl;
	std::unique_ptr<EditPlayerViewAngleImpl> pimpl;

public:
	EditPlayerViewAngle(GameState game, IDIContainer& dicon);
	~EditPlayerViewAngle();

	virtual std::string_view getName() override { return nameof(EditPlayerViewAngle); }
};