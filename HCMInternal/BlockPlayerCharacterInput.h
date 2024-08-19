#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "SharedRequestProvider.h"

// similiar to BlockGameInput but only blocks input that directly controls the player character (chief/arby), similiar to script command player_enable_input
// ie you can still pause and interact with the pause menu while this is turned on



class BlockPlayerCharacterInput : public IOptionalCheat
{
private:
	std::unique_ptr<TokenSharedRequestProvider> pimpl;


public:
	BlockPlayerCharacterInput(GameState gameImpl, IDIContainer& dicon);

	~BlockPlayerCharacterInput();

	std::string_view getName() override { return nameof(BlockPlayerCharacterInput); }

	// blocks player char input while held
	std::shared_ptr<SharedRequestToken> makeScopedRequest() { 
		return pimpl->makeScopedRequest(); 
	} 

};