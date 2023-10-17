#pragma once
#include "ScopedServiceRequest.h"
#include "PointerManager.h"
class PauseGame
{
public:
	class PauseGameImpl;
private:
	std::shared_ptr< PauseGameImpl> pimpl;
public:

	PauseGame(std::shared_ptr<PointerManager> ptr);
	~PauseGame();

	std::map<GameState, HCMInitException>& getServiceFailures();
	std::unique_ptr<ScopedServiceRequest> scopedRequest(std::string callerID);

};

