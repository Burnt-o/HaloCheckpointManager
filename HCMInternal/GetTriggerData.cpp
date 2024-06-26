#include "pch.h"
#include "GetTriggerData.h"
#include "GetTriggerData.h"
#include "Render3DEventProvider.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "GetScenarioAddress.h"
#include "IMakeOrGetCheat.h"
#include "TagBlock.h"
#include "DynamicStructFactory.h"
#include "MCCString.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include <spanstream>
#include "GetDebugString.h"
#include "TriggerNamesHardcoded.h"
#include "TagBlockReader.h"

class ITriggerDataCreator
{
public:
	virtual ~ITriggerDataCreator() = default;
	virtual void updateTriggerData(TriggerDataMapLock triggerDataAllLocked, LevelID levelID) = 0;
};


class ITriggerNameResolver
{
public:
	virtual std::string getTriggerName(int triggerIndex, uintptr_t triggerDataStruct, LevelID level) = 0;
	virtual ~ITriggerNameResolver() = default;
};

class TriggerNameResolverH1 : public ITriggerNameResolver
{
private:
	enum class triggerDataFields { name };
	std::shared_ptr<StrideableDynamicStruct<triggerDataFields>> triggerDataStruct;

public:
	TriggerNameResolverH1(GameState game, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve< PointerDataStore>().lock();
		triggerDataStruct = DynamicStructFactory::makeStrideable<triggerDataFields>(ptr, game);
	}

	virtual std::string getTriggerName(int triggerIndex, uintptr_t ptriggerDataStruct, LevelID level)
	{
		triggerDataStruct->currentBaseAddress = ptriggerDataStruct;
		char* name = *triggerDataStruct->field<char*>(triggerDataFields::name);
		if (IsBadReadPtr(name, sizeof(uint32_t))) throw HCMRuntimeException(std::format("Bad read address for name at {}", (uintptr_t)name));
		return std::string(name);
	}
};

class TriggerNameResolverH2 : public ITriggerNameResolver
{
private:
	enum class triggerDataFields { stringID };
	std::shared_ptr<StrideableDynamicStruct<triggerDataFields>> triggerDataStruct;

	std::weak_ptr<GetDebugString> getDebugStringWeak;

public:
	TriggerNameResolverH2(GameState game, IDIContainer& dicon)
		: getDebugStringWeak(resolveDependentCheat(GetDebugString))
	{
		auto ptr = dicon.Resolve< PointerDataStore>().lock();
		triggerDataStruct = DynamicStructFactory::makeStrideable<triggerDataFields>(ptr, game);
	}

	virtual std::string getTriggerName(int triggerIndex, uintptr_t ptriggerDataStruct, LevelID level)
	{

		auto* pStringID = triggerDataStruct->field<uint32_t>(triggerDataFields::stringID);
		if (IsBadReadPtr(pStringID, sizeof(uint32_t))) throw HCMRuntimeException(std::format("Bad read address for pStringID at {}", (uintptr_t)pStringID));

		lockOrThrow(getDebugStringWeak, getDebugString);

		auto name = getDebugString->getDebugString((uintptr_t)pStringID);

		if (!name) throw name.error();
		return name.value();
	}
};

class TriggerNameResolverHardcoded : public ITriggerNameResolver
{
private:


public:
	TriggerNameResolverHardcoded(GameState game, IDIContainer& dicon)
	{
	}

	virtual std::string getTriggerName(int triggerIndex, uintptr_t ptriggerDataStruct, LevelID level)
	{
		if (triggerNamesHardcoded.contains(level) && triggerNamesHardcoded.at(level).size() > triggerIndex)
			return triggerNamesHardcoded.at(level).at(triggerIndex);
		else
			return std::format("Trigger{}", triggerIndex);
	}
};



class TriggerDataCreatorImpl : public ITriggerDataCreator, public TriggerDataFactory
{
private:

	//std::shared_ptr<MultilevelPointer> metaHeaderAddress;
	//int64_t tagBase;

	enum class scenarioTagDataFields { triggerVolumeTagBlock };
	std::shared_ptr<DynamicStruct<scenarioTagDataFields>> scenarioTagDataStruct;

	//enum class tagBlockDataFields { entryCount, offset };
	//std::shared_ptr<DynamicStruct<tagBlockDataFields>> tagBlockDataStruct;

	enum class triggerDataFields { forward, up, position, extents };
	std::shared_ptr<StrideableDynamicStruct<triggerDataFields>> triggerDataStruct;

	std::weak_ptr< GetScenarioAddress> getScenarioAddressWeak;

	std::shared_ptr<ITriggerNameResolver> triggerNameResolver;
	std::function<bool(std::string)> triggerIsBSPLambda;

	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr< TagBlockReader> tagBlockReaderWeak;

public:

