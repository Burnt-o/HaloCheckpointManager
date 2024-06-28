#include "pch.h"
#include "TagTableRange.h"
#include "MultilevelPointer.h"
#include "IMCCStateHook.h"
#include "DynamicStructFactory.h"


/*
TODO:
move injected depencies to different files for readability
replace the HCMRuntimeException in std::expected<T, HCMRuntimeException> with custom, more performant error enum/struct

*/

class ITagTypeResolver
{
public:
	virtual std::expected<MagicString, HCMRuntimeException> getTagType(uintptr_t tagElement) = 0;
	virtual ~ITagTypeResolver() = default;
};

// tag type is stored in the tag meta element (h1/h2)
class TagTypeResolveFromElement : public ITagTypeResolver
{
private: 
	enum class tagElementDataFields { tagGroupMagic };
	std::shared_ptr<DynamicStruct<tagElementDataFields>> tagMetaElementStruct;

public:
	TagTypeResolveFromElement(GameState game, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		tagMetaElementStruct = DynamicStructFactory::make<tagElementDataFields>(ptr, game);
	}

	virtual std::expected<MagicString, HCMRuntimeException> getTagType(uintptr_t tagElement) override
	{
		tagMetaElementStruct->currentBaseAddress = tagElement;
		return *tagMetaElementStruct->field<MagicString>(tagElementDataFields::tagGroupMagic);
	}
};

// tag element has index to tag group table (h3+)
class TagTypeResolveFromMagicTable : public ITagTypeResolver
{
private:
	enum class tagElementDataFields { tagGroupIndex };
	std::shared_ptr<DynamicStruct<tagElementDataFields>> tagMetaElementStruct;
	std::shared_ptr<MultilevelPointer> tagGroupTableAddress;

	enum class tagGroupDataFields { tagGroupMagic };
	std::shared_ptr<StrideableDynamicStruct<tagGroupDataFields>> tagGroupDataStruct;

public:
	TagTypeResolveFromMagicTable(GameState game, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		tagMetaElementStruct = DynamicStructFactory::make<tagElementDataFields>(ptr, game);
		tagGroupDataStruct = DynamicStructFactory::makeStrideable<tagGroupDataFields>(ptr, game);
		tagGroupTableAddress = ptr->getData < std::shared_ptr < MultilevelPointer>>(nameof(tagGroupTableAddress), game);
	}

	virtual std::expected<MagicString, HCMRuntimeException> getTagType(uintptr_t tagElement) override
	{
		tagMetaElementStruct->currentBaseAddress = tagElement;
		auto tagGroupIndex = *tagMetaElementStruct->field<uint16_t>(tagElementDataFields::tagGroupIndex);

		uintptr_t tagGroupTableResolved;
		if (!tagGroupTableAddress->resolve(&tagGroupTableResolved))
			return std::unexpected(HCMRuntimeException(std::format("Failed to resolve tagGroupTableAddress, {}", MultilevelPointer::GetLastError())));

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "tagGroupTableResolved: " << std::hex << p, p = tagGroupTableResolved);

		tagGroupDataStruct->setIndex(tagGroupTableResolved, tagGroupIndex);
		return *tagGroupDataStruct->field<MagicString>(tagGroupDataFields::tagGroupMagic);

	}
};


class ITagMetaTableAddressResolver
{
public:
	virtual ~ITagMetaTableAddressResolver() = default;
	virtual std::expected<uintptr_t, HCMRuntimeException> getTagMetaTableAddress() = 0;
};

class ITagMetaTableCountResolver
{
public:
	virtual ~ITagMetaTableCountResolver() = default;
	virtual std::expected<uint32_t, HCMRuntimeException> getTagMetaTableCount() = 0;
};

// looks it up from the metaHeader
class TagMetaTableAddressCountResolverFromMetaHeader : public ITagMetaTableAddressResolver, public ITagMetaTableCountResolver
{
private:
	std::shared_ptr<MultilevelPointer> metaHeaderAddress;

