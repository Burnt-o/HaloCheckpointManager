#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

// actually switches zone set in case of h3+

class ISwitchBSPImpl { public: virtual ~ISwitchBSPImpl() = default; };
class SwitchBSP : public IOptionalCheat
{
private:

	std::unique_ptr<ISwitchBSPImpl> pimpl;

public:
	SwitchBSP(GameState gameImpl, IDIContainer& dicon);
	~SwitchBSP();

	std::string_view getName() override { return nameof(SwitchBSP); }

};