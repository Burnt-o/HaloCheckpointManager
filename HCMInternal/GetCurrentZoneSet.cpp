#include "pch.h"
#include "GetCurrentZoneSet.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"

class GetCurrentZoneSet::GetCurrentZoneSetImpl
{
private:
	std::shared_ptr<MultilevelPointer> currentZoneSet;

public:
	GetCurrentZoneSetImpl(GameState game, IDIContainer& dicon)
	{
		currentZoneSet = dicon.Resolve<PointerDataStore>().lock()->getData<std::shared_ptr<MultilevelPointer>>
			(nameof(currentZoneSet), game);
	}
	uint32_t getCurrentZoneSet()
	{
		uint32_t currentZS;
		if (!currentZoneSet->readData(&currentZS)) throw HCMRuntimeException(std::format("Bad read of currentZoneSet, {}", MultilevelPointer::GetLastError()));
		return currentZS;
	}
};


GetCurrentZoneSet::GetCurrentZoneSet(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique<GetCurrentZoneSetImpl>(game, dicon);
}
GetCurrentZoneSet::~GetCurrentZoneSet() = default;

uint32_t GetCurrentZoneSet::getCurrentZoneSet() { return pimpl->getCurrentZoneSet(); }