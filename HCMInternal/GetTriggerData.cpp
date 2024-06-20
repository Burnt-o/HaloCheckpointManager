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
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include <spanstream>


class ITriggerDataCreator
{
public:
	virtual ~ITriggerDataCreator() = default;
	virtual void updateTriggerData(TriggerDataMapLock triggerDataAllLocked, LevelID levelID) = 0;
};


class TriggerDataCreatorH1 : public ITriggerDataCreator, public TriggerDataFactory
{
private:
	int triggerVolumeTagBlockOffset;

	std::shared_ptr<MultilevelPointer> metaHeaderAddress;
	int64_t tagBase;

	enum class tagBlockDataFields { entryCount, offset };
	std::shared_ptr<DynamicStruct<tagBlockDataFields>> tagBlockDataStruct;

	enum class triggerDataFields { name, forward, up, position, extents };
	std::shared_ptr<StrideableDynamicStruct<triggerDataFields>> triggerDataStruct;

	std::weak_ptr< GetTagAddress> getTagAddressWeak;

public:

	TriggerDataCreatorH1(GameState game, IDIContainer& dicon)
		: getTagAddressWeak(resolveDependentCheat(GetTagAddress))

	{
		auto ptr = dicon.Resolve< PointerDataStore>().lock();
		triggerVolumeTagBlockOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(triggerVolumeTagBlockOffset), game).get();
		tagBlockDataStruct = DynamicStructFactory::make<tagBlockDataFields>(ptr, game);
		triggerDataStruct = DynamicStructFactory::makeStrideable<triggerDataFields>(ptr, game);
		metaHeaderAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(metaHeaderAddress), game);
		tagBase = *ptr->getData<std::shared_ptr<int64_t>>(nameof(tagBase), game).get();
	}

	virtual void updateTriggerData(TriggerDataMapLock triggerDataAllLocked, LevelID levelID) override
	{

		PLOG_DEBUG << "updating trigger data!";


		lockOrThrow(getTagAddressWeak, getTagAddress);

		tagBlockDataStruct->currentBaseAddress = getTagAddress->getScenarioAddress() + triggerVolumeTagBlockOffset;
		auto triggerCount = *tagBlockDataStruct->field<uint32_t>(tagBlockDataFields::entryCount);

		uintptr_t metaHeaderAddressResolved;
		if (!metaHeaderAddress->resolve(&metaHeaderAddressResolved)) throw HCMRuntimeException(std::format("Could not resolve metaHeaderAddress, {}", MultilevelPointer::GetLastError()));


		auto* pTriggerTagBlockOffset = tagBlockDataStruct->field<uint32_t>(tagBlockDataFields::offset);
		if (IsBadReadPtr((void*)pTriggerTagBlockOffset, 8)) throw HCMRuntimeException(std::format("Bad read address for pTriggerTagBlockOffset at {}", (uintptr_t)pTriggerTagBlockOffset));

		auto triggerTagBlockOffset = *pTriggerTagBlockOffset;

		PLOG_DEBUG << "triggerTagBlockOffset: " << std::hex << triggerTagBlockOffset;

		uintptr_t tagDataArray = metaHeaderAddressResolved + tagBase + triggerTagBlockOffset;

		PLOG_DEBUG << "triggerDataStruct first entry location: " << std::hex << (uintptr_t)triggerDataStruct->currentBaseAddress;

		triggerDataAllLocked->clear();

		for (uint32_t triggerIndex = 0; triggerIndex < triggerCount; triggerIndex++)
		{
			triggerDataStruct->setIndex(tagDataArray, triggerIndex);

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


			PLOG_DEBUG << "read Trigger Data at index " << triggerIndex << ", location " << std::hex << triggerDataStruct->currentBaseAddress << ":";
			PLOG_DEBUG << "Name: " << std::string(pName);
			PLOG_DEBUG << "Posi: " << *pPos;
			PLOG_DEBUG << "Exte: " << *pExtents;
			PLOG_DEBUG << "Ford: " << *pForward;
			PLOG_DEBUG << "UpDr: " << *pUp;


			triggerDataAllLocked->emplace(triggerDataStruct->currentBaseAddress,
				std::move(makeTriggerData(
					std::string(pName),
					triggerIndex,
					std::string(pName).starts_with("bsp"),
					*pPos,
					*pExtents,
					*pForward,
					*pUp
				)));


		}
	}

};


