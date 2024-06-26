#include "pch.h"
#include "GetTagAddress.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "DynamicStructFactory.h"
#include "IMCCStateHook.h"

// used for checking magic strings "scnr" "tags" "bipd" etc
union char_int {
	char chars[4];
	int32_t num;
};


// First gen (ie h1) stores an offset to the tag as part of the tagElementDataStructure. just add this offset to metaHeaderAddress (same thing as magicAddress in 1st gen) and boom there's ya tag
class GetTagAddressImplTagElement : public IGetTagAddressImpl
{
private:

	// callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;
	// event to fire on mcc state change (just clear the caches)
	void onGameStateChange(const MCCState&)
	{
		PLOG_INFO << "clearing " << nameof(GetTagAddressImplTagElement) << " caches";
		cacheValid = false;
	}

	//data
	enum class metaHeaderDataFields { tagTableOffset, numberOfTags, magic, scenarioDatum};
	std::shared_ptr<DynamicStruct<metaHeaderDataFields>> metaHeaderDataStruct;

	enum class tagElementDataFields { tagDatum, offset };
	std::shared_ptr<StrideableDynamicStruct<tagElementDataFields>> tagElementDataStruct;

	std::shared_ptr<MultilevelPointer> metaHeaderAddress;
	int64_t tagBase;

	char_int expectedMagic;

	// cache
	bool cacheValid = false;
	uintptr_t cachedMetaHeaderAddress;
	uintptr_t cachedTagTableAddress;
	int cachedTagCount;



public:
	GetTagAddressImplTagElement(GameState game, IDIContainer& dicon)
		:
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) { onGameStateChange(n); })
	{
		expectedMagic.chars[3] = 't';
		expectedMagic.chars[2] = 'a';
		expectedMagic.chars[1] = 'g';
		expectedMagic.chars[0] = 's';

		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		tagElementDataStruct = DynamicStructFactory::makeStrideable<tagElementDataFields>(ptr, game);
		metaHeaderDataStruct = DynamicStructFactory::make<metaHeaderDataFields>(ptr, game);
		metaHeaderAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(metaHeaderAddress), game);
		tagBase = *ptr->getData<std::shared_ptr<int64_t>>(nameof(tagBase), game).get();
	}

	void updateCache()
	{
		if (!cacheValid)
		{
			LOG_ONCE(PLOG_DEBUG << "Resolving caches");

			// resolve meta header address
			if (!metaHeaderAddress->resolve(&cachedMetaHeaderAddress)) throw HCMRuntimeException(std::format("Could not resolve metaHeaderAddress, {}", MultilevelPointer::GetLastError()));
			metaHeaderDataStruct->currentBaseAddress = cachedMetaHeaderAddress;

			// validate magic
			char_int observedMagic = *metaHeaderDataStruct->field<char_int>(metaHeaderDataFields::magic);
			if (observedMagic.num != expectedMagic.num) throw HCMRuntimeException(std::format("metaHeaderTable magic mismatch! Expected: {}, observed: {}", expectedMagic.chars, observedMagic.chars));

			// resolve tag count
			cachedTagCount = *metaHeaderDataStruct->field<int32_t>(metaHeaderDataFields::numberOfTags);
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "cached tag count: " << tc, tc = cachedTagCount);

			// resolve tag table address
			int64_t tagTableOffset = *metaHeaderDataStruct->field<int32_t>(metaHeaderDataFields::tagTableOffset);
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagTableOffset: " << std::hex << tt, tt = tagTableOffset);
			tagTableOffset += tagBase;
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagTableOffset after adding tagbase: " << std::hex << tt, tt = tagTableOffset);
			cachedTagTableAddress = cachedMetaHeaderAddress + tagTableOffset;
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "cachedTagTableAddress: " << std::hex << tta, tta = cachedTagTableAddress);

			cacheValid = true;
		}
	}


	virtual uintptr_t getTagAddress(Datum tagDatum) override  	// so get the tagElement of the tagDatum, lookup the nameOffset, add to the currentCacheAddress
	{

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "resolving tag name for tagDatum: " << tdatum, tdatum = tagDatum);
		updateCache();
		

		if (tagDatum.index > cachedTagCount) throw HCMRuntimeException(std::format("tagDatumIndex out of range! index was {:X}, tagCount was {:X}", tagDatum.index, cachedTagCount));

		tagElementDataStruct->setIndex(cachedTagTableAddress, tagDatum.index);

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagElementDataStruct at index: " << tdidx, tdidx = tagDatum.index);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "stride size is: " << ss, ss = tagElementDataStruct->StrideSize);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagElement header address: 0x" << std::hex << taddy, taddy = tagElementDataStruct->currentBaseAddress);

		// datum at element should exactly match passed tagDatum, this is a safety check
		Datum* datumAtElement = tagElementDataStruct->field<Datum>(tagElementDataFields::tagDatum);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tag datum check address: " << std::hex << ptdatum, ptdatum = (uintptr_t)datumAtElement);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "resolved datum: " << tdatum, tdatum = *datumAtElement);

		if (*datumAtElement != tagDatum) throw HCMRuntimeException(std::format("tagDatum mismatch! Expected {}, observed {}", tagDatum.toString(), datumAtElement->toString()));

		auto* tagOffset = tagElementDataStruct->field<uint32_t>(tagElementDataFields::offset);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "pTagOffset: " << std::hex << pno, pno = (uintptr_t)tagOffset);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagOffset: " << to, to = *tagOffset);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagBase: " << tb, tb = tagBase);
		uintptr_t tagAddress = cachedMetaHeaderAddress + *tagOffset + tagBase;
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagAddress: " << std::hex << ta, ta = tagAddress);

		if (IsBadReadPtr((void*)tagAddress, 8)) throw HCMRuntimeException(std::format("Bad read address for tagDatum {} at {}", tagDatum.toString(), tagAddress));

		return tagAddress; 
	}

	//virtual uintptr_t getScenarioAddress() override
	//{
	//	// Get the scenario datum from the metaHeader struct, then call getTagAddress on it.
	//	updateCache();

	//	auto* pScenarioDatum = metaHeaderDataStruct->field<uint32_t>(metaHeaderDataFields::scenarioDatum);
	//	if (IsBadReadPtr((void*)pScenarioDatum, 4)) throw HCMRuntimeException(std::format("Bad read address for pScenarioDatum at {:x}", (uintptr_t)pScenarioDatum));

	//	return getTagAddress(*pScenarioDatum);

	//}
};


