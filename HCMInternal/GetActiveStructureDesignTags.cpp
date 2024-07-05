#include "pch.h"
#include "GetActiveStructureDesignTags.h"
#include "GetCurrentBSPSet.h"
#include "GetCurrentZoneSet.h"
#include "DynamicStructFactory.h"
#include "IMakeOrGetCheat.h"
#include "TagReferenceReader.h"
#include "TagBlockReader.h"
#include "GetScenarioAddress.h"
#include "PointerDataStore.h"
#include "bitset_iter.h"


// h3 and ODST have their scnr tag associate each SBSP to a SDDT tag. the SDDT tag ref can be null, and can be shared between SBSPs.
// So we need to grab the current BSPSet, and lookup the SDDT tag associations from the scnr tag.
template <GameState::Value mGame>
class GetActiveStructureDesignTagsImplSBSP : public IGetActiveStructureDesignTags
{
private:

	std::weak_ptr< GetCurrentBSPSet> getCurrentBSPSetWeak;
	std::weak_ptr<TagReferenceReader> tagReferenceReaderWeak;
	std::weak_ptr<TagBlockReader> tagBlockReaderWeak;
	std::weak_ptr< GetScenarioAddress> getScenarioAddressWeak;

	enum class scenarioTagDataFields { StructureBSPsTagBlock };
	std::shared_ptr<DynamicStruct<scenarioTagDataFields>> scenarioTagDataStruct;


	// looked up from scen tag at above offset
	enum class structureBSPsMetaDataFields { structureDesignReference };
	std::shared_ptr<StrideableDynamicStruct<structureBSPsMetaDataFields>> structureBSPsMetaDataStruct;


public:
	GetActiveStructureDesignTagsImplSBSP(GameState game, IDIContainer& dicon)
		: getCurrentBSPSetWeak(resolveDependentCheat(GetCurrentBSPSet)),
		tagReferenceReaderWeak(resolveDependentCheat(TagReferenceReader)),
		tagBlockReaderWeak(resolveDependentCheat(TagBlockReader)),
		getScenarioAddressWeak(resolveDependentCheat(GetScenarioAddress))
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		scenarioTagDataStruct = DynamicStructFactory::make<scenarioTagDataFields>(ptr, game);
		structureBSPsMetaDataStruct = DynamicStructFactory::makeStrideable<structureBSPsMetaDataFields>(ptr, game);
	}

	virtual std::set<TagInfo> getActiveStructureDesignTags() override
	{
		lockOrThrow(getScenarioAddressWeak, getScenarioAddress);
		auto scenAddress = getScenarioAddress->getScenarioAddress();
		if (!scenAddress) 
			throw scenAddress.error();

		scenarioTagDataStruct->currentBaseAddress = scenAddress.value();


		auto* pStructureBSPsTagBlock = scenarioTagDataStruct->field<uint32_t>(scenarioTagDataFields::StructureBSPsTagBlock);
		if (IsBadReadPtr(pStructureBSPsTagBlock, sizeof(uint32_t)))
			throw HCMRuntimeException(std::format("Bad read of pStructureBSPsTagBlock at {:X}", (uintptr_t)pStructureBSPsTagBlock));

		lockOrThrow(tagBlockReaderWeak, tagBlockReader);
		auto structureBSPsTagBlock = tagBlockReader->read((uintptr_t)pStructureBSPsTagBlock);
		if (!structureBSPsTagBlock)
			throw structureBSPsTagBlock.error();

		lockOrThrow(getCurrentBSPSetWeak, getCurrentBSPSet);
		auto bspSet = getCurrentBSPSet->getCurrentBSPSet();

		// set because multiple sbsps can refer to the same sddt tag and we don't want duplicates
		std::set<TagInfo> out;

		PLOG_DEBUG << "structureBSPsTagBlock.value().elementCount: " << structureBSPsTagBlock.value().elementCount;
		PLOG_DEBUG << "loaded BSP count: " << bspSet.size();
		PLOG_DEBUG << "bspSet.value: 0x" << std::hex <<  bspSet.to_ulong();

		lockOrThrow(tagReferenceReaderWeak, tagReferenceReader);
		for (auto bspIndex : bitset::indices_on(bspSet))
		{
			PLOG_DEBUG << "Getting SDDT tag for bspIndex: " << bspIndex;

			if (bspIndex >= structureBSPsTagBlock.value().elementCount)
				throw HCMRuntimeException(std::format("loaded BSP index exceeded scnr tag BSP tag block count! observed: {}, max: {}", bspIndex, structureBSPsTagBlock.value().elementCount));

			structureBSPsMetaDataStruct->setIndex(structureBSPsTagBlock.value().firstElement, bspIndex);
			auto* pStructureDesignReference = structureBSPsMetaDataStruct->field<uint32_t>(structureBSPsMetaDataFields::structureDesignReference);
			if (IsBadReadPtr(pStructureDesignReference, sizeof(uint32_t)))
				throw HCMRuntimeException(std::format("Bad read of pStructureDesignReference at {:X}", (uintptr_t)pStructureDesignReference));


			auto checkTagRefNull = tagReferenceReader->isNull((uintptr_t)pStructureDesignReference);
			if (!checkTagRefNull)
				throw checkTagRefNull.error();

			if (checkTagRefNull.value())
				continue;

			auto tagInfo = tagReferenceReader->read((uintptr_t)pStructureDesignReference);
			if (!tagInfo)
			{
				throw tagInfo.error();
			}

			PLOG_DEBUG << "Found SDDT tag for bspIndex: " << bspIndex << " with datum: " << tagInfo.value().tagDatum.toString();

			out.insert(tagInfo.value());

		}

		return out;
	}
};

