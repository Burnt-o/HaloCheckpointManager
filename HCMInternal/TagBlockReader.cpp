#include "pch.h"
#include "TagBlockReader.h"
#include "MultilevelPointer.h"
#include "IMCCStateHook.h"
#include "DynamicStructFactory.h"

template<GameState::Value gameT>
class TagBlockReaderImplMagic : public TagBlockReader::ITagBlockReader
{
private:
	// callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	std::shared_ptr<MultilevelPointer> magicAddress;
	uintptr_t magicAddressCached;
	int64_t tagBase;

	bool cacheValid = false;

	enum class tagBlockDataFields { entryCount, offset };
	std::shared_ptr<DynamicStruct<tagBlockDataFields>> tagBlockDataStruct;

	std::function<uintptr_t(uint32_t&)> mOffsetTransformation;
public:
	TagBlockReaderImplMagic(GameState game, IDIContainer& dicon, std::function<uintptr_t(uint32_t&)> offsetTransformation)
		: MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) { cacheValid = false; }),
		mOffsetTransformation(offsetTransformation)
	{
		auto ptr = dicon.Resolve< PointerDataStore>().lock();
		magicAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(magicAddress), game);
		tagBase = *ptr->getData<std::shared_ptr<int64_t>>(nameof(tagBase), game).get();
		tagBlockDataStruct = DynamicStructFactory::make<tagBlockDataFields>(ptr, game);
	}

	std::optional<HCMRuntimeException> updateCache()
	{
		if (!magicAddress->readData(&magicAddressCached)) 
			return HCMRuntimeException(std::format("Could not resolve magicAddress, {}", MultilevelPointer::GetLastError()));
		PLOG_DEBUG << "tagBlockReader cache updated, magicAddress: " << std::hex << magicAddressCached;
		return std::nullopt;
	}

	std::expected<ParsedTagBlock, HCMRuntimeException> read(uintptr_t tagBlock)
	{
		LOG_ONCE(PLOG_DEBUG << "reading tagblock");
		if (!cacheValid)
		{
			auto cacheUpdated = updateCache();
			if (cacheUpdated.has_value())
			{
				return std::unexpected(cacheUpdated.value());
			}
				
			cacheValid = true;
		}

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagBlock: " << std::hex << p, p = tagBlock);

		tagBlockDataStruct->currentBaseAddress = tagBlock;

		auto* pOffset = tagBlockDataStruct->field<uint32_t>(tagBlockDataFields::offset);
		auto* pEntryCount = tagBlockDataStruct->field<uint32_t>(tagBlockDataFields::entryCount);
		if (IsBadReadPtr(pOffset, sizeof(uint32_t))) 
			return std::unexpected(HCMRuntimeException(std::format("Bad read of pOffset at 0x{:X}", (uintptr_t)pOffset)));
		if (IsBadReadPtr(pEntryCount, sizeof(uint32_t))) 
			return std::unexpected(HCMRuntimeException(std::format("Bad read of pEntryCount at 0x{:X}", (uintptr_t)pEntryCount)));

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "pOffset: " << std::hex << p, p = (uintptr_t)pOffset);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "*pOffset: " << std::hex << o, o = *pOffset);

		uintptr_t offsetTransformed = mOffsetTransformation(*pOffset);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "offsetTransformed: " << std::hex << o, o = offsetTransformed);

		uintptr_t pFirstElement = offsetTransformed + tagBase + magicAddressCached;
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "pFirstElement: " << std::hex << p, p = pFirstElement);
		if (IsBadReadPtr((void*)pFirstElement, sizeof(uint32_t)))
			return std::unexpected(HCMRuntimeException(std::format("Bad read of pFirstElement at 0x{:X}", pFirstElement)));

		return ParsedTagBlock(pFirstElement, *pEntryCount);
	}
};



TagBlockReader::TagBlockReader(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1: pimpl = std::make_unique<TagBlockReaderImplMagic<GameState::Value::Halo1>>(game, dicon, [](uint32_t& in) {return in; }); break;
	case GameState::Value::Halo2: pimpl = std::make_unique<TagBlockReaderImplMagic<GameState::Value::Halo2>>(game, dicon, [](uint32_t& in) {return in; }); break;
	case GameState::Value::Halo3: pimpl = std::make_unique<TagBlockReaderImplMagic<GameState::Value::Halo3>>(game, dicon, [](uint32_t& in) { return ((uintptr_t)in) << 2; }); break;
	case GameState::Value::Halo3ODST: pimpl = std::make_unique<TagBlockReaderImplMagic<GameState::Value::Halo3ODST>>(game, dicon, [](uint32_t& in) { return ((uintptr_t)in) << 2; }); break;
	//case GameState::Value::HaloReach: pimpl = std::make_unique<TagBlockReaderImpl<GameState::Value::HaloReach>>(game, dicon); break;
	//case GameState::Value::Halo4: pimpl = std::make_unique<TagBlockReaderImpl<GameState::Value::Halo4>>(game, dicon); break;
	default: throw HCMInitException("tagBlockReader not impl yet");
	}
}

TagBlockReader::~TagBlockReader() = default;