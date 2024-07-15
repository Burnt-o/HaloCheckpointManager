#include "pch.h"
#include "GetSoftCeilingData.h"
#include "IMakeOrGetCheat.h"
#include "IMCCStateHook.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "GetScenarioAddress.h"
#include "DynamicStructFactory.h"
#include "PointerDataStore.h"
#include "TagBlockReader.h"
#include "GetActiveStructureDesignTags.h"
#include "BSPSetChangeHookEvent.h"
#include "ZoneSetChangeHookEvent.h"






class ISoftCeilingDataFactory
{
public:
	virtual ~ISoftCeilingDataFactory() = default;
	virtual std::expected<SoftCeilingVector, HCMRuntimeException> getSoftCeilings() = 0;
};



template<GameState::Value mGame>
class SoftCeilingDataFactoryImpl : public ISoftCeilingDataFactory
{
private:
	std::weak_ptr<GetActiveStructureDesignTags> getActiveStructureDesignTagsWeak;
	std::weak_ptr<TagBlockReader> tagBlockReaderWeak;
	std::weak_ptr< GetScenarioAddress> getScenarioAddressWeak;

	enum class scenarioTagDataFields { softCeilingMetaTagBlock };
	std::shared_ptr<DynamicStruct<scenarioTagDataFields>> scenarioTagDataStruct;

	// looked up from scen tag at above offset
	enum class softCeilingMetaDataFields { stringID, flagsMask, type };
	std::shared_ptr<StrideableDynamicStruct<softCeilingMetaDataFields>> softCeilingMetaDataStruct;

	// sddt tag
	enum class sddtTagDataFields { softCeilingTagBlock };
	std::shared_ptr<DynamicStruct<sddtTagDataFields>> sddtTagDataStruct;

	// looked up from sddt tag at above offset
	enum class softCeilingDataFields { stringID, softCeilingTriangleTagBlock};
	std::shared_ptr<StrideableDynamicStruct<softCeilingDataFields>> softCeilingDataStruct;