	enum class metaHeaderDataFields { tagTableOffset, numberOfTags };
	std::shared_ptr<DynamicStruct<metaHeaderDataFields>> metaHeaderDataStruct;
public:
	TagMetaTableAddressCountResolverFromMetaHeader(GameState game, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		metaHeaderAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(metaHeaderAddress), game);
		metaHeaderDataStruct = DynamicStructFactory::make<metaHeaderDataFields>(ptr, game);
	}

	virtual std::expected<uintptr_t, HCMRuntimeException> getTagMetaTableAddress() override
	{
		uintptr_t metaHeaderAddressResolved;
		if (!metaHeaderAddress->resolve(&metaHeaderAddressResolved)) 
			return std::unexpected(HCMRuntimeException(std::format("Could not resolve metaHeaderAddress, {}", MultilevelPointer::GetLastError())));

		metaHeaderDataStruct->currentBaseAddress = metaHeaderAddressResolved;
		uint32_t tagTableOffset = *metaHeaderDataStruct->field<uint32_t>(metaHeaderDataFields::tagTableOffset);

		return (metaHeaderAddressResolved + tagTableOffset);
	}

	virtual std::expected<uint32_t, HCMRuntimeException> getTagMetaTableCount() override
	{
		uintptr_t metaHeaderAddressResolved;
		if (!metaHeaderAddress->resolve(&metaHeaderAddressResolved))
			return std::unexpected(HCMRuntimeException(std::format("Could not resolve metaHeaderAddress, {}", MultilevelPointer::GetLastError())));

		metaHeaderDataStruct->currentBaseAddress = metaHeaderAddressResolved;
		uint32_t tagTableCount = *metaHeaderDataStruct->field<uint32_t>(metaHeaderDataFields::numberOfTags);

		return tagTableCount;
	}
};

// just looks up a pointer
class TagMetaTableAddressResolverFromPointer : public ITagMetaTableAddressResolver
{
private:
	std::shared_ptr<MultilevelPointer> tagTableAddress;

public:
	TagMetaTableAddressResolverFromPointer(GameState game, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		tagTableAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(tagTableAddress), game);
	}

	virtual std::expected<uintptr_t, HCMRuntimeException> getTagMetaTableAddress() override
	{
		uintptr_t tagTableAddressResolved;
		if (!tagTableAddress->resolve(&tagTableAddressResolved))
			return std::unexpected(HCMRuntimeException(std::format("Could not resolve tagTableAddress, {}", MultilevelPointer::GetLastError())));

		return tagTableAddressResolved;
	}

};

// just looks up a pointer
class TagMetaTableCountResolverFromPointer : public ITagMetaTableCountResolver
{
private:
	std::shared_ptr<MultilevelPointer> tagTableCount;

public:
	TagMetaTableCountResolverFromPointer(GameState game, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		tagTableCount = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(tagTableCount), game);
	}

	virtual std::expected<uint32_t, HCMRuntimeException> getTagMetaTableCount() override
	{
		uint32_t tagTableCountResolved;
		if (!tagTableCount->readData(&tagTableCountResolved))
			return std::unexpected(HCMRuntimeException(std::format("Could not resolve tagTableCount, {}", MultilevelPointer::GetLastError())));


		return tagTableCountResolved;
	}

};


template <GameState::Value gameT>
class TagTableRangeImpl : public ITagTableRangeImpl
{
private:
	// callbacks
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;


	std::shared_ptr<ITagMetaTableCountResolver> getTagMetaTableCount;
	std::shared_ptr<ITagMetaTableAddressResolver> getTagMetaTableAddress;
	std::shared_ptr<ITagTypeResolver> getTagType;
	std::shared_ptr<MultilevelPointer> magicAddress;
	int64_t tagBase;

	std::shared_ptr<MultilevelPointer> tagTableHeaderAddress;
	uintptr_t tagTableHeaderAddressCached;
	uintptr_t tagTableMetaDataAddressCached;
	std::vector<TagInfo> tagTableInfoCached;
	bool cacheValid = false;


	enum class tagElementDataFields { offset, datumIndexSalt
	};
	std::shared_ptr<StrideableDynamicStruct<tagElementDataFields>> tagMetaElementStruct;

	std::function<uintptr_t(uint32_t&)> mOffsetTransformation;

	std::expected<void, HCMRuntimeException> updateCache()
	{
		LOG_ONCE(PLOG_DEBUG << "updating tagTableRange cache");


		auto tagTableAddressResolved = getTagMetaTableAddress->getTagMetaTableAddress();
		if (!tagTableAddressResolved) 
			return std::unexpected(tagTableAddressResolved.error());

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "resolved tagTableAddress: 0x" << std::hex << p, p = tagTableAddressResolved.value());

