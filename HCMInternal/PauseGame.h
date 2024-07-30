#pragma once
#include "ScopedRequestProvider.h"
#include "PointerDataStore.h"
class PauseGame
{
private:
	class PauseGameImpl;
	std::shared_ptr< PauseGameImpl> pimpl;
public:

	PauseGame(std::shared_ptr<PointerDataStore> ptr);
	~PauseGame();

	std::map<GameState, HCMInitException>& getServiceFailures();
	std::shared_ptr<ScopedRequestToken> makeScopedRequest(); // pauses the game while held (unless overridden)
	std::shared_ptr<ScopedRequestToken> makeOverrideScopedRequest(); // overrides any current pauses while held (used by advanceTicks cheat)

};