	enum class softCeilingTriangleDataFields { vertex0, vertex1, vertex2};
	std::shared_ptr<StrideableDynamicStruct<softCeilingTriangleDataFields>> softCeilingTriangleDataStruct;



public:
	SoftCeilingDataFactoryImpl(GameState game, IDIContainer& dicon)
		: getActiveStructureDesignTagsWeak(resolveDependentCheat(GetActiveStructureDesignTags)),
		tagBlockReaderWeak(resolveDependentCheat(TagBlockReader)),
		getScenarioAddressWeak(resolveDependentCheat(GetScenarioAddress))
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		scenarioTagDataStruct = DynamicStructFactory::make< scenarioTagDataFields>(ptr, game);
		softCeilingMetaDataStruct = DynamicStructFactory::makeStrideable< softCeilingMetaDataFields>(ptr, game);
		sddtTagDataStruct = DynamicStructFactory::make< sddtTagDataFields>(ptr, game);
		softCeilingDataStruct = DynamicStructFactory::makeStrideable< softCeilingDataFields>(ptr, game);
		softCeilingTriangleDataStruct = DynamicStructFactory::makeStrideable< softCeilingTriangleDataFields>(ptr, game);
	}

	virtual std::expected<SoftCeilingVector, HCMRuntimeException> getSoftCeilings() override
	{
		try
		{
			// get meta info from scenario tag
			LOG_ONCE(PLOG_DEBUG << "the fun begins");

			lockOrThrow(getScenarioAddressWeak, getScenarioAddress);
			auto scenTagAddress = getScenarioAddress->getScenarioAddress();
			if (!scenTagAddress)
				return std::unexpected(scenTagAddress.error());

			scenarioTagDataStruct->currentBaseAddress = scenTagAddress.value();

			auto* softCeilingMetaDataTagBlock = scenarioTagDataStruct->field<uint32_t>(scenarioTagDataFields::softCeilingMetaTagBlock);

			lockOrThrow(tagBlockReaderWeak, tagBlockReader);
			auto softCeilingMetaDataTagBlockResolved = tagBlockReader->read((uintptr_t)softCeilingMetaDataTagBlock);
			if (!softCeilingMetaDataTagBlockResolved) 
				return std::unexpected(softCeilingMetaDataTagBlockResolved.error());

			struct softCeilingMetaInfo
			{
				const SoftCeilingObjectMask softCeilingObjectMask;
				const SoftCeilingType softCeilingType;
				explicit softCeilingMetaInfo(SoftCeilingObjectMask softCeilingObjectMask, SoftCeilingType softCeilingType)
					: softCeilingObjectMask(softCeilingObjectMask), softCeilingType(softCeilingType) {}
			};
			std::map<uint32_t, softCeilingMetaInfo> softCeilingMetaInfoMap; // key is stringID

			LOG_ONCE_CAPTURE(PLOG_DEBUG << "iterating soft ceiling meta information, count: " << c, c = softCeilingMetaDataTagBlockResolved.value().elementCount);
			for (int metaIndex = 0; metaIndex < softCeilingMetaDataTagBlockResolved.value().elementCount; metaIndex++)
			{
				softCeilingMetaDataStruct->setIndex(softCeilingMetaDataTagBlockResolved.value().firstElement, metaIndex);
				softCeilingMetaInfoMap.emplace(std::pair< uint32_t, softCeilingMetaInfo>(
					*softCeilingMetaDataStruct->field<uint32_t>(softCeilingMetaDataFields::stringID),
					softCeilingMetaInfo(
						*softCeilingMetaDataStruct->field<SoftCeilingObjectMask>(softCeilingMetaDataFields::flagsMask),
						*softCeilingMetaDataStruct->field<SoftCeilingType>(softCeilingMetaDataFields::type)
					)));
			}
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "successfully iterated soft ceiling meta, map count: " << c, c = softCeilingMetaInfoMap.size());




			lockOrThrow(getActiveStructureDesignTagsWeak, getActiveStructureDesignTags);
			auto sddtTags = getActiveStructureDesignTags->getActiveStructureDesignTags();




			// address isn't correct for halo reach - it's tagBase(0x50000000) less than it should be.


			SoftCeilingVector outVec;

			// loop through all sddt tags, go to the softCeiling tagBlock
			for (auto& sddtTag : sddtTags)
			{
				PLOG_DEBUG << "reading sddt tag at address: 0x" << std::hex << sddtTag.tagAddress;
				sddtTagDataStruct->currentBaseAddress = sddtTag.tagAddress;
				auto* pSoftCeilingTagBlock = sddtTagDataStruct->field<uint32_t>(sddtTagDataFields::softCeilingTagBlock);
				if (IsBadReadPtr(pSoftCeilingTagBlock, sizeof(uint32_t)))
					return std::unexpected(HCMRuntimeException(std::format("Bad read of pSoftCeilingTagBlock at {:X}", (uintptr_t)pSoftCeilingTagBlock)));


				auto softCeilingTagBlockResolved = tagBlockReader->read((uintptr_t)pSoftCeilingTagBlock);

				if (!softCeilingTagBlockResolved)
					return std::unexpected(softCeilingTagBlockResolved.error());

				LOG_ONCE(PLOG_DEBUG << "reading softCeilingTagBlock");
				// loop through softCeilings in the softCeilingTagBlock, grab the stringID, match to the meta info, then go to the softCeilingTriangle tagblock
				for (int softCeilingIndex = 0; softCeilingIndex < softCeilingTagBlockResolved.value().elementCount; softCeilingIndex++)
				{
					softCeilingDataStruct->setIndex(softCeilingTagBlockResolved.value().firstElement, softCeilingIndex);
					auto stringID = *softCeilingDataStruct->field<uint32_t>(softCeilingDataFields::stringID);

					if (softCeilingMetaInfoMap.contains(stringID) == false)
					{
						return std::unexpected(HCMRuntimeException(std::format("softCeilingData did not have match in metaData, for string ID {:X}, softCeilingIndex {:X}, sddtTagDatum {}", stringID, softCeilingIndex, sddtTag.tagDatum.toString())));
					}

					auto& matchedSoftCeilingMeta = softCeilingMetaInfoMap.at(stringID);

					auto* softCeilingTriangleTagBlock = softCeilingDataStruct->field<uint32_t>(softCeilingDataFields::softCeilingTriangleTagBlock);
					auto softCeilingTriangleTagBlockResolved = tagBlockReader->read((uintptr_t)softCeilingTriangleTagBlock);
					if (!softCeilingTriangleTagBlockResolved) 
						return std::unexpected(softCeilingTriangleTagBlockResolved.error());

					PLOG_DEBUG << "reading softCeilingTagBlock at address: " << std::hex << softCeilingDataStruct->currentBaseAddress;;
					// loop through each triangle. emplace into outvec together with the meta info
					for (int softCeilingTriangleIndex = 0; softCeilingTriangleIndex < softCeilingTriangleTagBlockResolved.value().elementCount; softCeilingTriangleIndex++)
					{
						softCeilingTriangleDataStruct->setIndex(softCeilingTriangleTagBlockResolved.value().firstElement, softCeilingTriangleIndex);

#ifdef HCM_DEBUG
						PLOG_DEBUG << "softCeilingTriangleDataStruct->currentBaseAddress" << std::hex << softCeilingTriangleDataStruct->currentBaseAddress;
#endif

						std::array<SimpleMath::Vector3, 3> vertices;
						vertices[0] = *softCeilingTriangleDataStruct->field<SimpleMath::Vector3>(softCeilingTriangleDataFields::vertex0);
						vertices[1] = *softCeilingTriangleDataStruct->field<SimpleMath::Vector3>(softCeilingTriangleDataFields::vertex1);
						vertices[2] = *softCeilingTriangleDataStruct->field<SimpleMath::Vector3>(softCeilingTriangleDataFields::vertex2);

#ifdef HCM_DEBUG
						PLOG_DEBUG << "vert0: " << vertices[0];
						PLOG_DEBUG << "vert1: " << vertices[1];
						PLOG_DEBUG << "vert2: " << vertices[2];
#endif

						outVec.emplace_back(SoftCeilingData(
							matchedSoftCeilingMeta.softCeilingObjectMask,
							matchedSoftCeilingMeta.softCeilingType,
							vertices
						));
					}
				}

			}
			
			PLOG_DEBUG << "ceiling data acquired, triangle count: " << outVec.size();

			return outVec;
		}
		catch (HCMRuntimeException ex)
		{
			return std::unexpected(ex);
		}
	}
};





