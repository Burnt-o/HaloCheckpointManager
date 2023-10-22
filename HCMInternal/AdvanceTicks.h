#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "MultilevelPointer.h"
#include "ControlServiceContainer.h"
#include "ModuleHook.h"

class AdvanceTicksImpl;
class AdvanceTicks : public IOptionalCheat
{
private:

	static std::unique_ptr<AdvanceTicksImpl> pimpl; // one shared instance of impl

	// hooks 
	std::shared_ptr<ModuleMidHook> tickIncrementHook; // seperate hook for each game, each gets bound to impls tickIncrementHookFunction
	//static void tickIncrementHookFunction(SafetyHookContext& ctx);


public:
	AdvanceTicks(GameState gameImpl, IDIContainer& dicon);

	~AdvanceTicks();

	std::string_view getName() override { return nameof(AdvanceTicks); }



};