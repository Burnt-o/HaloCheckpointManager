#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "Datum.h"
#include "MagicString.h"

struct TagInfo
{
	MagicString tagType;
	Datum tagDatum;
	uintptr_t tagAddress;
	explicit TagInfo(MagicString tType, Datum tDatum, uintptr_t tAddress) : tagType(tType), tagDatum(tDatum), tagAddress(tAddress) {}
};

class ITagTableRangeImpl {
public:
	virtual ~ITagTableRangeImpl() = default;
	virtual std::expected<std::vector<TagInfo>, HCMRuntimeException> getTagTableRange() = 0;
	virtual std::expected<TagInfo, HCMRuntimeException> getTagByDatum(Datum tagDatum) = 0;
};

class TagTableRange : public IOptionalCheat
{
private:
	std::unique_ptr<ITagTableRangeImpl> pimpl;

public:
	TagTableRange(GameState game, IDIContainer& dicon);
	~TagTableRange();


	std::expected<std::vector<TagInfo>, HCMRuntimeException> getTagTableRange() { return pimpl->getTagTableRange(); }
	std::expected<TagInfo, HCMRuntimeException> getTagByDatum(Datum tagDatum) { return pimpl->getTagByDatum(tagDatum); }

	virtual std::string_view getName() override { return nameof(TagTableRange); }
};
