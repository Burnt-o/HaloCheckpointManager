#include "pch.h"
#include "GetSoftCeilingData.h"
#include "TagTableRange.h"
#include "IMakeOrGetCheat.h"
#include "IMCCStateHook.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "GetScenarioAddress.h"
#include "DynamicStructFactory.h"
#include "PointerDataStore.h"
#include "TagBlockReader.h"

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
	std::weak_ptr<TagTableRange> tagTableRangeWeak;
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
		: tagTableRangeWeak(resolveDependentCheat(TagTableRange)),
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




			lockOrThrow(tagTableRangeWeak, tagTableRange);
			auto tagTable = tagTableRange->getTagTableRange();

			if (!tagTable)
				return std::unexpected(tagTable.error());
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "acquired tag table range, tagCount: " << c, c = tagTable.value().size());


			// get all sddt tags
			std::vector<TagInfo> sddtTags;
			MagicString sddtMagic = MagicString("tdds");
			for (auto& tagMeta : tagTable.value())
			{
//#ifdef HCM_DEBUG
//				PLOG_DEBUG << "tagMagic: " << tagMeta.tagType.getString();
//				PLOG_DEBUG << "sddt: " << sddtMagic.getString();
//				
//#endif
				if (tagMeta.tagType == sddtMagic)
				{
					sddtTags.emplace_back(tagMeta);
				}
			}

			LOG_ONCE_CAPTURE(PLOG_DEBUG << "sddt tag count: " << c, c = sddtTags.size());

			for (auto& sddtTag : sddtTags)
			{
				PLOG_DEBUG << "sddtTagAddress: " << std::hex << sddtTag.tagAddress;
			}


			SoftCeilingVector outVec;

			// loop through all sddt tags, go to the softCeiling tagBlock
			for (auto& sddtTag : sddtTags)
			{
				sddtTagDataStruct->currentBaseAddress = sddtTag.tagAddress;
				auto softCeilingTagBlock = sddtTagDataStruct->field<uint32_t>(sddtTagDataFields::softCeilingTagBlock);
				auto softCeilingTagBlockResolved = tagBlockReader->read((uintptr_t)softCeilingTagBlock);

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

					LOG_ONCE(PLOG_DEBUG << "reading softCeilingTriangleTagBlock");
					// loop through each triangle. emplace into outvec together with the meta info
					for (int softCeilingTriangleIndex = 0; softCeilingTriangleIndex < softCeilingTriangleTagBlockResolved.value().elementCount; softCeilingTriangleIndex++)
					{
						softCeilingTriangleDataStruct->setIndex(softCeilingTriangleTagBlockResolved.value().firstElement, softCeilingTriangleIndex);

						std::array<SimpleMath::Vector3, 3> vertices;
						vertices[0] = *softCeilingTriangleDataStruct->field<SimpleMath::Vector3>(softCeilingTriangleDataFields::vertex0);
						vertices[1] = *softCeilingTriangleDataStruct->field<SimpleMath::Vector3>(softCeilingTriangleDataFields::vertex1);
						vertices[2] = *softCeilingTriangleDataStruct->field<SimpleMath::Vector3>(softCeilingTriangleDataFields::vertex2);

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
class SoftCeilingDataCacher : public GetSoftCeilingDataUntemplated
{
private:
	// callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;
	ScopedCallback<ToggleEvent> softCeilingBarrierToggleCallback;

	// injected services
	std::unique_ptr<ISoftCeilingDataFactory> mSoftCeilingDataFactory;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::shared_ptr< SettingsStateAndEvents> settings;
	std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;

	// resolved data
	libguarded::plain_guarded<SoftCeilingVector> softCeilingData = libguarded::plain_guarded<SoftCeilingVector>();
	bool cacheValid = false;

	std::expected<void, HCMRuntimeException> updateCache()
	{
		auto newData = mSoftCeilingDataFactory->getSoftCeilings();
		if (!newData) 
			return std::unexpected(newData.error());


		auto softCeilingDataLock = softCeilingData.lock();
		softCeilingDataLock->clear();
		for (auto& sc : newData.value())
		{
			softCeilingDataLock->push_back(sc); // makes a copy. TODO: uncopify
		}
		cacheValid = true;
	}

public:

	SoftCeilingDataCacher(GameState game, IDIContainer& dicon, std::unique_ptr<ISoftCeilingDataFactory> softCeilingDataFactory)
		:
		mSoftCeilingDataFactory(std::move(softCeilingDataFactory)),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) { cacheValid = false; }),
		softCeilingBarrierToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->softCeilingOverlayToggle->valueChangedEvent, [this](bool& n) {cacheValid = false; }),
		settings(dicon.Resolve<SettingsStateAndEvents>().lock()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>().lock())
	{

	}

	virtual std::expected<SoftCeilingVectorLock, HCMRuntimeException> getSoftCeilings()  override
	{
		if (!cacheValid)
		{
			auto cacheSuccessfullyUpdated = updateCache();
			if (!cacheSuccessfullyUpdated)
				return std::unexpected(cacheSuccessfullyUpdated.error());
		}


		return softCeilingData.lock();
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
		pimpl = std::make_unique<SoftCeilingDataCacher<GameState::Value::Halo3>>(gameImpl, dicon, std::make_unique<SoftCeilingDataFactoryImpl<GameState::Value::Halo3>>(gameImpl, dicon));
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<SoftCeilingDataCacher<GameState::Value::Halo3ODST>>(gameImpl, dicon, std::make_unique<SoftCeilingDataFactoryImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon));
		break;
	default:
		throw HCMInitException(std::format("{} not impl yet", nameof(GetSoftCeilingData)));
	}
}
GetSoftCeilingData::~GetSoftCeilingData() = default;