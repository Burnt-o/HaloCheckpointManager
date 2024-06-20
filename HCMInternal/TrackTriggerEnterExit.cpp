#include "pch.h"
#include "TrackTriggerEnterExit.h"
#include "GameTickEventHook.h"
#include "GetPlayerTriggerPosition.h"
#include "GetTriggerData.h"
#include "IMessagesGUI.h"
#include "IMakeOrGetCheat.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "GameTickEventHook.h"
#include "IMCCStateHook.h"

template <GameState::Value gameT>
class TrackTriggerEnterExitImpl : public TrackTriggerEnterExit::ITrackTriggerEnterExit
{
private:

	// injected services
	std::weak_ptr< GetPlayerTriggerPosition> getPlayerTriggerPositionWeak;
	std::weak_ptr< IMessagesGUI> messageGUIWeak;
	std::weak_ptr< GetTriggerData> getTriggerDataWeak;
	std::weak_ptr< SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<GameTickEventHook> gameTickEventHookWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;

	// callbacks
	std::unique_ptr<ScopedCallback<eventpp::CallbackList<void(uint32_t)>>> mGameTickEventCallback;

	ScopedCallback<ToggleEvent> triggerOverlayToggleCallback;
	ScopedCallback<ToggleEvent> triggerOverlayMessageOnEnterCallback;
	ScopedCallback<ToggleEvent> triggerOverlayMessageOnExitCallback;
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	void onSettingChanged()
	{

		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);

			lockOrThrow(settingsWeak, settings);

			bool shouldEnableHook = mccStateHook->isGameCurrentlyPlaying((GameState)gameT) &&
				settings->triggerOverlayToggle->GetValue() &&
				(settings->triggerOverlayMessageOnEnter->GetValue() || settings->triggerOverlayMessageOnExit->GetValue());

			PLOG_DEBUG << "TrackTriggerEnterExitImpl setting to state: " << (shouldEnableHook ? "true" : "false");

			if (shouldEnableHook)
			{
				lockOrThrow(gameTickEventHookWeak, gameTickEventHook);
				mGameTickEventCallback = std::make_unique<ScopedCallback<eventpp::CallbackList<void(uint32_t)>>>(gameTickEventHook->getGameTickEvent(), [this](uint32_t i) {onGameTickEvent(i); });
			}
			else
			{
				// TODO: destruction guard
				mGameTickEventCallback.reset();
			}


		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onGameTickEvent(uint32_t& currentTick)
	{
		static uint32_t lastTick = 0;
		if (lastTick > currentTick) // revert detection
		{
			lastTick = currentTick;
			return; // don't process triggers on revert
		}
		lastTick = currentTick;

		try
		{
			lockOrThrow(getTriggerDataWeak, getTriggerData);
			lockOrThrow(getPlayerTriggerPositionWeak, getPlayerTriggerPosition);
			lockOrThrow(messageGUIWeak, messageGUI);
			lockOrThrow(settingsWeak, settings);

			bool shouldPrintForEnter = settings->triggerOverlayMessageOnEnter->GetValue();
			bool shouldPrintForExit = settings->triggerOverlayMessageOnExit->GetValue();

			auto& playerTriggerPosition = getPlayerTriggerPosition->getPlayerTriggerPosition();
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "checking triggers against player trigger vertex at: " << p, p = playerTriggerPosition);

			auto filteredTriggerData = getTriggerData->getFilteredTriggers();

			for (auto& [triggerPointer, triggerData] : *filteredTriggerData.get())
			{
				bool playerIsInsideThisTick = triggerData.model.box.Contains(playerTriggerPosition) == DirectX::ContainmentType::CONTAINS;

				if (playerIsInsideThisTick == triggerData.playerWasInsideLastTick)
					continue;

				bool playerNewlyEntered = playerIsInsideThisTick == true && triggerData.playerWasInsideLastTick == false;
				bool playerNewlyExited = playerIsInsideThisTick == false && triggerData.playerWasInsideLastTick == true;

				triggerData.playerWasInsideLastTick = playerIsInsideThisTick; // update trigger data

				// print message if applicable setting
				if (shouldPrintForEnter && playerNewlyEntered)
				{
					messageGUI->addMessage(std::format("Entered trigger {} on tick {}", triggerData.name, currentTick));
				}

				if (shouldPrintForExit && playerNewlyExited)
				{
					messageGUI->addMessage(std::format("Exited trigger {} on tick {}", triggerData.name, currentTick));
				}

			}


		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}


	}

public:

	TrackTriggerEnterExitImpl(GameState game, IDIContainer& dicon)
		: getPlayerTriggerPositionWeak(resolveDependentCheat(GetPlayerTriggerPosition)),
		getTriggerDataWeak(resolveDependentCheat(GetTriggerData)),
		messageGUIWeak(dicon.Resolve<IMessagesGUI>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		gameTickEventHookWeak(resolveDependentCheat(GameTickEventHook)),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		triggerOverlayToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayToggle->valueChangedEvent, [this](bool& n) { onSettingChanged(); }),
		triggerOverlayMessageOnEnterCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayMessageOnEnter->valueChangedEvent, [this](bool& n) { onSettingChanged(); }),
		triggerOverlayMessageOnExitCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayMessageOnExit->valueChangedEvent, [this](bool& n) { onSettingChanged(); }),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) {onSettingChanged(); })
	{
		PLOG_DEBUG << "TrackTriggerEnterExitImpl con";
	}
};


TrackTriggerEnterExit::TrackTriggerEnterExit(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<TrackTriggerEnterExitImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<TrackTriggerEnterExitImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<TrackTriggerEnterExitImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<TrackTriggerEnterExitImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<TrackTriggerEnterExitImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<TrackTriggerEnterExitImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}


}