// H2 is the same as H1 except that the tagTableAddress is looked up via pointer instead of adding an offset to metaHeaderAddress
class GetTagAddressImplTagElementOffsetTablePointer : public IGetTagAddressImpl
{
private:

	// callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;
	// event to fire on mcc state change (just clear the caches)
	void onGameStateChange(const MCCState&)
	{
		PLOG_INFO << "clearing " << nameof(GetTagAddressImplTagElement) << " caches";
		cacheValid = false;
	}

	//data
	enum class metaHeaderDataFields { numberOfTags, magic, scenarioDatum };
	std::shared_ptr<DynamicStruct<metaHeaderDataFields>> metaHeaderDataStruct;

	enum class tagElementDataFields { tagDatum, offset };
	std::shared_ptr<StrideableDynamicStruct<tagElementDataFields>> tagElementDataStruct;

	std::shared_ptr<MultilevelPointer> metaHeaderAddress;
	std::shared_ptr<MultilevelPointer> tagTableAddress;
	int64_t tagBase;

	char_int expectedMagic;

	// cache
	bool cacheValid = false;
	uintptr_t cachedMetaHeaderAddress;
	uintptr_t cachedTagTableAddress;
	int cachedTagCount;



public:
	GetTagAddressImplTagElementOffsetTablePointer(GameState game, IDIContainer& dicon)
		:
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) { onGameStateChange(n); })
	{
		expectedMagic.chars[3] = 't';
		expectedMagic.chars[2] = 'a';
		expectedMagic.chars[1] = 'g';
		expectedMagic.chars[0] = 's';

		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		tagElementDataStruct = DynamicStructFactory::makeStrideable<tagElementDataFields>(ptr, game);
		metaHeaderDataStruct = DynamicStructFactory::make<metaHeaderDataFields>(ptr, game);
		metaHeaderAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(metaHeaderAddress), game);
		tagBase = *ptr->getData<std::shared_ptr<int64_t>>(nameof(tagBase), game).get();
		tagTableAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(tagTableAddress), game);
	}

	void updateCache()
	{
		if (!cacheValid)
		{
			LOG_ONCE(PLOG_DEBUG << "Resolving caches");

			// resolve meta header address
			if (!metaHeaderAddress->resolve(&cachedMetaHeaderAddress)) throw HCMRuntimeException(std::format("Could not resolve metaHeaderAddress, {}", MultilevelPointer::GetLastError()));
			metaHeaderDataStruct->currentBaseAddress = cachedMetaHeaderAddress;

			// validate magic
			char_int observedMagic = *metaHeaderDataStruct->field<char_int>(metaHeaderDataFields::magic);
			if (observedMagic.num != expectedMagic.num) throw HCMRuntimeException(std::format("metaHeaderTable magic mismatch! Expected: {}, observed: {}", expectedMagic.chars, observedMagic.chars));

			// resolve tag count
			cachedTagCount = *metaHeaderDataStruct->field<int32_t>(metaHeaderDataFields::numberOfTags);
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "cached tag count: " << tc, tc = cachedTagCount);

			// resolve tag table address
			if (!tagTableAddress->resolve(&cachedTagTableAddress)) throw HCMRuntimeException(std::format("Could not resolve tagTableAddress, {}", MultilevelPointer::GetLastError()));
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "cachedTagTableAddress: " << std::hex << tta, tta = cachedTagTableAddress);

			cacheValid = true;
		}
	}


	virtual uintptr_t getTagAddress(Datum tagDatum) override  	// so get the tagElement of the tagDatum, lookup the nameOffset, add to the currentCacheAddress
	{

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "resolving tag name for tagDatum: " << tdatum, tdatum = tagDatum);
		updateCache();


		if (tagDatum.index > cachedTagCount) throw HCMRuntimeException(std::format("tagDatumIndex out of range! index was {:X}, tagCount was {:X}", tagDatum.index, cachedTagCount));

		tagElementDataStruct->setIndex(cachedTagTableAddress, tagDatum.index);

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagElementDataStruct at index: " << tdidx, tdidx = tagDatum.index);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "stride size is: " << ss, ss = tagElementDataStruct->StrideSize);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagElement header address: 0x" << std::hex << taddy, taddy = tagElementDataStruct->currentBaseAddress);

		// datum at element should exactly match passed tagDatum, this is a safety check
		Datum* datumAtElement = tagElementDataStruct->field<Datum>(tagElementDataFields::tagDatum);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tag datum check address: " << std::hex << ptdatum, ptdatum = (uintptr_t)datumAtElement);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "resolved datum: " << tdatum, tdatum = *datumAtElement);

		if (*datumAtElement != tagDatum) throw HCMRuntimeException(std::format("tagDatum mismatch! Expected {}, observed {}", tagDatum.toString(), datumAtElement->toString()));

		auto* tagOffset = tagElementDataStruct->field<uint32_t>(tagElementDataFields::offset);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "pTagOffset: " << std::hex << pno, pno = (uintptr_t)tagOffset);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagOffset: " << to, to = *tagOffset);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagBase: " << tb, tb = tagBase);
		uintptr_t tagAddress = cachedMetaHeaderAddress + *tagOffset + tagBase;
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagAddress: " << std::hex << ta, ta = tagAddress);

		if (IsBadReadPtr((void*)tagAddress, 8)) throw HCMRuntimeException(std::format("Bad read address for tagDatum {} at {}", tagDatum.toString(), tagAddress));

		return tagAddress;
	}

	//virtual uintptr_t getScenarioAddress() override
	//{
	//	// Get the scenario datum from the metaHeader struct, then call getTagAddress on it.
	//	updateCache();

	//	auto* pScenarioDatum = metaHeaderDataStruct->field<uint32_t>(metaHeaderDataFields::scenarioDatum);
	//	if (IsBadReadPtr((void*)pScenarioDatum, 4)) throw HCMRuntimeException(std::format("Bad read address for pScenarioDatum at {:x}", (uintptr_t)pScenarioDatum));

	//	return getTagAddress(*pScenarioDatum);

	//}
};

GetTagAddress::GetTagAddress(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<GetTagAddressImplTagElement>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<GetTagAddressImplTagElementOffsetTablePointer>(gameImpl, dicon);
		break;

	//case GameState::Value::Halo3:
	//	pimpl = std::make_unique<GetTagAddressImplTagElement>(gameImpl, dicon);
	//	break;

	//case GameState::Value::Halo3ODST:
	//	pimpl = std::make_unique<GetTagAddressImplTagElement>(gameImpl, dicon);
	//	break;

	//case GameState::Value::HaloReach:
	//	pimpl = std::make_unique<GetTagAddressImplTagElement>(gameImpl, dicon);
	//	break;

	//case GameState::Value::Halo4:
	//	pimpl = std::make_unique<GetTagAddressImplTagElement>(gameImpl, dicon);
	//	break;

	default:
		throw HCMInitException("Not impl yet");
	}
}

GetTagAddress::~GetTagAddress()
{
	PLOG_DEBUG << "~" << getName();
}


uintptr_t GetTagAddress::getTagAddress(Datum tagDatum) { return pimpl->getTagAddress(tagDatum); }
//uintptr_t GetTagAddress::getScenarioAddress() { return pimpl->getScenarioAddress();  }