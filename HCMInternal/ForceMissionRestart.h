#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "GameEngineFunctions.h"
class ForceMissionRestart : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> mForceMissionRestartCallbackHandle;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<GameEngineFunctions> gameEngineFunctionsWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;




	// primary event callback
	void onForceMissionRestart()
	{

		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);

			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;
			PLOG_DEBUG << "Force MissionRestart called";
			lockOrThrow(messagesGUIWeak, messagesGUI);
			lockOrThrow(gameEngineFunctionsWeak, gameEngineFunctions);
			
			messagesGUI->addMessage("Mission restart forced.");
			gameEngineFunctions->RestartLevel();

		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}


public:

	ForceMissionRestart(GameState game, IDIContainer& dicon)
		: mGame(game),
		mForceMissionRestartCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->forceMissionRestartEvent, [this]() {onForceMissionRestart(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		gameEngineFunctionsWeak(resolveDependentCheat(GameEngineFunctions))
	{

	
	}

	~ForceMissionRestart()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	virtual std::string_view getName() override { return nameof(ForceMissionRestart); }

};