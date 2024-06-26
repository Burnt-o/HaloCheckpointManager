#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"


 // only impl for h2 so far, because it's not relavent for h1, and third gen (h3+) have a waaaay too complicated system for resolving debug strings
class GetDebugString : public IOptionalCheat
{
private:
	class GetDebugStringImpl;
	std::unique_ptr<GetDebugStringImpl> pimpl;

public:
	GetDebugString(GameState game, IDIContainer& dicon);
	~GetDebugString();

	std::expected<std::string, HCMRuntimeException> getDebugString(uintptr_t pStringID);

	virtual std::string_view getName() override { return nameof(GetDebugString); }
};

