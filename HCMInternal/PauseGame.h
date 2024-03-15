#pragma once
#include "ScopedServiceRequest.h"
#include "PointerDataStore.h"
class PauseGame
{
public:
	class PauseGameImpl;
	class OverridePauseGameImpl;
private:
	std::shared_ptr< PauseGameImpl> pimpl;
	std::shared_ptr< OverridePauseGameImpl> overridePimpl;
public:

	PauseGame(std::shared_ptr<PointerDataStore> ptr);
	~PauseGame();

	std::map<GameState, HCMInitException>& getServiceFailures();
	std::unique_ptr<ScopedServiceRequest> scopedRequest(std::string callerID); // pauses the game while held (unless overridden)

	std::unique_ptr<ScopedServiceRequest> scopedOverrideRequest(std::string callerID); // overrides any current pauses while held (used by advanceTicks cheat)

};