template<GameState::Value mGame>
class SoftCeilingDataCacher : public IGetSoftCeilingDataImpl
{
private:
	// callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;
	ScopedCallback<ToggleEvent> softCeilingBarrierToggleCallback;

	// these clear the colour cache
	ScopedCallback < eventpp::CallbackList<void(SimpleMath::Vector4&) >> softCeilingOverlayColorAccelChangedCallback;
	ScopedCallback < eventpp::CallbackList<void(SimpleMath::Vector4&)>> softCeilingOverlayColorSlippyChangedCallback;
	ScopedCallback < eventpp::CallbackList<void(SimpleMath::Vector4&)>> softCeilingOverlayColorKillChangedCallback;
	ScopedCallback < eventpp::CallbackList<void(float&)>> softCeilingOverlaySolidTransparencyChangedCallback;
	ScopedCallback < eventpp::CallbackList<void(float&)>> softCeilingOverlayWireframeTransparencyChangedCallback;

	// bsp change callbacks. clear cache. null when service isn't requested
	std::unique_ptr<ScopedCallback<eventpp::CallbackList<void(BSPSet)>>> BSPSetChangeEventCallback;
	std::unique_ptr<ScopedCallback<eventpp::CallbackList<void(uint32_t)>>> ZoneSetChangeEventCallback;

	// injected services
	std::unique_ptr<ISoftCeilingDataFactory> mSoftCeilingDataFactory;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::shared_ptr< SettingsStateAndEvents> settings;
	std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;
	std::optional<std::shared_ptr<BSPSetChangeHookEvent>> BSPSetChangeEventProvider;
	std::optional<std::shared_ptr<ZoneSetChangeHookEvent>> ZoneSetChangeEventProvider;

	// resolved data
	libguarded::plain_guarded<SoftCeilingVector> softCeilingData = libguarded::plain_guarded<SoftCeilingVector>();
	bool dataCacheValid = false;
	bool colourCacheValid = false;


