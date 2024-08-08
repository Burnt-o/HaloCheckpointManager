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
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<SettingsStateAndEvents> mSettingsWeak;
	std::weak_ptr<BlockGameInput> blockInputServiceWeak; // actual implementation is over here

	// data
	std::shared_ptr<SharedRequestToken> blockInputRequest;

	// primary event callback
	void onTogglePauseChanged(bool& newValue)
	{
		try
		{
			lockOrThrow(mSettingsWeak, mSettings);


			if (newValue && mSettings->pauseAlsoBlocksInput->GetValue())
			{
				lockOrThrow(blockInputServiceWeak, blockInputService);
				blockInputRequest = blockInputService->makeScopedRequest();
			}
			else
			{
				if (blockInputRequest)
					blockInputRequest.reset();
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}



public:
	ToggleBlockInput(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl),
		mTogglePauseCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->togglePause->valueChangedEvent, [this](bool& newValue) { onTogglePauseChanged(newValue); }),
		mSettingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())

	{
		auto control = dicon.Resolve<ControlServiceContainer>().lock();
		if (control->blockGameInputServiceFailure.has_value()) throw control->blockGameInputServiceFailure.value();
		blockInputServiceWeak = control->blockGameInputService.value();
	}

	~ToggleBlockInput()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	std::string_view getName() override { return nameof(ToggleBlockInput); }

};