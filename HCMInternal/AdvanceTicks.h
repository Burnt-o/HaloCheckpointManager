#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "MultilevelPointer.h"
#include "ControlServiceContainer.h"
#include "ModuleHook.h"
#include "IMCCStateHook.h"

class AdvanceTicksImpl;
class AdvanceTicks : public IOptionalCheat
{
private:
	std::unique_ptr<AdvanceTicksImpl> pimpl; 


public:
	AdvanceTicks(GameState gameImpl, IDIContainer& dicon);

	~AdvanceTicks();

	std::string_view getName() override { return nameof(AdvanceTicks); }



};