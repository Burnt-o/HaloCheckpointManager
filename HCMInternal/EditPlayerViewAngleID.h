#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"

class EditPlayerViewAngleID : public IOptionalCheat
{
private:
	class EditPlayerViewAngleIDImpl;
	std::unique_ptr<EditPlayerViewAngleIDImpl> pimpl;

public:
	EditPlayerViewAngleID(GameState game, IDIContainer& dicon);
	~EditPlayerViewAngleID();

	virtual std::string_view getName() override { return nameof(EditPlayerViewAngleID); }
};