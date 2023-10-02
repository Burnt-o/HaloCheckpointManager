#include "pch.h"
#include "ForceCoreLoad.h"
#include "GameStateHook.h"
#include "MessagesGUI.h"
#include "OptionsState.h"
class ForceCoreLoadImpl
{
private:
	GameState mGame;

	eventpp::ScopedRemover < eventpp::CallbackList<void()>> mForceCoreLoadCallbackHandle;

	// injected services
	std::shared_ptr<GameStateHook> gameStateHook;
	std::shared_ptr<MessagesGUI> messagesGUI;

	void onForceCoreLoad()
	{
		if (gameStateHook.get()->getCurrentGameState() != mGame) return;

		PLOG_DEBUG << "Force CoreLoad called";
		try
		{
			byte enableFlag = 1;
			if (!forceCoreLoadFlag.get()->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write CoreLoad flag {}", MultilevelPointer::GetLastError()));
			messagesGUI.get()->addMessage("CoreLoad forced.");
		}
		catch (HCMRuntimeException ex)
		{
			RuntimeExceptionHandler::handleMessage(ex);
		}

	}

	//data
	std::shared_ptr<MultilevelPointer> forceCoreLoadFlag;

public:

	ForceCoreLoadImpl(GameState gameImpl, IDIContainer dicon)
		: mGame(gameImpl), gameStateHook(dicon.Resolve<GameStateHook>()), messagesGUI(dicon.Resolve<MessagesGUI>())
	{
		mForceCoreLoadCallbackHandle.append(OptionsState::forceCoreLoadEvent.get()->append([this]() {onForceCoreLoad(); }));
		auto ptr = dicon.Resolve<PointerManager>();
		forceCoreLoadFlag = ptr.get()->getData<std::shared_ptr<MultilevelPointer>>("forceCoreLoadFlag", mGame);
	}



};