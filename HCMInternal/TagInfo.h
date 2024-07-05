#pragma once
#include "MagicString.h"
#include "Datum.h"


struct TagInfo
{
	MagicString tagType;
	Datum tagDatum;
	uintptr_t tagAddress;
	explicit TagInfo(MagicString tType, Datum tDatum, uintptr_t tAddress) : tagType(tType), tagDatum(tDatum), tagAddress(tAddress) {}

	bool operator< (const TagInfo& other) const
	{
		return tagDatum.index < other.tagDatum.index;
	}
};