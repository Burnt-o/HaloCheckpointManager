#include "pch.h"
#include "TagReferenceReader.h"
#include "MultilevelPointer.h"
#include "IMCCStateHook.h"
#include "DynamicStructFactory.h"
#include "TagTableRange.h"
#include "IMakeOrGetCheat.h"

template<GameState::Value gameT>
class TagReferenceReaderImpl : public TagReferenceReader::ITagReferenceReader
{
private:

	std::weak_ptr<TagTableRange> tagTableRangeWeak;

	enum class tagReferenceDataFields { datumIndex };
	std::shared_ptr<DynamicStruct<tagReferenceDataFields>> tagReferenceDataStruct;

public:
	TagReferenceReaderImpl(GameState game, IDIContainer& dicon)
		: tagTableRangeWeak(resolveDependentCheat(TagTableRange))
	{
		auto ptr = dicon.Resolve< PointerDataStore>().lock();
		tagReferenceDataStruct = DynamicStructFactory::make<tagReferenceDataFields>(ptr, game);
	}



	std::expected<TagInfo, HCMRuntimeException> read(uintptr_t tagReference)
	{

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagReference: " << std::hex << p, p = tagReference);

		tagReferenceDataStruct->currentBaseAddress = tagReference;

		auto* pDatumIndex = tagReferenceDataStruct->field<uint16_t>(tagReferenceDataFields::datumIndex);
		if (IsBadReadPtr(pDatumIndex, sizeof(uint16_t)))
			return std::unexpected(HCMRuntimeException(std::format("Bad read of pDatumIndex at 0x{:X}", (uintptr_t)pDatumIndex)));
	
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "pDatumIndex: " << std::hex << p, p = (uintptr_t)pDatumIndex);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "*pDatumIndex: " << std::hex << o, o = *pDatumIndex);

		lockOrThrow(tagTableRangeWeak, tagTableRange);
		return tagTableRange->getTagByIndex(*pDatumIndex);
	}
};



TagReferenceReader::TagReferenceReader(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1: pimpl = std::make_unique<TagReferenceReaderImpl<GameState::Value::Halo1>>(game, dicon); break;
	case GameState::Value::Halo2: pimpl = std::make_unique<TagReferenceReaderImpl<GameState::Value::Halo2>>(game, dicon); break;
	case GameState::Value::Halo3: pimpl = std::make_unique<TagReferenceReaderImpl<GameState::Value::Halo3>>(game, dicon); break;
	case GameState::Value::Halo3ODST: pimpl = std::make_unique<TagReferenceReaderImpl<GameState::Value::Halo3ODST>>(game, dicon); break;
	case GameState::Value::HaloReach: pimpl = std::make_unique<TagReferenceReaderImpl<GameState::Value::HaloReach>>(game, dicon); break;
	case GameState::Value::Halo4: pimpl = std::make_unique<TagReferenceReaderImpl<GameState::Value::Halo4>>(game, dicon); break;
	default: throw HCMInitException("tagReferenceReader not impl yet");
	}
}

TagReferenceReader::~TagReferenceReader() = default;