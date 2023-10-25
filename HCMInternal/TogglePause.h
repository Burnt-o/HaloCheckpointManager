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

class TogglePause : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback <ToggleEvent>mTogglePauseCallbackHandle;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHook;
	std::weak_ptr<IMessagesGUI> messagesGUI;
	std::weak_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<PauseGame> pauseGameService; // actual implementation is over here

	// data
	std::unique_ptr<ScopedServiceRequest> pauseRequest;





public:
	TogglePause(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl),
		mTogglePauseCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->togglePause->valueChangedEvent, [this](bool& newValue) { onTogglePauseChanged(newValue); }),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		messagesGUI(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto control = dicon.Resolve<ControlServiceContainer>().lock();
		if (control->pauseGameServiceFailure.has_value()) throw control->pauseGameServiceFailure.value();
		if (control->pauseGameService.value()->getServiceFailures().contains(gameImpl)) throw control->pauseGameService.value()->getServiceFailures().at(gameImpl);
		pauseGameService = control->pauseGameService.value();
	}

	~TogglePause()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	std::string_view getName() override { return nameof(TogglePause); }

	// primary event callback
	// public so AdvanceTicks can access
	void onTogglePauseChanged(bool& newValue)
	{
		//if (!mccStateHook.lock()->isGameCurrentlyPlaying(mGame))
		//{
		//	if (pauseRequest)
		//		pauseRequest.reset();
		//	return;
		//}


		if (newValue)
		{
			pauseRequest = pauseGameService.lock()->scopedRequest(mGame.toString() + nameof(TogglePause));
		}
		else
		{
			if (pauseRequest)
				pauseRequest.reset();
		}
	}

};