// hreach and h4 associate each Zone Set (in scnr tag) with a bitmask of SDDT indices, with the SDDT indices associated by a Structure Design tagblock in the scnr tag.
// So we need to lookup the current ZoneSet index, find the SDDT bitmask from the scnr ZoneSet tagblock, and associate each bit to the SDDT tag by index in the Structure Design Tagblock 
template <GameState::Value mGame>
class GetActiveStructureDesignTagsImplZoneSet : public IGetActiveStructureDesignTags
{
private:

	std::weak_ptr< GetCurrentZoneSet> getCurrentZoneSetWeak;
	std::weak_ptr<TagReferenceReader> tagReferenceReaderWeak;
	std::weak_ptr<TagBlockReader> tagBlockReaderWeak;
	std::weak_ptr< GetScenarioAddress> getScenarioAddressWeak;

	enum class scenarioTagDataFields { StructureDesignTagBlock, ZoneSetTagBlock };
	std::shared_ptr<DynamicStruct<scenarioTagDataFields>> scenarioTagDataStruct;


	// looked up from scen tag at StructureDesignTagBlock offset
	enum class structureDesignMetaDataFields { structureDesignReference };
	std::shared_ptr<StrideableDynamicStruct<structureDesignMetaDataFields>> structureDesignMetaDataStruct;

