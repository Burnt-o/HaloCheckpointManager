#include "pch.h"
#include "GetCurrentBSPSet.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "TagBlockReader.h"
#include "GetScenarioAddress.h"
#include "DynamicStructFactory.h"
#include "IMakeOrGetCheat.h"

class GetCurrentBSPSet::GetCurrentBSPSetImpl
{
private:
	std::shared_ptr<MultilevelPointer> currentBSPSet;

	enum class scenarioTagDataFields { StructureBSPsTagBlock };
	std::shared_ptr<DynamicStruct<scenarioTagDataFields>> scenarioTagDataStruct;

	std::weak_ptr< TagBlockReader> tagBlockReaderWeak;
	std::weak_ptr<GetScenarioAddress> getScenarioAddressWeak;
public:

	GetCurrentBSPSetImpl(GameState game, IDIContainer& dicon)
		:	tagBlockReaderWeak(resolveDependentCheat(TagBlockReader)),
		getScenarioAddressWeak(resolveDependentCheat(GetScenarioAddress))
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		currentBSPSet = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(currentBSPSet), game);
		scenarioTagDataStruct = DynamicStructFactory::make< scenarioTagDataFields>(ptr, game);
	}
	const BSPSet getCurrentBSPSet()
	{
		BSPSet bspSet(BSPIndex::None);
		if (!currentBSPSet->readData(&bspSet)) 
			throw HCMRuntimeException(std::format("Could not read currentBSPSet, {}", MultilevelPointer::GetLastError()));

		return bspSet;
	}

	const int getMaxBSPIndex()
	{
		lockOrThrow(getScenarioAddressWeak, getScenarioAddress);
		auto scenAddress = getScenarioAddress->getScenarioAddress();
		if (!scenAddress)
			throw scenAddress.error();

		scenarioTagDataStruct->currentBaseAddress = scenAddress.value();
		auto* pStructureBSPsTagBlock = scenarioTagDataStruct->field<uint32_t>(scenarioTagDataFields::StructureBSPsTagBlock);

		if (IsBadReadPtr(pStructureBSPsTagBlock, sizeof(uint32_t)))
			throw HCMRuntimeException(std::format("Bad read of pStructureBSPsTagBlock at 0x{:X}", (uintptr_t)pStructureBSPsTagBlock));

		lockOrThrow(tagBlockReaderWeak, tagBlockReader);
		auto StructureBSPsTagBlock = tagBlockReader->read((uintptr_t)pStructureBSPsTagBlock);

		if (!StructureBSPsTagBlock)
			throw StructureBSPsTagBlock.error();

		return StructureBSPsTagBlock.value().elementCount - 1;

	}
};




GetCurrentBSPSet::GetCurrentBSPSet(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique< GetCurrentBSPSetImpl>(game, dicon);
}

GetCurrentBSPSet::~GetCurrentBSPSet() = default;

const BSPSet GetCurrentBSPSet::getCurrentBSPSet() { return pimpl->getCurrentBSPSet(); }
const int GetCurrentBSPSet::getMaxBSPIndex() { return pimpl->getMaxBSPIndex(); }