	std::optional<HCMRuntimeException> updateColorCache()
	{
		PLOG_DEBUG << "updating color cache";

		auto softCeilingDataLock = softCeilingData.lock();

		// we will multiply colors w/ transparencies so make copies
		auto solidColorAccel = settings->softCeilingOverlayColorAccel->GetValue();
		auto solidColorSlippy = settings->softCeilingOverlayColorSlippy->GetValue();
		auto solidColorKill = settings->softCeilingOverlayColorKill->GetValue();


		float opacitySolid = settings->softCeilingOverlaySolidTransparency->GetValue();

		solidColorAccel.w = opacitySolid;
		solidColorSlippy.w = opacitySolid;
		solidColorKill.w = opacitySolid;


		float opacityWireframe = settings->softCeilingOverlayWireframeTransparency->GetValue();

		auto wireframeColorAccel = solidColorAccel;
		auto wireframeColorSlippy = solidColorSlippy;
		auto wireframeColorKill = solidColorKill;

		wireframeColorAccel.w = opacityWireframe;
		wireframeColorSlippy.w = opacityWireframe;
		wireframeColorKill.w = opacityWireframe;

		// loop thru tris and apply colours
		for (auto& softCeiling : *softCeilingDataLock.get())
		{
			softCeiling.colorSolid = softCeiling.softCeilingType == SoftCeilingType::Acceleration ? solidColorAccel :
				softCeiling.softCeilingType == SoftCeilingType::SoftKill ? solidColorKill : solidColorSlippy;

			softCeiling.colorWireframe = softCeiling.softCeilingType == SoftCeilingType::Acceleration ? wireframeColorAccel :
				softCeiling.softCeilingType == SoftCeilingType::SoftKill ? wireframeColorKill : wireframeColorSlippy;
		}

		colourCacheValid = true;

		return std::nullopt;

	}

	std::optional<HCMRuntimeException> updateDataCache()
	{
		auto newData = mSoftCeilingDataFactory->getSoftCeilings();
		if (!newData) 
			return newData.error();


		auto softCeilingDataLock = softCeilingData.lock();
		softCeilingDataLock->clear();
		for (auto& sc : newData.value())
		{
			softCeilingDataLock->push_back(sc); // makes a copy. TODO: uncopify
		}
		dataCacheValid = true;
		colourCacheValid = false;

		return std::nullopt;
	}


public:

	SoftCeilingDataCacher(GameState game, IDIContainer& dicon, std::unique_ptr<ISoftCeilingDataFactory> softCeilingDataFactory)
		:
		mSoftCeilingDataFactory(std::move(softCeilingDataFactory)),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) { dataCacheValid = false; }),
		softCeilingBarrierToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->softCeilingOverlayToggle->valueChangedEvent, [this](bool& n) {dataCacheValid = false; }),
		settings(dicon.Resolve<SettingsStateAndEvents>().lock()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>().lock()),
		softCeilingOverlayColorAccelChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->softCeilingOverlayColorAccel->valueChangedEvent, [=](auto&) { colourCacheValid = false; }),
		softCeilingOverlayColorSlippyChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->softCeilingOverlayColorSlippy->valueChangedEvent, [=](auto&) { colourCacheValid = false; }),
		softCeilingOverlayColorKillChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->softCeilingOverlayColorKill->valueChangedEvent, [=](auto&) { colourCacheValid = false; }),
		softCeilingOverlaySolidTransparencyChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->softCeilingOverlaySolidTransparency->valueChangedEvent, [=](auto&) { colourCacheValid = false; }),
		softCeilingOverlayWireframeTransparencyChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->softCeilingOverlayWireframeTransparency->valueChangedEvent, [=](auto&) { colourCacheValid = false; })
	{

		// setup bsp change callbacks for clearing cache. these aren't required so no biggy if they fail.
		try
		{
			BSPSetChangeEventProvider = resolveDependentCheat(BSPSetChangeHookEvent);
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "Failed to resolve BSPSetChangeEventCallback, continuing anyway";
		}

		try
		{
			ZoneSetChangeEventProvider = resolveDependentCheat(ZoneSetChangeHookEvent);
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "Failed to resolve ZoneSetChangeEventCallback, continuing anyway";
		}
	}

	virtual std::expected<SoftCeilingVectorLock, HCMRuntimeException> getSoftCeilings()  override
	{
		if (!dataCacheValid)
		{
			auto dataCacheSuccessfullyUpdated = updateDataCache();
			if (dataCacheSuccessfullyUpdated.has_value())
				return std::unexpected(dataCacheSuccessfullyUpdated.value());
		}

		if (!colourCacheValid)
		{
			auto colorCacheSuccessfullyUpdated = updateColorCache();
			if (colorCacheSuccessfullyUpdated.has_value())
				return std::unexpected(colorCacheSuccessfullyUpdated.value());
		}

		return softCeilingData.lock();
	}

	void updateRequestState(bool requested)
	{
		if (requested)
		{
			if (BSPSetChangeEventProvider.has_value())
				BSPSetChangeEventCallback = BSPSetChangeEventProvider.value()->getBSPSetChangeEvent()->subscribe([this](BSPSet) { dataCacheValid = false; });

			if (ZoneSetChangeEventProvider.has_value())
				ZoneSetChangeEventCallback = ZoneSetChangeEventProvider.value()->getZoneSetChangeEvent()->subscribe([this](uint32_t) { dataCacheValid = false; });
		}
		else
		{
			BSPSetChangeEventCallback.reset();
			ZoneSetChangeEventCallback.reset();
		}
	}



};

