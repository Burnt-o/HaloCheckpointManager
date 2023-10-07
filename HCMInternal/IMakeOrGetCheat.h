#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "OptionalCheatEnum.h"
class IMakeOrGetCheat
{
public:
	virtual std::shared_ptr< IOptionalCheat> getOrMakeCheat(const std::pair<GameState, OptionalCheatEnum>& gameCheatPair, IDIContainer& dicon) = 0;
};


#define resolveDependentCheat(x) std::dynamic_pointer_cast<x>(dicon.Resolve<IMakeOrGetCheat>()->getOrMakeCheat({game, OptionalCheatEnum::x}, dicon))