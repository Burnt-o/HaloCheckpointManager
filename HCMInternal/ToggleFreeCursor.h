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

class ToggleFreeCursor : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	//ScopedCallback <ToggleEvent>mTogglePauseCallbackHandle;

	// injected services
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<SettingsStateAndEvents> mSettingsWeak;
	std::weak_ptr<FreeMCCCursor> freeCursorServiceWeak; // actual implementation is over here

	// data
	std::unique_ptr<ScopedServiceRequest> freeCursorRequest;

	// primary event callback
	void onTogglePauseChanged(bool& newValue)
	{
		try
		{
			lockOrThrow(mSettingsWeak, mSettings);


			if (newValue && mSettings->pauseAlsoFreesCursor->GetValue())
			{
				lockOrThrow(freeCursorServiceWeak, freeCursorService);
				freeCursorRequest = freeCursorService->scopedRequest(mGame.toString() + nameof(ToggleFreeCursor));
			}
			else
			{
				if (freeCursorRequest)
					freeCursorRequest.reset();
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}



public:
	ToggleFreeCursor(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl),
		//mTogglePauseCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->togglePause->valueChangedEvent, [this](bool& newValue) { onTogglePauseChanged(newValue); }),
		mSettingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		PLOG_DEBUG << "made it to ToggleFreeCursor constructor body, atleast";
		PLOG_DEBUG << "settingsStateAndEvents expired? " << dicon.Resolve<SettingsStateAndEvents>().expired();
		PLOG_DEBUG << "settingsStateAndEvents lock? " << dicon.Resolve<SettingsStateAndEvents>().lock();
		auto control = dicon.Resolve<ControlServiceContainer>().lock();
		if (control->freeMCCSCursorServiceFailure.has_value()) throw control->freeMCCSCursorServiceFailure.value();

		freeCursorServiceWeak = control->freeMCCSCursorService.value();
	}

	~ToggleFreeCursor()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	std::string_view getName() override { return nameof(ToggleFreeCursor); }

};