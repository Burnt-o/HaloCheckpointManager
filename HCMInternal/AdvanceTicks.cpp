#pragma once
#include "pch.h"
#include "AdvanceTicks.h"
#include "GameState.h"
#include "DIContainer.h"

#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "TogglePause.h"
#include "PointerManager.h"
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
	std::weak_ptr<IMessagesGUI> messagesGUI;
	std::weak_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<PauseGame> pauseService;
	std::weak_ptr<GameTickEventHook> gameTickEventHook;
	std::weak_ptr<SettingsStateAndEvents> settings;
	std::weak_ptr<IMCCStateHook> mccStateHook;


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
		if (!mccStateHook.lock()->isGameCurrentlyPlaying(mGame))
		{
			mGameTickEventCallback.reset();
			pauseOverrideRequest.reset();
			return;
		}

		auto gameTickEvent = gameTickEventHook.lock();
		if (!gameTickEvent) { messagesGUI.lock()->addMessage("Advance ticks failed: bad gameTickEventHook weak ptr."); return; }

		// need to destroy old ones BEFORE constructing new ones
		mGameTickEventCallback.reset();
		pauseOverrideRequest.reset();

		// something is going wrong here. on any call but the first the mGameTIckEventCallback binding fails, somehow
		// why? the event is absolutely firing. Why is the binding failing?
		// I mean I could fix this by only binding once but I really don't want to have to do that
		// it looks like the scoped call back is just instantly auto-destructing

		advanceTicksCount = settings.lock()->advanceTicksCount->GetValue();
		mGameTickEventCallback = std::make_unique<ScopedCallback<eventpp::CallbackList<void(int)>>>(gameTickEvent->getGameTickEvent(), [this](int i) {onGameTickEvent(i); });
		pauseOverrideRequest = pauseService.lock()->scopedOverrideRequest(nameof(AdvanceTicks::AdvanceTicksImpl));


		messagesGUI.lock()->addMessage(std::format("Advancing {} tick{}.", advanceTicksCount, advanceTicksCount == 1 ? "" : "s"));
	}
public:
	AdvanceTicksImpl(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl),
		mAdvanceTicksCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->advanceTicksEvent, [this]() { onAdvanceTicksEvent(); }),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		messagesGUI(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settings(dicon.Resolve<SettingsStateAndEvents>())
	{
		auto csc = dicon.Resolve<ControlServiceContainer>().lock();

		if (!csc->pauseGameService.has_value()) throw HCMInitException("Cannot advance ticks without pause service");
		pauseService = csc->pauseGameService.value();

		gameTickEventHook = std::dynamic_pointer_cast<GameTickEventHook>(dicon.Resolve<IMakeOrGetCheat>().lock()->getOrMakeCheat(std::make_pair(gameImpl, OptionalCheatEnum::GameTickEventHook), dicon));
		

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

