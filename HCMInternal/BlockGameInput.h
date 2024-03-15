#pragma once
#include "ScopedServiceRequest.h"
#include "PointerDataStore.h"
class BlockGameInput
{
public:
	class BlockGameInputImpl;
private:
	std::shared_ptr< BlockGameInputImpl> pimpl;
public:

	BlockGameInput(std::shared_ptr<PointerDataStore> ptr);
	~BlockGameInput();


	std::unique_ptr<ScopedServiceRequest> scopedRequest(std::string callerID);



















};