	TriggerDataCreatorImpl(GameState game, IDIContainer& dicon, std::shared_ptr<ITriggerNameResolver> nameResolver, std::function<bool(std::string)> bspCheck)
		: getScenarioAddressWeak(resolveDependentCheat(GetScenarioAddress)),
		triggerNameResolver(nameResolver),
		triggerIsBSPLambda(bspCheck),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		tagBlockReaderWeak(resolveDependentCheat(TagBlockReader))

	{
		auto ptr = dicon.Resolve< PointerDataStore>().lock();
		triggerDataStruct = DynamicStructFactory::makeStrideable<triggerDataFields>(ptr, game);
		scenarioTagDataStruct = DynamicStructFactory::make<scenarioTagDataFields>(ptr, game);

	}

	virtual void updateTriggerData(TriggerDataMapLock triggerDataAllLocked, LevelID levelID) override
	{

		PLOG_DEBUG << "updating trigger data!";


		lockOrThrow(getScenarioAddressWeak, getScenarioAddress);
		auto scenAddress = getScenarioAddress->getScenarioAddress();
		if (!scenAddress) throw scenAddress.error();

		scenarioTagDataStruct->currentBaseAddress = scenAddress.value();

		auto* ptriggerVolumeTagBlock = scenarioTagDataStruct->field<void*>(scenarioTagDataFields::triggerVolumeTagBlock);

		lockOrThrow(tagBlockReaderWeak, tagBlockReader);
		auto tagBlockResolved = tagBlockReader->read((uintptr_t)ptriggerVolumeTagBlock);
		if (!tagBlockResolved) throw tagBlockResolved.error();

		PLOG_DEBUG << "triggerDataStruct first entry location: " << std::hex << tagBlockResolved.value().firstElement;

		lockOrThrow(mccStateHookWeak, mccStateHook);
		auto currentLevel = mccStateHook->getCurrentMCCState().currentLevelID;
		triggerDataAllLocked->clear();

		for (uint32_t triggerIndex = 0; triggerIndex < tagBlockResolved.value().elementCount; triggerIndex++)
		{
			triggerDataStruct->setIndex(tagBlockResolved.value().firstElement, triggerIndex);

			auto name = triggerNameResolver->getTriggerName(triggerIndex, triggerDataStruct->currentBaseAddress, currentLevel);

			auto* pPos = triggerDataStruct->field<SimpleMath::Vector3>(triggerDataFields::position);
			if (IsBadReadPtr(pPos, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad read address for pPos at {}", (uintptr_t)pPos));

			auto* pExtents = triggerDataStruct->field<SimpleMath::Vector3>(triggerDataFields::extents);
			if (IsBadReadPtr(pExtents, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad read address for pExtents at {}", (uintptr_t)pExtents));

			auto* pForward = triggerDataStruct->field<SimpleMath::Vector3>(triggerDataFields::forward);
			if (IsBadReadPtr(pForward, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad read address for pForward at {}", (uintptr_t)pForward));

			auto* pUp = triggerDataStruct->field<SimpleMath::Vector3>(triggerDataFields::up);
			if (IsBadReadPtr(pUp, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad read address for pUp at {}", (uintptr_t)pUp));


			PLOG_DEBUG << "read Trigger Data at index " << triggerIndex << ", location " << std::hex << triggerDataStruct->currentBaseAddress << ":";
			PLOG_DEBUG << "Name: " << name;
			PLOG_DEBUG << "Posi: " << *pPos;
			PLOG_DEBUG << "Exte: " << *pExtents;
			PLOG_DEBUG << "Ford: " << *pForward;
			PLOG_DEBUG << "UpDr: " << *pUp;


			triggerDataAllLocked->emplace(triggerDataStruct->currentBaseAddress,
				std::move(makeTriggerData(
					name,
					triggerIndex,
					triggerIsBSPLambda(name),
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
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo1>>(gameImpl, dicon, std::make_unique<TriggerDataCreatorImpl>(gameImpl, dicon, std::make_shared<TriggerNameResolverH1>(gameImpl, dicon), [](std::string str) { return str.starts_with("bsp"); }));
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo2>>(gameImpl, dicon, std::make_unique<TriggerDataCreatorImpl>(gameImpl, dicon, std::make_shared<TriggerNameResolverH2>(gameImpl, dicon), [](std::string str) { return str.starts_with("trans"); }));
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo3>>(gameImpl, dicon, std::make_unique<TriggerDataCreatorImpl>(gameImpl, dicon, std::make_shared<TriggerNameResolverHardcoded>(gameImpl, dicon), [](std::string str) { return str.contains("zone_set"); }));
		break;

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