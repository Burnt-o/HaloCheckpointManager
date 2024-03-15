#include "pch.h"
#include "GetHavokAnchorPoint.h"
#include "PointerDataStore.h"
#include "DynamicStructFactory.h"
#include "ModuleHook.h"
#include "GetHavokComponent.h"
#include "IMakeOrGetCheat.h"



class GetHavokAnchorPointImpl : public GetHavokAnchorPoint::IGetHavokAnchorPointImpl
{
private:
	GameState mGame;

	std::shared_ptr<MultilevelPointerSpecialisation::BaseOffset> havokAnchorPointResolver;

	//injected services
	std::weak_ptr<GetHavokComponent> getHavokComponentWeak;

public:
	GetHavokAnchorPointImpl(GameState game, IDIContainer& dicon)
		: mGame(game)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		getHavokComponentWeak = resolveDependentCheat(GetHavokComponent);
		havokAnchorPointResolver = ptr->getData<std::shared_ptr<MultilevelPointerSpecialisation::BaseOffset>>(nameof(havokAnchorPointResolver), game);
	}

	virtual uintptr_t getHavokAnchorPoint(Datum havokDatum) override
	{
		lockOrThrow(getHavokComponentWeak, getHavokComponent);
		auto havokComponentAddress = getHavokComponent->getHavokComponent(havokDatum);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "havokComponentAddress: " << paddy, paddy = havokComponentAddress);
		return getHavokAnchorPoint(havokComponentAddress);
	}

	virtual uintptr_t getHavokAnchorPoint(uintptr_t havokComponentAddress) override
	{
		havokAnchorPointResolver->updateBaseAddress(havokComponentAddress);
		uintptr_t outAddress;
		if (!havokAnchorPointResolver->resolve(&outAddress)) throw HCMRuntimeException(std::format("Could not resolve havokAnchorPointResolver with base address {:X}, error: {}", havokComponentAddress, MultilevelPointer::GetLastError()));
		
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "havokAnchorPointAddress: " << addy, addy = outAddress);
		return outAddress;
	}
};


GetHavokAnchorPoint::GetHavokAnchorPoint(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique<GetHavokAnchorPointImpl>(game, dicon);
}

GetHavokAnchorPoint::~GetHavokAnchorPoint()
{
	PLOG_DEBUG << "~" << getName();
}

uintptr_t GetHavokAnchorPoint::getHavokAnchorPoint(Datum havokDatum)
{
	return pimpl->getHavokAnchorPoint(havokDatum);
}

uintptr_t GetHavokAnchorPoint::getHavokAnchorPoint(uintptr_t havokComponentAddress)
{
	return pimpl->getHavokAnchorPoint(havokComponentAddress);
}
