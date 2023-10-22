#pragma once
#include "ScopedServiceRequest.h"
#include "PointerManager.h"
class BlockGameInput
{
public:
	class BlockGameInputImpl;
private:
	std::shared_ptr< BlockGameInputImpl> pimpl;
public:

	BlockGameInput(std::shared_ptr<PointerManager> ptr);
	~BlockGameInput();


	std::unique_ptr<ScopedServiceRequest> scopedRequest(std::string callerID);

	std::map<GameState, HCMInitException>& getServiceFailures();
};

