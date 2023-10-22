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

// 2 valid options for implementation:
// Either we inline hook the games doGameTick function and just call it manually x times,
// or we temporarily tell the togglePause cheat to unpause until x ticks have passed (hooking the tickIncrement func to count).

// The latter has potentially to be buggy, but is easier to RE the pointerData for.
// This is an implementation of the latter. Had to think long and hard about how to not make it buggy, so I added an scopedOverrideRequest thing to pauseGame service to do the hard work.


class AdvanceTicksImpl
{
private:
	static inline AdvanceTicksImpl* instance = nullptr;

	// event callbacks
	ScopedCallback <ActionEvent> mAdvanceTicksCallbackHandle;

	// injected services

	std::weak_ptr<IMessagesGUI> messagesGUI;
	std::weak_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<PauseGame> pauseService;
	std::weak_ptr<SettingsStateAndEvents> settings;

	int advanceTicksCount = 0;





	std::optional<std::unique_ptr<ScopedServiceRequest>> pauseOverrideRequest; 


	// primary event callback
	void onAdvanceTicksEvent()
	{
		advanceTicksCount = settings.lock()->advanceTicksCount->GetValue();
		pauseOverrideRequest = pauseService.lock()->scopedOverrideRequest(nameof(AdvanceTicks::AdvanceTicksImpl));
		messagesGUI.lock()->addMessage(std::format("Advancing {} tick{}.", advanceTicksCount, advanceTicksCount == 1 ? "" : "s"));
	}
public:
	AdvanceTicksImpl(GameState gameImpl, IDIContainer& dicon)
		: mAdvanceTicksCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->advanceTicksEvent, [this]() { onAdvanceTicksEvent(); }),

		messagesGUI(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settings(dicon.Resolve<SettingsStateAndEvents>())
	{
		auto csc = dicon.Resolve<ControlServiceContainer>().lock();

		if (!csc->pauseGameService.has_value()) throw HCMInitException("Cannot advance ticks without pause service");
		pauseService = csc->pauseGameService.value();

		if (instance) throw HCMInitException("cannot have more than one AdvanceTicksImpl");
		instance = this;

	}


	static void tickIncrementHookFunction(SafetyHookContext& ctx)
	{
		if (!instance) { PLOG_ERROR << "null AdvanceTicksImpl instance"; return; }
		if (instance->advanceTicksCount > 1) // since "0" will cause a hit
		{
			PLOG_DEBUG << "tickIncrementHookFunction hit";
			instance->advanceTicksCount = instance->advanceTicksCount - 1; // decrement our counter by one
		}
		else  // we have advanced enough ticks!
		{
			instance->pauseOverrideRequest = std::nullopt; // discard the pauseOverrideRequest, which will repause the game.
		}
	}
};


AdvanceTicks::AdvanceTicks(GameState gameImpl, IDIContainer& dicon)
	: mGame(gameImpl),
	mccStateHook(dicon.Resolve<IMCCStateHook>()),
	mAdvanceTicksCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->advanceTicksEvent, [this]() { onAdvanceTicksEvent(); })
{

	// may throw, if so we don't want to create pimpl if it's not needed
	auto tickIncrementFunction = dicon.Resolve<PointerManager>().lock()->getData<std::shared_ptr<MultilevelPointer>>(nameof(tickIncrementFunction), gameImpl);

	// only need one instance of pimpl
	if (!pimpl)
		pimpl = std::make_unique<AdvanceTicksImpl>(gameImpl, dicon); // may throw eg if pauseGameService isn't available

	// only assign to pimpls tickIncrementHookFunction now that pimpl is definitely constructed
	// hook state starts unattached, will be turned on the first time the advanceTicks event is fired;
	tickIncrementHook = ModuleMidHook::make(gameImpl.toModuleName(), tickIncrementFunction, pimpl->tickIncrementHookFunction);

}

AdvanceTicks::~AdvanceTicks()
{
	PLOG_DEBUG << "~" << getName();
}


std::unique_ptr<AdvanceTicksImpl> AdvanceTicks::pimpl = nullptr;
