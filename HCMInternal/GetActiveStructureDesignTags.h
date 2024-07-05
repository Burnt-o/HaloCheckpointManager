#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "TagInfo.h"

class IGetActiveStructureDesignTags
{
public:
	virtual std::set<TagInfo> getActiveStructureDesignTags() = 0;
	virtual ~IGetActiveStructureDesignTags() = default;
};

class GetActiveStructureDesignTags : public IOptionalCheat
{
private:
	std::unique_ptr<IGetActiveStructureDesignTags> pimpl;

public:
	GetActiveStructureDesignTags(GameState game, IDIContainer& dicon);
	~GetActiveStructureDesignTags();

	std::set<TagInfo> getActiveStructureDesignTags() { return pimpl->getActiveStructureDesignTags(); }

	virtual std::string_view getName() override { return nameof(GetActiveStructureDesignTags); }
};