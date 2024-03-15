#pragma once
#include "pch.h"
#include "AdvanceTicks.h"
#include "GameState.h"
#include "DIContainer.h"

#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "TogglePause.h"
#include "PointerDataStore.h"
#include "GameTickEventHook.h"
#include "IMakeOrGetCheat.h"
// 2 valid options for implementation:
// Either we inline hook the games doGameTick function and just call it manually x times,
// or we temporarily tell the togglePause cheat to unpause until x ticks have passed (hooking the tickIncrement func to count).

// The latter has potentially to be buggy, but is easier to RE the pointerData for.
// This is an implementation of the latter. Had to think long and hard about how to not make it buggy, so I added an scopedOverrideRequest thing to pauseGame service to do the hard work.


class AdvanceTicksImpl
{
private:
	GameState mGame;

	// event callbacks
	ScopedCallback <ActionEvent> mAdvanceTicksCallbackHandle;
	std::unique_ptr<ScopedCallback<eventpp::CallbackList<void(int)>>> mGameTickEventCallback;


	// injected services
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<PauseGame> pauseServiceWeak;
	std::weak_ptr<GameTickEventHook> gameTickEventHookWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;


	int advanceTicksCount = 0;

	void onGameTickEvent(int tickCount)
	{
		if (advanceTicksCount > 1) // since "0" will cause a hit
		{
			PLOG_VERBOSE << "tickIncrementHookFunction hit";
			advanceTicksCount = advanceTicksCount - 1; // decrement our counter by one
		}
		else  // we have advanced enough ticks!
		{
			PLOG_VERBOSE << "advance ticks ending pause override";
			pauseOverrideRequest.reset(); // discard the pauseOverrideRequest, which will repause the game.
			mGameTickEventCallback.reset(); // discard the onGameTickEvent callback
		}
	}



	std::unique_ptr<ScopedServiceRequest> pauseOverrideRequest; 


	// primary event callback
	void onAdvanceTicksEvent()
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			lockOrThrow(gameTickEventHookWeak, gameTickEventHook);
			lockOrThrow(settingsWeak, settings);
			lockOrThrow(pauseServiceWeak, pauseService);
			lockOrThrow(messagesGUIWeak, messagesGUI);

			if (!mccStateHook->isGameCurrentlyPlaying(mGame))
			{
				mGameTickEventCallback.reset();
				pauseOverrideRequest.reset();
				return;
			}

			// need to destroy old ones BEFORE constructing new ones
			mGameTickEventCallback.reset();
			pauseOverrideRequest.reset();

			// something is going wrong here. on any call but the first the mGameTIckEventCallback binding fails, somehow
			// why? the event is absolutely firing. Why is the binding failing?
			// I mean I could fix this by only binding once but I really don't want to have to do that
			// it looks like the scoped call back is just instantly auto-destructing

			advanceTicksCount = settings->advanceTicksCount->GetValue();
			mGameTickEventCallback = std::make_unique<ScopedCallback<eventpp::CallbackList<void(int)>>>(gameTickEventHook->getGameTickEvent(), [this](int i) {onGameTickEvent(i); });
			pauseOverrideRequest = pauseService->scopedOverrideRequest(nameof(AdvanceTicks::AdvanceTicksImpl));


			messagesGUI->addMessage(std::format("Advancing {} tick{}.", advanceTicksCount, advanceTicksCount == 1 ? "" : "s"));
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

		
	}
public:
	AdvanceTicksImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		mAdvanceTicksCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->advanceTicksEvent, [this]() { onAdvanceTicksEvent(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		gameTickEventHookWeak(resolveDependentCheat(GameTickEventHook))
	{
		auto csc = dicon.Resolve<ControlServiceContainer>().lock();

		if (!csc->pauseGameService.has_value()) throw HCMInitException("Cannot advance ticks without pause service");
		pauseServiceWeak = csc->pauseGameService.value();


	}



};


AdvanceTicks::AdvanceTicks(GameState gameImpl, IDIContainer& dicon)
	: pimpl(std::make_unique<AdvanceTicksImpl>(gameImpl, dicon)) // may throw eg if pauseGameService or GameTickEventHook isn't available
{
}

AdvanceTicks::~AdvanceTicks()
{
	PLOG_DEBUG << "~" << getName();
}