// names looked up by debugStringTable/debugStringMetaTable (TODO: really ought to write a generic class for dealing with that pattern)
class TriggerDataCreatorH2 : public ITriggerDataCreator, public TriggerDataFactory
{
private:
	int triggerVolumeTagBlockOffset;

	std::shared_ptr<MultilevelPointer> debugStringTableAddress;
	std::shared_ptr<MultilevelPointer> debugStringMetaTableAddress;
	std::shared_ptr<MultilevelPointer> metaHeaderAddress;
	int64_t tagBase;

	enum class tagBlockDataFields { entryCount, offset };
	std::shared_ptr<DynamicStruct<tagBlockDataFields>> tagBlockDataStruct;

	enum class triggerDataFields { debugStringIndex, debugStringLength, forward, up, position, extents };
	std::shared_ptr<StrideableDynamicStruct<triggerDataFields>> triggerDataStruct;

	std::weak_ptr< GetTagAddress> getTagAddressWeak;

public:

	TriggerDataCreatorH2(GameState game, IDIContainer& dicon)
		: getTagAddressWeak(resolveDependentCheat(GetTagAddress))

	{
		auto ptr = dicon.Resolve< PointerDataStore>().lock();
		triggerVolumeTagBlockOffset = *ptr->getData<std::shared_ptr<int64_t>>(nameof(triggerVolumeTagBlockOffset), game).get();
		tagBlockDataStruct = DynamicStructFactory::make<tagBlockDataFields>(ptr, game);
		triggerDataStruct = DynamicStructFactory::makeStrideable<triggerDataFields>(ptr, game);
		metaHeaderAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(metaHeaderAddress), game);
		tagBase = *ptr->getData<std::shared_ptr<int64_t>>(nameof(tagBase), game).get();

		debugStringTableAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(debugStringTableAddress), game);
		debugStringMetaTableAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(debugStringMetaTableAddress), game);
	}

	virtual void updateTriggerData(TriggerDataMapLock triggerDataAllLocked, LevelID levelID) override
	{

		PLOG_DEBUG << "updating trigger data!";


		lockOrThrow(getTagAddressWeak, getTagAddress);

		tagBlockDataStruct->currentBaseAddress = getTagAddress->getScenarioAddress() + triggerVolumeTagBlockOffset;
		auto triggerCount = *tagBlockDataStruct->field<uint32_t>(tagBlockDataFields::entryCount);

		uintptr_t metaHeaderAddressResolved;
		if (!metaHeaderAddress->resolve(&metaHeaderAddressResolved)) throw HCMRuntimeException(std::format("Could not resolve metaHeaderAddress, {}", MultilevelPointer::GetLastError()));


		auto* pTriggerTagBlockOffset = tagBlockDataStruct->field<uint32_t>(tagBlockDataFields::offset);
		if (IsBadReadPtr((void*)pTriggerTagBlockOffset, 8)) throw HCMRuntimeException(std::format("Bad read address for pTriggerTagBlockOffset at {}", (uintptr_t)pTriggerTagBlockOffset));

		auto triggerTagBlockOffset = *pTriggerTagBlockOffset;

		PLOG_DEBUG << "triggerTagBlockOffset: " << std::hex << triggerTagBlockOffset;

		uintptr_t tagDataArray = metaHeaderAddressResolved + tagBase + triggerTagBlockOffset;

		PLOG_DEBUG << "triggerDataStruct first entry location: " << std::hex << (uintptr_t)triggerDataStruct->currentBaseAddress;

		triggerDataAllLocked->clear();

		uintptr_t debugStringTable;
		if (!debugStringTableAddress->resolve(&debugStringTable)) throw HCMRuntimeException(std::format("Could not resolve debugStringTableAddress, {}", MultilevelPointer::GetLastError()));

		uintptr_t debugStringMetaTable;
		if (!debugStringMetaTableAddress->resolve(&debugStringMetaTable)) throw HCMRuntimeException(std::format("Could not resolve debugStringMetaTableAddress, {}", MultilevelPointer::GetLastError()));

		for (uint32_t triggerIndex = 0; triggerIndex < triggerCount; triggerIndex++)
		{
			triggerDataStruct->setIndex(tagDataArray, triggerIndex);

			auto* pDebugStringIndex = triggerDataStruct->field<uint16_t>(triggerDataFields::debugStringIndex);
			if (IsBadReadPtr(pDebugStringIndex, sizeof(uint16_t))) throw HCMRuntimeException(std::format("Bad read address for pDebugStringIndex at {}", (uintptr_t)pDebugStringIndex));

			auto* pDebugStringLength= triggerDataStruct->field<uint8_t>(triggerDataFields::debugStringLength);
			if (IsBadReadPtr(pDebugStringLength, sizeof(uint8_t))) throw HCMRuntimeException(std::format("Bad read address for pDebugStringLength at {}", (uintptr_t)pDebugStringLength));

			auto pNameOffset = debugStringMetaTable + (*pDebugStringIndex * sizeof(uint32_t));
			if (IsBadReadPtr((void*)pNameOffset, sizeof(uint32_t))) throw HCMRuntimeException(std::format("Bad read address for pNameOffset at {}", (uintptr_t)pNameOffset));

			auto pName = debugStringTable + *(uint32_t*)pNameOffset;

#ifdef HCM_DEBUG
			PLOG_DEBUG << "triggerIndex: 0x" << std::hex << triggerIndex;
			PLOG_DEBUG << "pDebugStringIndex: 0x" << std::hex << *pDebugStringIndex;
			PLOG_DEBUG << "pDebugStringLength: 0x" << std::hex << (int)*pDebugStringLength;
			PLOG_DEBUG << "nameOffset: 0x" << std::hex << *(uint32_t*)pNameOffset;
			PLOG_DEBUG << "pName: 0x" << std::hex << pName;
#endif


			if (IsBadReadPtr((void*)pName, *pDebugStringLength)) throw HCMRuntimeException(std::format("Bad read address for pName at {}", pName));

			auto* pPos = triggerDataStruct->field<SimpleMath::Vector3>(triggerDataFields::position);
			if (IsBadReadPtr(pPos, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad read address for pPos at {}", (uintptr_t)pPos));

			auto* pExtents = triggerDataStruct->field<SimpleMath::Vector3>(triggerDataFields::extents);
			if (IsBadReadPtr(pExtents, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad read address for pExtents at {}", (uintptr_t)pExtents));

			auto* pForward = triggerDataStruct->field<SimpleMath::Vector3>(triggerDataFields::forward);
			if (IsBadReadPtr(pForward, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad read address for pForward at {}", (uintptr_t)pForward));

			auto* pUp = triggerDataStruct->field<SimpleMath::Vector3>(triggerDataFields::up);
			if (IsBadReadPtr(pUp, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad read address for pUp at {}", (uintptr_t)pUp));


			PLOG_DEBUG << "read Trigger Data at index " << triggerIndex << ", location " << std::hex << triggerDataStruct->currentBaseAddress << ":";
			PLOG_DEBUG << "Name: " << std::string((char*)pName);
			PLOG_DEBUG << "Posi: " << *pPos;
			PLOG_DEBUG << "Exte: " << *pExtents;
			PLOG_DEBUG << "Ford: " << *pForward;
			PLOG_DEBUG << "UpDr: " << *pUp;


			triggerDataAllLocked->emplace(triggerDataStruct->currentBaseAddress,
				std::move(makeTriggerData(
					std::string((char*)pName),
					triggerIndex,
					std::string((char*)pName).starts_with("trans_"),
					*pPos,
					*pExtents,
					*pForward,
					*pUp
				)));


		}
	}

};





template<GameState::Value mGame>
class GetTriggerDataImpl : public GetTriggerDataUntemplated
{
private:
	// callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	// injected services
	std::unique_ptr<ITriggerDataCreator> mTriggerDataCreator;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::shared_ptr< SettingsStateAndEvents> settings;
	std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;

	// data for resolution


	// resolved data
	libguarded::plain_guarded<TriggerDataMap> triggerDataAll = libguarded::plain_guarded<TriggerDataMap>();
	bool triggerDataCached = false;
	libguarded::plain_guarded<TriggerDataMap> triggerDataFiltered = libguarded::plain_guarded<TriggerDataMap>();


	void onGameStateChange(const MCCState&)
	{
		PLOG_INFO << "clearing " << nameof(GetTriggerDataImpl) << " caches";
		triggerDataCached = false;
	}

	void updateTriggerData()
	{
		lockOrThrow(mccStateHookWeak, mccStateHook);
		mTriggerDataCreator->updateTriggerData(triggerDataAll.lock(), mccStateHook->getCurrentMCCState().currentLevelID);
		triggerDataFiltered.lock()->clear();
	}

public:
	GetTriggerDataImpl(GameState game, IDIContainer& dicon, std::unique_ptr<ITriggerDataCreator> triggerDataCreator)
		: 
		mTriggerDataCreator(std::move(triggerDataCreator)),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) { onGameStateChange(n); }),
		settings(dicon.Resolve<SettingsStateAndEvents>().lock()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>().lock())
	
	
	{

	}

	virtual TriggerDataMapLock getAllTriggers() override
	{
			try
			{
				if (triggerDataCached == false)
				{
					updateTriggerData();
					triggerDataCached = true;
					//settings->triggerOverlayFilterString->UpdateValueWithInput();
				}
			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
				//settings->triggerOverlayToggle->GetValueDisplay() = false;
				//settings->triggerOverlayToggle->UpdateValueWithInput();
			}
			return triggerDataAll.lock();
	}
	virtual TriggerDataMapLock getFilteredTriggers() override
	{
		try
		{
			if (triggerDataCached == false)
			{
				updateTriggerData();
				triggerDataCached = true;
				//settings->triggerOverlayFilterString->UpdateValueWithInput();
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
			//settings->triggerOverlayToggle->GetValueDisplay() = false;
			//settings->triggerOverlayToggle->UpdateValueWithInput();
		}

		return triggerDataFiltered.lock();
	}

};


TriggerDataMapLock GetTriggerData::getAllTriggers() {
	return pimpl->getAllTriggers();
}
TriggerDataMapLock GetTriggerData::getFilteredTriggers() {
	return pimpl->getFilteredTriggers();
}

GetTriggerData::GetTriggerData(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo1>>(gameImpl, dicon, std::make_unique<TriggerDataCreatorH1>(gameImpl, dicon));
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo2>>(gameImpl, dicon, std::make_unique<TriggerDataCreatorH2>(gameImpl, dicon));
		break;

	//case GameState::Value::Halo3:
	//	pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo3>>(gameImpl, dicon);
	//	break;

	//case GameState::Value::Halo3ODST:
	//	pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
	//	break;

	//case GameState::Value::HaloReach:
	//	pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
	//	break;

	//case GameState::Value::Halo4:
	//	pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo4>>(gameImpl, dicon);
	//	break;
	default:
		throw HCMInitException("not impl yet");
	}
}

GetTriggerData::~GetTriggerData() = default;