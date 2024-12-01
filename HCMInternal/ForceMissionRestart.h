#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
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
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	//data
	std::shared_ptr<MultilevelPointer> forceMissionRestartFlag;

	// primary event callback
	void onForceMissionRestart()
	{

		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			lockOrThrow(messagesGUIWeak, messagesGUI);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;
			PLOG_DEBUG << "Force MissionRestart called";

			byte enableFlag = 1;
			if (!forceMissionRestartFlag->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write MissionRestart flag {}", MultilevelPointer::GetLastError()));
			messagesGUI->addMessage("Mission restart forced.");
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}


public:

	ForceMissionRestart(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl),
		mForceMissionRestartCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->forceMissionRestartEvent, [this]() {onForceMissionRestart(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		forceMissionRestartFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(forceMissionRestartFlag), mGame);
	}

	~ForceMissionRestart()
	{
		PLOG_VERBOSE << "~" << getName();
	}

	virtual std::string_view getName() override { return nameof(ForceMissionRestart); }

};