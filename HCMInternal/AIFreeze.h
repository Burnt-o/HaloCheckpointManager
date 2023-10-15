#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"


class AIFreezeImplUntemplated { public: virtual ~AIFreezeImplUntemplated() = default; };
class AIFreeze : public IOptionalCheat
{
private:

	std::unique_ptr<AIFreezeImplUntemplated> pimpl;

public:
	AIFreeze(GameState gameImpl, IDIContainer& dicon);
	~AIFreeze();

	std::string_view getName() override { return nameof(AIFreeze); }

};