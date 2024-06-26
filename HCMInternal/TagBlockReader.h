#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

struct ParsedTagBlock
{
	explicit ParsedTagBlock(uintptr_t p, size_t s) : firstElement(p), elementCount(s) {}
	uintptr_t firstElement;
	size_t elementCount;
};


class TagBlockReader : public IOptionalCheat
{

public:
	class ITagBlockReader
	{
	public:
		virtual std::expected<ParsedTagBlock, HCMRuntimeException> read(uintptr_t tagBlock) = 0;
		virtual ~ITagBlockReader() = default;
	};



	TagBlockReader(GameState gameImpl, IDIContainer& dicon);
	~TagBlockReader();

	std::expected<ParsedTagBlock, HCMRuntimeException> read(uintptr_t tagBlock) { return pimpl->read(tagBlock); }

	std::string_view getName() override { return nameof(GetAggroData); }

private:
	std::unique_ptr<ITagBlockReader> pimpl;
};