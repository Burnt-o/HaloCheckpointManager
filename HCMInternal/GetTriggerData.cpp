#include "pch.h"
#include "GetTriggerData.h"
#include "GetTriggerData.h"
#include "Render3DEventProvider.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "GetTagAddress.h"
#include "IMakeOrGetCheat.h"
#include "TagBlock.h"
#include "DynamicStructFactory.h"
#include "MCCString.h"

template<GameState::Value mGame>
class GetTriggerDataImpl : public GetTriggerDataUntemplated
{
private:
	// callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	// injected services
	std::weak_ptr< GetTagAddress> getTagAddressWeak;

	// data for resolution
	int triggerVolumeTagBlockOffset;

	std::shared_ptr<MultilevelPointer> metaHeaderAddress;
	int64_t tagBase;

	enum class tagBlockDataFields { entryCount, offset };
	std::shared_ptr<DynamicStruct<tagBlockDataFields>> tagBlockDataStruct;

	enum class triggerDataFields { name, forward, up, position, extents };
	std::shared_ptr<StrideableDynamicStruct<triggerDataFields>> triggerDataStruct;

	// resolved data
	std::map<TriggerPointer, TriggerData> triggerData;
	bool triggerDataCached = false;

	void onGameStateChange(const MCCState&)
	{
		PLOG_INFO << "clearing " << nameof(GetTriggerDataImpl) << " caches";
		triggerDataCached = false;
		triggerData.clear(); // TODO: render thread safety issue
	}

	void updateTriggerData()
	{
		PLOG_DEBUG << "updating trigger data!";
		triggerData.clear(); // TODO: render thread safety issue

		lockOrThrow(getTagAddressWeak, getTagAddress);

		tagBlockDataStruct->currentBaseAddress = getTagAddress->getScenarioAddress() + triggerVolumeTagBlockOffset;
		auto triggerCount = *tagBlockDataStruct->field<uint32_t>(tagBlockDataFields::entryCount);

		uintptr_t metaHeaderAddressResolved;
		if (!metaHeaderAddress->resolve(&metaHeaderAddressResolved)) throw HCMRuntimeException(std::format("Could not resolve metaHeaderAddress, {}", MultilevelPointer::GetLastError()));


		auto* pTriggerTagBlockOffset = tagBlockDataStruct->field<uint32_t>(tagBlockDataFields::offset);
		if (IsBadReadPtr((void*)pTriggerTagBlockOffset, 8)) throw HCMRuntimeException(std::format("Bad read address for pTriggerTagBlockOffset at {}", (uintptr_t)pTriggerTagBlockOffset));

		auto triggerTagBlockOffset = *pTriggerTagBlockOffset;

		PLOG_DEBUG << "triggerTagBlockOffset: " << std::hex << triggerTagBlockOffset;

		triggerDataStruct->currentBaseAddress = metaHeaderAddressResolved + tagBase + triggerTagBlockOffset;

		PLOG_DEBUG << "triggerDataStruct first entry location: " << std::hex << (uintptr_t)triggerDataStruct->currentBaseAddress;

		for (int i = 0; i < triggerCount; i++)
		{
			triggerDataStruct->setIndex(i);

			auto* pName = triggerDataStruct->field<const char>(triggerDataFields::name);
			if (IsBadReadPtr(pName, 0x20)) throw HCMRuntimeException(std::format("Bad read address for pName at {}", (uintptr_t)pName));

			auto* pPos = triggerDataStruct->field<SimpleMath::Vector3>(triggerDataFields::position);
			if (IsBadReadPtr(pPos, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad read address for pPos at {}", (uintptr_t)pPos));

			auto* pExtents = triggerDataStruct->field<SimpleMath::Vector3>(triggerDataFields::extents);
			if (IsBadReadPtr(pExtents, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad read address for pExtents at {}", (uintptr_t)pExtents));

			auto* pForward = triggerDataStruct->field<SimpleMath::Vector3>(triggerDataFields::forward);
			if (IsBadReadPtr(pForward, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad read address for pForward at {}", (uintptr_t)pForward));

			auto* pUp = triggerDataStruct->field<SimpleMath::Vector3>(triggerDataFields::up);
			if (IsBadReadPtr(pUp, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad read address for pUp at {}", (uintptr_t)pUp));


			PLOG_DEBUG << "read Trigger Data at index " << i << ", location " << std::hex << triggerDataStruct->currentBaseAddress << ":";
			PLOG_DEBUG << "Name: " << std::string(pName);
			PLOG_DEBUG << "Posi: " << *pPos;
			PLOG_DEBUG << "Exte: " << *pExtents;
			PLOG_DEBUG << "Ford: " << *pForward;
			PLOG_DEBUG << "UpDr: " << *pUp;


			triggerData.emplace(triggerDataStruct->currentBaseAddress, 
				TriggerData(
					std::string(pName),
					*pPos,
					*pExtents,
					*pForward,
					*pUp
				));

		}

	}

public:
	GetTriggerDataImpl(GameState game, IDIContainer& dicon)
		: getTagAddressWeak(resolveDependentCheat(GetTagAddress)),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) { onGameStateChange(n); })
	{
		auto ptr = dicon.Resolve< PointerDataStore>().lock();
		triggerVolumeTagBlockOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(triggerVolumeTagBlockOffset), game).get();
		tagBlockDataStruct = DynamicStructFactory::make<tagBlockDataFields>(ptr, game);
		triggerDataStruct = DynamicStructFactory::makeStrideable<triggerDataFields>(ptr, game);
		metaHeaderAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(metaHeaderAddress), game);
		tagBase = *ptr->getData<std::shared_ptr<int64_t>>(nameof(tagBase), game).get();

	}

	virtual std::map<TriggerPointer, TriggerData>& getTriggerData() override
	{
		if (triggerDataCached == false)
		{
			updateTriggerData();
			triggerDataCached = true;
		}
			

		return triggerData;
	}
};


std::map<TriggerPointer, TriggerData>& GetTriggerData::getTriggerData()
{
	return pimpl->getTriggerData();
}

GetTriggerData::GetTriggerData(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

GetTriggerData::~GetTriggerData() = default;