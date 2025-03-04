#include "pch.h"
#include "GetCurrentBSP.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"

class GetCurrentBSP::GetCurrentBSPImpl
{
private:
	//data
	std::shared_ptr<MultilevelPointer> currentBSP;

public:
	GetCurrentBSPImpl(GameState game, IDIContainer& dicon)
	{
		currentBSP = dicon.Resolve<PointerDataStore>().lock()->getData<std::shared_ptr<MultilevelPointer>>(nameof(currentBSP), game);
	}

	DWORD getCurrentBSP()
	{
		DWORD readBSP;
		if (!currentBSP->readData(&readBSP)) throw HCMRuntimeException("Could not read currentBSP");
		return readBSP; 
	}

};

GetCurrentBSP::GetCurrentBSP(GameState gameImpl, IDIContainer& dicon)
{
	if (gameImpl.operator GameState::Value() == GameState::Value::Halo1 || gameImpl.operator GameState::Value() == GameState::Value::Halo2)
		pimpl = std::make_unique<GetCurrentBSPImpl>(gameImpl, dicon);
	else
		throw HCMInitException("GetCurrentBSP not applicable to third gen");
}
GetCurrentBSP::~GetCurrentBSP()
{
	PLOG_VERBOSE << "~" << getName();
}
DWORD GetCurrentBSP::getCurrentBSP() { return pimpl->getCurrentBSP(); }