		auto tagCount = getTagMetaTableCount->getTagMetaTableCount();
		if (!tagCount)
			return std::unexpected(tagCount.error());

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "resolved tagCount: 0x" << std::hex << c, c = tagCount.value());

		uintptr_t magicAddressResolved;
		if (!magicAddress->readData(&magicAddressResolved)) 
			return std::unexpected(HCMRuntimeException(std::format("Could not resolve magicAddress, {}", MultilevelPointer::GetLastError())));
		
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "resolved magicAddress: 0x" << std::hex << p, p = magicAddressResolved);

		std::vector<TagInfo> outVec;

		for (auto tagIndex = 0; tagIndex < tagCount.value(); tagIndex++)
		{
			tagMetaElementStruct->setIndex(tagTableAddressResolved.value(), tagIndex);
			auto tagType = getTagType->getTagType(tagMetaElementStruct->currentBaseAddress);
			if (!tagType)
				return std::unexpected(tagType.error());

			LOG_ONCE_CAPTURE(PLOG_DEBUG << "acquired first tag tagType: " << tt, tt = tagType.value().getString());

			Datum tagDatum;
			tagDatum.index = tagIndex;
			tagDatum.salt  = *tagMetaElementStruct->field<uint16_t>(tagElementDataFields::datumIndexSalt);

			if (tagDatum.index != tagIndex)
				return std::unexpected(HCMRuntimeException(std::format("Mismatched tagDatum getting TagTableRange: expected {:X}, observed {:X}", tagIndex, tagDatum.index)));

			LOG_ONCE_CAPTURE(PLOG_DEBUG << "first tag tagDatum: 0x" << td, td = tagDatum.toString());

			auto tagOffset = *tagMetaElementStruct->field<uint32_t>(tagElementDataFields::offset);

			LOG_ONCE_CAPTURE(PLOG_DEBUG << "first tag tagOffset (preTransform): 0x" << std::hex << to, to= tagOffset);

			auto tagOffsetPost = mOffsetTransformation(tagOffset);

			LOG_ONCE_CAPTURE(PLOG_DEBUG << "first tag tagOffset (postTransform): 0x" << std::hex << to, to = tagOffsetPost);

			uintptr_t tagAddress = tagOffsetPost + magicAddressResolved;

			LOG_ONCE_CAPTURE(PLOG_DEBUG << "first tag tagAddress: 0x" << std::hex << ta, ta = tagAddress);

			TagInfo out = TagInfo(tagType.value(), tagDatum, tagAddress);
			outVec.emplace_back(out);


		}

		tagTableInfoCached = outVec;
	}

public:
	TagTableRangeImpl(GameState game, IDIContainer& dicon, std::function<uintptr_t(uint32_t&)> offsetTransformation,
		std::shared_ptr<ITagMetaTableAddressResolver> getTagMetaTableAddress,
		std::shared_ptr<ITagMetaTableCountResolver> getTagMetaTableCount,
		std::shared_ptr<ITagTypeResolver> getTagType
	)
		: MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) { cacheValid = false; }),
		mOffsetTransformation(offsetTransformation),
		getTagMetaTableCount(getTagMetaTableCount),
		getTagMetaTableAddress(getTagMetaTableAddress),
		getTagType(getTagType)

	{
		auto ptr = dicon.Resolve< PointerDataStore>().lock();
		magicAddress = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(magicAddress), game);
		tagBase = *ptr->getData<std::shared_ptr<int64_t>>(nameof(tagBase), game).get();
		tagMetaElementStruct = DynamicStructFactory::makeStrideable<tagElementDataFields>(ptr, game);


	}

	virtual std::expected<std::vector<TagInfo>, HCMRuntimeException> getTagTableRange() override
	{
		if (!cacheValid)
		{
			auto cacheUpdated = updateCache();
			if (!cacheUpdated)
				return std::unexpected(cacheUpdated.error());
			cacheValid = true;
		}

		return tagTableInfoCached;

	}

	virtual std::expected<TagInfo, HCMRuntimeException> getTagByDatum(Datum tagDatum) override
	{
		if (!cacheValid)
		{
			auto cacheUpdated = updateCache();
			if (!cacheUpdated)
				return std::unexpected(cacheUpdated.error());
			cacheValid = true;
		}

		if (tagDatum.isNull())
			return std::unexpected(HCMRuntimeException("Null tag datum"));

		if (tagTableInfoCached.size() > tagDatum.index)
			return std::unexpected(HCMRuntimeException(std::format("tagDatum index ({:X}) larger than tagCount ({:X})", tagTableInfoCached.size(), tagDatum.index)));

		auto& out = tagTableInfoCached.at(tagDatum.index);
		if (out.tagDatum != tagDatum)
			return std::unexpected(HCMRuntimeException(std::format("tagDatum mismatch: expected {}, observed {}", tagDatum.toString(), out.tagDatum.toString())));

		return out;

	}
};

