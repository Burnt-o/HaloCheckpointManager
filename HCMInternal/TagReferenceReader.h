#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "TagInfo.h"


class TagReferenceReader : public IOptionalCheat
{

public:
	class ITagReferenceReader
	{
	public:
		virtual std::expected<TagInfo, HCMRuntimeException> read(uintptr_t tagReference) = 0;
		virtual ~ITagReferenceReader() = default;
	};



	TagReferenceReader(GameState gameImpl, IDIContainer& dicon);
	~TagReferenceReader();

	std::expected<TagInfo, HCMRuntimeException> read(uintptr_t tagReference) { return pimpl->read(tagReference); }

	std::string_view getName() override { return nameof(GetAggroData); }

private:
	std::unique_ptr<ITagReferenceReader> pimpl;
};