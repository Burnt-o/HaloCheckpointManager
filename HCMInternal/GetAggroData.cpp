#include "pch.h"
#include "GetAggroData.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"

class GetAggroData::GetAggroDataImpl
{
private:

	// data
	std::shared_ptr<MultilevelPointer> aggroLevel;
	std::shared_ptr<MultilevelPointer> aggroDecayTimer;
	std::shared_ptr<MultilevelPointer> playerHasAggro;


public:
	GetAggroDataImpl(GameState gameImpl, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve < PointerManager>().lock();
		aggroLevel = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(aggroLevel), gameImpl);
		aggroDecayTimer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(aggroDecayTimer), gameImpl);
		playerHasAggro = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(playerHasAggro), gameImpl);

	}

	AggroData getAggroData()
	{
		AggroData aggroData;
		if (!aggroLevel->readData(&aggroData.aggroLevel)) 
			throw HCMRuntimeException(std::format("Failed to read aggroLevel pointer: {}", MultilevelPointer::GetLastError()));

		if (!aggroDecayTimer->readData(&aggroData.aggroDecayTimer))
			throw HCMRuntimeException(std::format("Failed to read aggroDecayTimer pointer: {}", MultilevelPointer::GetLastError()));

		if (!playerHasAggro->readData(&aggroData.playerHasAggro))
			throw HCMRuntimeException(std::format("Failed to read playerHasAggro pointer: {}", MultilevelPointer::GetLastError()));
		
		return aggroData;
	}
};




GetAggroData::GetAggroData(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique<GetAggroDataImpl>(gameImpl, dicon);
}

GetAggroData::~GetAggroData()
{
	PLOG_VERBOSE << "~" << getName();
}


AggroData GetAggroData::getAggroData() { return pimpl->getAggroData(); }