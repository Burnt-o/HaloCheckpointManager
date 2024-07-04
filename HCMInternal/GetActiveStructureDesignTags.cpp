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

	virtual std::vector<TagInfo> getActiveStructureDesignTags() override
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
		auto bspIndexSet = bspSet.toIndexSet();

		std::vector<TagInfo> out;

		PLOG_DEBUG << "structureBSPsTagBlock.value().elementCount: " << structureBSPsTagBlock.value().elementCount;
		PLOG_DEBUG << "loaded BSP count: " << bspIndexSet.size();
		PLOG_DEBUG << "bspSet.value: 0x" << bspSet.toHexadecimalString();

		lockOrThrow(tagReferenceReaderWeak, tagReferenceReader);
		for (uint8_t bspIndex : bspIndexSet)
		{
			PLOG_DEBUG << "Getting SDDT tag for bspIndex: " << bspIndex;

			if (bspIndex >= structureBSPsTagBlock.value().elementCount)
				throw HCMRuntimeException(std::format("loaded BSP index exceeded scnr tag BSP tag block count! observed: {}, max: {}", bspIndex, structureBSPsTagBlock.value().elementCount));

			structureBSPsMetaDataStruct->setIndex(structureBSPsTagBlock.value().firstElement, bspIndex);
			auto* pStructureDesignReference = structureBSPsMetaDataStruct->field<uint32_t>(structureBSPsMetaDataFields::structureDesignReference);
			if (IsBadReadPtr(pStructureDesignReference, sizeof(uint32_t)))
				throw HCMRuntimeException(std::format("Bad read of pStructureDesignReference at {:X}", (uintptr_t)pStructureDesignReference));

			auto tagInfo = tagReferenceReader->read((uintptr_t)pStructureDesignReference);
			if (!tagInfo)
				throw tagInfo.error();

			PLOG_DEBUG << "Found SDDT tag for bspIndex: " << bspIndex << " with datum: " << tagInfo.value().tagDatum.toString();

			out.push_back(tagInfo.value());

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

public:
	GetActiveStructureDesignTagsImplZoneSet(GameState game, IDIContainer& dicon)
	{
		throw HCMInitException("Not impl yet");
	}

	virtual std::vector<TagInfo> getActiveStructureDesignTags() override
	{
		throw HCMRuntimeException("Not impl yet");
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

