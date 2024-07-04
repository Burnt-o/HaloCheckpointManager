#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "TagInfo.h"


class ITagTableRangeImpl {
public:
	virtual ~ITagTableRangeImpl() = default;
	virtual std::expected<std::vector<TagInfo>, HCMRuntimeException> getTagTableRange() = 0;
	virtual std::expected<TagInfo, HCMRuntimeException> getTagByDatum(Datum tagDatum) = 0;
	virtual std::expected<TagInfo, HCMRuntimeException> getTagByIndex(uint16_t tagDatumIndex) = 0;
};

class TagTableRange : public IOptionalCheat
{
private:
	std::unique_ptr<ITagTableRangeImpl> pimpl;

public:
	TagTableRange(GameState game, IDIContainer& dicon);
	~TagTableRange();


	std::expected<std::vector<TagInfo>, HCMRuntimeException> getTagTableRange() { return pimpl->getTagTableRange(); }

	// confirms salt matches
	std::expected<TagInfo, HCMRuntimeException> getTagByDatum(Datum tagDatum) { return pimpl->getTagByDatum(tagDatum); }

	// does not check salt
	std::expected<TagInfo, HCMRuntimeException> getTagByIndex(uint16_t tagDatumIndex) { return pimpl->getTagByIndex(tagDatumIndex); }

	virtual std::string_view getName() override { return nameof(TagTableRange); }
};
