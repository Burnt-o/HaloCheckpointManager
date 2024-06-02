#include "pch.h"
#include "ForceFutureCheckpoint.h"
#include "IMessagesGUI.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "IMCCStateHook.h"
#include "GameTickEventHook.h"
#include "IMakeOrGetCheat.h"

class ForceFutureCheckpoint::ForceFutureCheckpointImpl 
{
private:
	GameState mGame;
	// injected services
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<GameTickEventHook> gameTickEventHookWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;

	// event callbacks
	ScopedCallback <ToggleEvent> forceFutureCheckpointToggleCallback;
	void onForceFutureCheckpointToggle(bool& newValue)
	{
		try
		{
			lockOrThrow(gameTickEventHookWeak, gameTickEventHook);

			if (newValue)
			{
				gameTickEventCallback = std::make_unique<ScopedCallback<eventpp::CallbackList<void(uint32_t)>>>(gameTickEventHook->getGameTickEvent(), [this](uint32_t i) {onGameTickEventCallback(i); });
			}
			else
			{
				gameTickEventCallback.reset();
			}

		}
		catch (HCMRuntimeException ex)
		{
			ex.prepend("Force Future Checkpoint service encountered an error: ");
			runtimeExceptions->handleMessage(ex);
		}
	}

	// runs every tick while armed, if current tick is the user-selected tick then disarm the service and force a checkpoint. 
	std::unique_ptr<ScopedCallback<eventpp::CallbackList<void(uint32_t)>>> gameTickEventCallback;
	void onGameTickEventCallback(uint32_t tickCount)
	{
		try
		{
			lockOrThrow(settingsWeak, settings)
			if (tickCount == settings->forceFutureCheckpointTick->GetValue())
			{ // current tick is the tick to fire the checkpoint!
				
				// begin by disabling the toggle setting (which will disable this tick event callback)
				settings->forceFutureCheckpointToggle->GetValueDisplay() = false;
				settings->forceFutureCheckpointToggle->UpdateValueWithInput();

				// force the checkpoint (fire the event; if it fails, not our problem)
				settings->forceCheckpointEvent->operator()();
			}
		}
		catch (HCMRuntimeException ex)
		{
			ex.prepend("Force Future Checkpoint service encountered an error: ");
			runtimeExceptions->handleMessage(ex);
		}
	}


	// fill the which-tick-to-force-checkpoint-on with a value 5 seconds from the current game tick.
	ScopedCallback <ActionEvent> forceFutureCheckpointFillCallback;
	void onForceFutureCheckpointFill()
	{
		PLOG_VERBOSE << "onForceFutureCheckpointFill";
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;


			lockOrThrow(gameTickEventHookWeak, gameTickEventHook);
			int currentTick = gameTickEventHook->getCurrentGameTick();

			int ticksPerSecond = (mGame.operator GameState::Value() == GameState::Value::Halo1) ? 30 : 60;
			constexpr int secondsInTheFuture = 5;

			int futureTick = currentTick + (ticksPerSecond * secondsInTheFuture);

			lockOrThrow(settingsWeak, settings);
			settings->forceFutureCheckpointTick->GetValueDisplay() = futureTick;
			settings->forceFutureCheckpointTick->UpdateValueWithInput();
		}
		catch (HCMRuntimeException ex)
		{
			ex.prepend("Force Future Checkpoint service encountered an error: ");
			runtimeExceptions->handleMessage(ex);
		}

	}




public:
	ForceFutureCheckpointImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		forceFutureCheckpointToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->forceFutureCheckpointToggle->valueChangedEvent, [this](bool& n) { onForceFutureCheckpointToggle(n); }),
		forceFutureCheckpointFillCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->forceFutureCheckpointFillEvent, [this]() { onForceFutureCheckpointFill(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		gameTickEventHookWeak(resolveDependentCheat(GameTickEventHook))
	{

	}

};




ForceFutureCheckpoint::ForceFutureCheckpoint(GameState gameImpl, IDIContainer& dicon)
	: pimpl(std::make_unique<ForceFutureCheckpointImpl>(gameImpl, dicon))
{
}

ForceFutureCheckpoint::~ForceFutureCheckpoint() = default;