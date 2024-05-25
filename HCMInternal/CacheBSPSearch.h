#pragma once
#include "pch.h"
#include "DIContainer.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "BumpControlDefs.h"

class CacheBSPSearch : public IOptionalCheat
{
private:
#ifdef CACHE_BSPS
	class CacheBSPSearchImpl;
	std::unique_ptr<CacheBSPSearchImpl> pimpl;
#endif

public:
	CacheBSPSearch(GameState gameImpl, IDIContainer& dicon);

	~CacheBSPSearch();
	std::string_view getName() override { return nameof(CacheBSPSearch); }



};