class SoftCeilingDataRequestManager : public TemplatedScopedServiceProvider<SoftCeilingDataProvider>
{
	std::shared_ptr< IGetSoftCeilingDataImpl> dataPimpl;

public:

	SoftCeilingDataRequestManager(std::shared_ptr< IGetSoftCeilingDataImpl> dataPimpl) : dataPimpl(dataPimpl) {}

	virtual std::unique_ptr<SoftCeilingDataProvider> makeRequest(std::string callerID) override
	{

		return std::make_unique< SoftCeilingDataProvider>(
			dataPimpl,
			shared_from_this(),
			callerID);
	}

	virtual void updateService() override
	{
		dataPimpl->updateRequestState(serviceIsRequested());
	}
};


GetSoftCeilingData::GetSoftCeilingData(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		throw HCMInitException("Soft ceilings not relavent for h1");
		break;

	case GameState::Value::Halo2:
		throw HCMInitException("Soft ceilings not relavent for h2");
		break;

	case GameState::Value::Halo3:
	{
		auto dataPimpl = std::make_shared<SoftCeilingDataCacher<GameState::Value::Halo3>>(gameImpl, dicon, std::make_unique<SoftCeilingDataFactoryImpl<GameState::Value::Halo3>>(gameImpl, dicon));
		pimpl = std::make_shared<SoftCeilingDataRequestManager>(dataPimpl);
		break;
	}

	case GameState::Value::Halo3ODST:
	{
		auto dataPimpl = std::make_shared<SoftCeilingDataCacher<GameState::Value::Halo3ODST>>(gameImpl, dicon, std::make_unique<SoftCeilingDataFactoryImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon));
		pimpl = std::make_shared<SoftCeilingDataRequestManager>(dataPimpl);
		break;
	}

	case GameState::Value::HaloReach:
	{
		auto dataPimpl = std::make_shared<SoftCeilingDataCacher<GameState::Value::HaloReach>>(gameImpl, dicon, std::make_unique<SoftCeilingDataFactoryImpl<GameState::Value::HaloReach>>(gameImpl, dicon));
		pimpl = std::make_shared<SoftCeilingDataRequestManager>(dataPimpl);
		break;
	}

	case GameState::Value::Halo4:
	{
		auto dataPimpl = std::make_shared<SoftCeilingDataCacher<GameState::Value::Halo4>>(gameImpl, dicon, std::make_unique<SoftCeilingDataFactoryImpl<GameState::Value::Halo4>>(gameImpl, dicon));
		pimpl = std::make_shared<SoftCeilingDataRequestManager>(dataPimpl);
		break;
	}
	default:
		throw HCMInitException(std::format("{} not impl yet", nameof(GetSoftCeilingData)));
	}
}
GetSoftCeilingData::~GetSoftCeilingData() = default; 