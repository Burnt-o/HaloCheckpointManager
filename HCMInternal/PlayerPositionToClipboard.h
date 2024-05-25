#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"



// reads the players position and copies it to the users clipboard
class PlayerPositionToClipboard : public IOptionalCheat
{
private:
	class PlayerPositionToClipboardImpl;
	std::unique_ptr< PlayerPositionToClipboardImpl> pimpl;

public:
	PlayerPositionToClipboard(GameState gameImpl, IDIContainer& dicon);
	~PlayerPositionToClipboard();


	virtual std::string_view getName() override { return nameof(PlayerPositionToClipboard); }
};