TagTableRange::TagTableRange(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1: 
	{
		auto tagMagicResolver = std::make_shared<TagTypeResolveFromElement>(game, dicon);
		auto h1TagTableResolver = std::make_shared < TagMetaTableAddressCountResolverFromMetaHeader>(game, dicon);
		pimpl = std::make_unique<TagTableRangeImpl<GameState::Value::Halo1>>(game, dicon, [](uint32_t& in) { return (uintptr_t)in;  }, h1TagTableResolver, h1TagTableResolver, tagMagicResolver);
	}		
	break;

	case GameState::Value::Halo2: 
	{
		auto tagMagicResolver = std::make_shared<TagTypeResolveFromElement>(game, dicon);
		auto tagTableAddressResolver = std::make_shared < TagMetaTableAddressResolverFromPointer>(game, dicon);
		auto tagTableCountResolver = std::make_shared < TagMetaTableCountResolverFromPointer>(game, dicon);
		pimpl = std::make_unique<TagTableRangeImpl<GameState::Value::Halo2>>(game, dicon, [](uint32_t& in) { return (uintptr_t)in;  }, tagTableAddressResolver, tagTableCountResolver, tagMagicResolver);
	}
		break;

	case GameState::Value::Halo3: 
	{
		auto tagMagicResolver = std::make_shared<TagTypeResolveFromMagicTable>(game, dicon);
		auto tagTableAddressResolver = std::make_shared < TagMetaTableAddressResolverFromPointer>(game, dicon);
		auto tagTableCountResolver = std::make_shared < TagMetaTableCountResolverFromPointer>(game, dicon);
		pimpl = std::make_unique<TagTableRangeImpl<GameState::Value::Halo3>>(game, dicon, [](uint32_t& in) { return (uintptr_t)in << 2;  }, tagTableAddressResolver, tagTableCountResolver, tagMagicResolver);
	}
	break;

	case GameState::Value::Halo3ODST:
	{
		auto tagMagicResolver = std::make_shared<TagTypeResolveFromMagicTable>(game, dicon);
		auto tagTableAddressResolver = std::make_shared < TagMetaTableAddressResolverFromPointer>(game, dicon);
		auto tagTableCountResolver = std::make_shared < TagMetaTableCountResolverFromPointer>(game, dicon);
		pimpl = std::make_unique<TagTableRangeImpl<GameState::Value::Halo3>>(game, dicon, [](uint32_t& in) { return (uintptr_t)in << 2;  }, tagTableAddressResolver, tagTableCountResolver, tagMagicResolver);
	}
	break;

	case GameState::Value::HaloReach:
	{
		auto tagMagicResolver = std::make_shared<TagTypeResolveFromMagicTable>(game, dicon);
		auto tagTableAddressResolver = std::make_shared < TagMetaTableAddressResolverFromPointer>(game, dicon);
		auto tagTableCountResolver = std::make_shared < TagMetaTableCountResolverFromPointer>(game, dicon);
		pimpl = std::make_unique<TagTableRangeImpl<GameState::Value::HaloReach>>(game, dicon, [](uint32_t& in) { return (uintptr_t)in << 2;  }, tagTableAddressResolver, tagTableCountResolver, tagMagicResolver);
	}
	break;

	case GameState::Value::Halo4:
	{
		auto tagMagicResolver = std::make_shared<TagTypeResolveFromMagicTable>(game, dicon);
		auto tagTableAddressResolver = std::make_shared < TagMetaTableAddressResolverFromPointer>(game, dicon);
		auto tagTableCountResolver = std::make_shared < TagMetaTableCountResolverFromPointer>(game, dicon);
		pimpl = std::make_unique<TagTableRangeImpl<GameState::Value::Halo4>>(game, dicon, [](uint32_t& in) { return (uintptr_t)in << 2;  }, tagTableAddressResolver, tagTableCountResolver, tagMagicResolver);
	}
	break;

	default: throw HCMInitException("tagBlockReader not impl yet");
	}
}

TagTableRange::~TagTableRange() = default;