	// looked up from scen tag at ZoneSetTagBlock offset
	enum class zoneSetMetaDataFields { runtimeStructureDesignZoneMask };
	std::shared_ptr<StrideableDynamicStruct<zoneSetMetaDataFields>> zoneSetMetaDataStruct;


public:
	GetActiveStructureDesignTagsImplZoneSet(GameState game, IDIContainer& dicon)
		: getCurrentZoneSetWeak(resolveDependentCheat(GetCurrentZoneSet)),
		tagReferenceReaderWeak(resolveDependentCheat(TagReferenceReader)),
		tagBlockReaderWeak(resolveDependentCheat(TagBlockReader)),
		getScenarioAddressWeak(resolveDependentCheat(GetScenarioAddress))
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		scenarioTagDataStruct = DynamicStructFactory::make<scenarioTagDataFields>(ptr, game);
		structureDesignMetaDataStruct = DynamicStructFactory::makeStrideable<structureDesignMetaDataFields>(ptr, game);
		zoneSetMetaDataStruct = DynamicStructFactory::makeStrideable<zoneSetMetaDataFields>(ptr, game);
	}

	virtual std::set<TagInfo> getActiveStructureDesignTags() override
	{
		using SDDTMask = std::bitset<32>;

		lockOrThrow(getScenarioAddressWeak, getScenarioAddress);
		auto scenAddress = getScenarioAddress->getScenarioAddress();
		if (!scenAddress)
			throw scenAddress.error();

		scenarioTagDataStruct->currentBaseAddress = scenAddress.value();


		auto* pZoneSetTagBlock = scenarioTagDataStruct->field<uint32_t>(scenarioTagDataFields::ZoneSetTagBlock);
		if (IsBadReadPtr(pZoneSetTagBlock, sizeof(uint32_t)))
			throw HCMRuntimeException(std::format("Bad read of pZoneSetTagBlock at {:X}", (uintptr_t)pZoneSetTagBlock));

		lockOrThrow(tagBlockReaderWeak, tagBlockReader);
		auto ZoneSetTagBlock = tagBlockReader->read((uintptr_t)pZoneSetTagBlock);
		if (!ZoneSetTagBlock)
			throw ZoneSetTagBlock.error();

		lockOrThrow(getCurrentZoneSetWeak, getCurrentZoneSet);
		auto currentZoneSet = getCurrentZoneSet->getCurrentZoneSet();
		
		if (currentZoneSet >= ZoneSetTagBlock.value().elementCount)
			throw HCMRuntimeException(std::format("currentZoneSet exceeded zoneset tagblock size, observed {}, expected {}", currentZoneSet, ZoneSetTagBlock.value().elementCount));

		zoneSetMetaDataStruct->setIndex(ZoneSetTagBlock.value().firstElement, currentZoneSet);
		auto* pRuntimeStructureDesignZoneMask = zoneSetMetaDataStruct->field<SDDTMask>(zoneSetMetaDataFields::runtimeStructureDesignZoneMask);
		if (IsBadReadPtr(pRuntimeStructureDesignZoneMask, sizeof(SDDTMask)))
			throw HCMRuntimeException(std::format("Bad read of pRuntimeStructureDesignZoneMask at {:X}", (uintptr_t)pRuntimeStructureDesignZoneMask));

		auto runtimeStructureDesignZoneMask = *pRuntimeStructureDesignZoneMask;

		auto highestSDDTIndexSet = find_last_index_of_bit_set(runtimeStructureDesignZoneMask);
		// if no sddt indexes are set then we return an empty set
		if (highestSDDTIndexSet.has_value() == false)
			return std::set<TagInfo>();


		// now we need to go to the structure design tag block
		auto* pStructureDesignTagBlock = scenarioTagDataStruct->field<uint32_t>(scenarioTagDataFields::StructureDesignTagBlock);
		if (IsBadReadPtr(pStructureDesignTagBlock, sizeof(uint32_t)))
			throw HCMRuntimeException(std::format("Bad read of pStructureDesignTagBlock at {:X}", (uintptr_t)pStructureDesignTagBlock));

		auto StructureDesignTagBlock = tagBlockReader->read((uintptr_t)pStructureDesignTagBlock);
		if (!StructureDesignTagBlock)
			throw StructureDesignTagBlock.error();

		// if highestSDDTIndexSet is larger than the element count of the sddt tagblock, that's an uh oh
		if (highestSDDTIndexSet.value() >= StructureDesignTagBlock.value().elementCount)
			throw HCMRuntimeException(std::format("highestSDDTIndexSet exceeded sddt tagblock size, observed {}, expected {}", highestSDDTIndexSet.value(), StructureDesignTagBlock.value().elementCount));



		lockOrThrow(tagReferenceReaderWeak, tagReferenceReader);
		std::set<TagInfo> out;

		// for each index in the runtimeStructureDesignZoneMask, look it up in the SDDTTagBlock and resolve the tag reference, appending to out
		for (auto sddtIndex : bitset::indices_on(runtimeStructureDesignZoneMask))
		{

			PLOG_DEBUG << "looking up sddt index: " << sddtIndex;

			structureDesignMetaDataStruct->setIndex(StructureDesignTagBlock.value().firstElement, sddtIndex);

			auto* pSDDTref = structureDesignMetaDataStruct->field<uint32_t>(structureDesignMetaDataFields::structureDesignReference);
			if (IsBadReadPtr(pSDDTref, sizeof(uint32_t)))
				throw HCMRuntimeException(std::format("Bad read of pSDDTref at {:X}", (uintptr_t)pSDDTref));

			auto checkTagRefNull = tagReferenceReader->isNull((uintptr_t)pSDDTref);
			if (!checkTagRefNull)
				throw checkTagRefNull.error();

			if (checkTagRefNull.value())
				continue;

			auto tagInfo = tagReferenceReader->read((uintptr_t)pSDDTref);
			if (!tagInfo)
			{
				throw tagInfo.error();
			}

			PLOG_DEBUG << "Found SDDT tag matching SDDT index: " << sddtIndex << " with datum: " << tagInfo.value().tagDatum.toString() << " for zoneset index: " << currentZoneSet;

			out.insert(tagInfo.value());


		}

		return out;
	}
};


GetActiveStructureDesignTags::GetActiveStructureDesignTags(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1:
		throw HCMInitException("GetActiveStructureDesignTags not applicable to h1");
		break;

	case GameState::Value::Halo2:
		throw HCMInitException("GetActiveStructureDesignTags not applicable to h2");
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<GetActiveStructureDesignTagsImplSBSP<GameState::Value::Halo3>>(game, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<GetActiveStructureDesignTagsImplSBSP<GameState::Value::Halo3ODST>>(game, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<GetActiveStructureDesignTagsImplZoneSet<GameState::Value::HaloReach>>(game, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<GetActiveStructureDesignTagsImplZoneSet<GameState::Value::Halo4>>(game, dicon);
		break;

	default:
		throw HCMInitException("Not impl yet");
	}
}
GetActiveStructureDesignTags::~GetActiveStructureDesignTags() = default;

