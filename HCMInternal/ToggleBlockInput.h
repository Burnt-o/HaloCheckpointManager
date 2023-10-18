#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "ControlServiceContainer.h"

class ToggleBlockInput : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback <ToggleEvent>mTogglePauseCallbackHandle;

	// injected services
	gsl::not_null<std::shared_ptr<IMCCStateHook>> mccStateHook;
	gsl::not_null<std::shared_ptr<IMessagesGUI>> messagesGUI;
	gsl::not_null<std::shared_ptr<RuntimeExceptionHandler>> runtimeExceptions;
	gsl::not_null<std::shared_ptr<SettingsStateAndEvents>> mSettings;
	std::shared_ptr<BlockGameInput> blockInputService; // actual implementation is over here

	// data
	std::unique_ptr<ScopedServiceRequest> blockInputRequest;

	// primary event callback
	void onTogglePauseChanged(bool& newValue)
	{
		if (newValue && mSettings->pauseAlsoBlocksInput->GetValue())
		{
			blockInputRequest = blockInputService->scopedRequest(mGame.toString() + nameof(ToggleBlockInput));
		}
		else
		{
			if (blockInputRequest)
				blockInputRequest.reset();
		}
	}



public:
	ToggleBlockInput(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl),
		mTogglePauseCallbackHandle(dicon.Resolve<SettingsStateAndEvents>()->togglePause->valueChangedEvent, [this](bool& newValue) { onTogglePauseChanged(newValue); }),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		messagesGUI(dicon.Resolve<IMessagesGUI>()),
		mSettings(dicon.Resolve<SettingsStateAndEvents>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto control = dicon.Resolve<ControlServiceContainer>();
		if (control->blockGameInputServiceFailure.has_value()) throw control->blockGameInputServiceFailure.value();

		blockInputService = control->blockGameInputService.value();
	}

	~ToggleBlockInput()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	std::string_view getName() override { return nameof(ToggleBlockInput); }

};