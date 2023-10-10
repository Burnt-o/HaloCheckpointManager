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

class AIFreeze : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback <ToggleEvent> mAIFreezeCallbackHandle;

	// injected services
	gsl::not_null<std::shared_ptr<IMCCStateHook>> mccStateHook;
	gsl::not_null<std::shared_ptr<IMessagesGUI>> messagesGUI;
	gsl::not_null<std::shared_ptr<RuntimeExceptionHandler>> runtimeExceptions;

	//data
	std::shared_ptr<ModuleMidHook> aiFreezeHook;

	enum class flagToModifyToWhat {
		zeroFlagToFalse,
		zeroFlagToTrue
	};


	// hm.. could make some midhookcontextinterpreter style thing that can get this kinda flagToModifyToWhat from pointer data for different versions etc and we eval once here instead of every func call.
	template <flagToModifyToWhat flagModication>
	static void aiFreezeHookFunction(SafetyHookContext& ctx);

	template<>
	static void aiFreezeHookFunction<flagToModifyToWhat::zeroFlagToFalse>(SafetyHookContext& ctx) { ctx.rflags &= ~(1UL << 6); }

	template<>
	static void aiFreezeHookFunction<flagToModifyToWhat::zeroFlagToTrue>(SafetyHookContext& ctx) { ctx.rflags |= (1 << 6); }

	// primary event callback
	void onToggleChange(bool& newValue)
	{
		aiFreezeHook->setWantsToBeAttached(newValue);
		PLOG_DEBUG << "onToggleChange: newval: " << newValue;
		if (mccStateHook->isGameCurrentlyPlaying(mGame))
		{
			messagesGUI->addMessage(newValue ? "AI frozen." : "AI unfrozen.");
		}

	}



public:
	AIFreeze(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl),
		mAIFreezeCallbackHandle(dicon.Resolve<SettingsStateAndEvents>()->aiFreezeToggle->valueChangedEvent, [this](bool& n) { onToggleChange(n); }),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		messagesGUI(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto aiFreezeFunction = dicon.Resolve<PointerManager>()->getData<std::shared_ptr<MultilevelPointer>>(nameof(aiFreezeFunction), mGame);
		switch (mGame)
		{
		case GameState::Value::Halo1:
			
			aiFreezeHook = ModuleMidHook::make(gameImpl.toModuleName(), aiFreezeFunction, aiFreezeHookFunction<flagToModifyToWhat::zeroFlagToFalse>);
			break;

		case GameState::Value::Halo2:
			aiFreezeHook = ModuleMidHook::make(gameImpl.toModuleName(), aiFreezeFunction, aiFreezeHookFunction<flagToModifyToWhat::zeroFlagToTrue>);
			break;

		default:
			throw HCMInitException("not impl");
		}
	}

	std::string_view getName() override { return nameof(AIFreeze); }

};