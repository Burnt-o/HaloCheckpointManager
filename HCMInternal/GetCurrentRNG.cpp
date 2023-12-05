#include "pch.h"
#include "GetCurrentRNG.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
class GetCurrentRNG::GetCurrentRNGImpl
{
private:
	//data
	std::shared_ptr<MultilevelPointer> currentRNG;

public:
	GetCurrentRNGImpl(GameState game, IDIContainer& dicon)
	{
		currentRNG = dicon.Resolve<PointerManager>().lock()->getData<std::shared_ptr<MultilevelPointer>>(nameof(currentRNG), game);
	}

	DWORD getCurrentRNG()
	{
		DWORD readRNG;
		if (!currentRNG->readData(&readRNG)) throw HCMRuntimeException("Could not read currentRNG");
		return readRNG;
	}

	~GetCurrentRNGImpl()
	{
		PLOG_DEBUG << "~GetCurrentRNGImpl";
	}
};

GetCurrentRNG::GetCurrentRNG(GameState game, IDIContainer& dicon) : pimpl(std::make_unique<GetCurrentRNG::GetCurrentRNGImpl>(game, dicon)) {}
GetCurrentRNG::~GetCurrentRNG() { PLOG_DEBUG << "~" << getName(); }
DWORD GetCurrentRNG::getCurrentRNG() { return pimpl->getCurrentRNG(); }