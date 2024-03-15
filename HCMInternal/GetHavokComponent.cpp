#include "pch.h"
#include "GetHavokComponent.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"
#include "IMCCStateHook.h"
#include "DynamicStructFactory.h"


class GetHavokComponentImpl : public GetHavokComponent::IGetHavokComponentImpl
{
private:

	GameState mGame;

	//data
	std::shared_ptr<MultilevelPointer> havokComponentMetaTableHeader;
	uintptr_t havokComponentMetaDataTable_cached = 0;

	enum class havokComponentMetaTableHeaderFields { tableOffset };
	std::shared_ptr<DynamicStruct<havokComponentMetaTableHeaderFields>> havokComponentMetaTableHeaderData;

	int64_t havokComponentMetaStride;

	//callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	// event to fire on mcc state change (just clear the caches)
	void onGameStateChange(const MCCState&)
	{
		PLOG_INFO << "clearing GetHavokComponentImpl caches"; 
		havokComponentMetaDataTable_cached = 0;
	}

public:
	GetHavokComponentImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		havokComponentMetaTableHeader = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(havokComponentMetaTableHeader), game);
		havokComponentMetaStride = *ptr->getData<std::shared_ptr<int64_t>>(nameof(havokComponentMetaStride), game).get();
		havokComponentMetaTableHeaderData = DynamicStructFactory::make<havokComponentMetaTableHeaderFields>(ptr, game);

	}

	virtual uintptr_t getHavokComponent(Datum havokDatum) override
	{

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "havokDatum: " << hDatum, hDatum = havokDatum);

		if (!havokComponentMetaDataTable_cached)
		{
			// resolve cache
			PLOG_INFO << "resolving getHavokComponent caches";
			uintptr_t havokComponentMetaTableHeaderResolved;
			if (!havokComponentMetaTableHeader->resolve(&havokComponentMetaTableHeaderResolved)) throw HCMRuntimeException(std::format("Failed to resolve havokComponentMetaTableHeader: {}", MultilevelPointer::GetLastError()));
			havokComponentMetaTableHeaderData->currentBaseAddress = havokComponentMetaTableHeaderResolved;
			uint32_t tableOffset = *havokComponentMetaTableHeaderData->field<uint32_t>(havokComponentMetaTableHeaderFields::tableOffset);
			havokComponentMetaDataTable_cached = havokComponentMetaTableHeaderResolved + tableOffset;
		}

		auto havokComponentHeaderAddress = havokComponentMetaDataTable_cached + (havokComponentMetaStride * havokDatum.index);


		LOG_ONCE_CAPTURE(PLOG_DEBUG << "havokComponentHeaderAddress: " << addy, addy = havokComponentHeaderAddress);

		if (IsBadReadPtr((void*)havokComponentHeaderAddress, 4)) throw HCMRuntimeException(std::format("getHavokComponent: Bad read at {:X}", havokComponentHeaderAddress));
		return havokComponentHeaderAddress;

	}
};


uintptr_t GetHavokComponent::getHavokComponent(Datum havokDatum)
{
	return pimpl->getHavokComponent(havokDatum);
}


GetHavokComponent::GetHavokComponent(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique< GetHavokComponentImpl>(game, dicon);
}

GetHavokComponent::~GetHavokComponent()
{
	PLOG_DEBUG << "~" << getName();
}