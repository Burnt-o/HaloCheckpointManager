#include "pch.h"
#include "ForceCheckpoint.h"
#include "GameStateHook.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MessagesGUI.h"
#include "OptionsState.h"
class ForceCheckpoint::ForceCheckpointImpl
{
private:
	GameState mGame;

	// event callbacks
	eventpp::ScopedRemover<ActionEvent> mForceCheckpointCallbackHandle;

	// injected services
	std::shared_ptr<GameStateHook> gameStateHook;
	std::shared_ptr<MessagesGUI> messagesGUI;

	void onForceCheckpoint()
	{
		if (gameStateHook.get()->getCurrentGameState() != mGame) return;

		PLOG_DEBUG << "Force checkpoint called";
		try
		{
			byte enableFlag = 1;
			if (!forceCheckpointFlag.get()->writeData(&enableFlag)) throw HCMRuntimeException(std::format("Failed to write checkpoint flag {}", MultilevelPointer::GetLastError()));
			messagesGUI.get()->addMessage("Checkpoint forced.");
		}
		catch (HCMRuntimeException ex)
		{
			RuntimeExceptionHandler::handleMessage(ex);
		}

	}

	//data
	std::shared_ptr<MultilevelPointer> forceCheckpointFlag;

public:

	ForceCheckpointImpl(GameState gameImpl, IDIContainer dicon)
		: mGame(gameImpl), gameStateHook(dicon.Resolve<GameStateHook>()), messagesGUI(dicon.Resolve<MessagesGUI>())
	{
		mForceCheckpointCallbackHandle.append(OptionsState::forceCheckpointEvent.get()->append([this]() {onForceCheckpoint(); }));
		auto ptr = dicon.Resolve<PointerManager>();
		forceCheckpointFlag = ptr.get()->getData<std::shared_ptr<MultilevelPointer>>("forceCheckpointFlag", mGame);
	}


};

ForceCheckpoint::ForceCheckpoint(GameState game, IDIContainer dicon) : impl(std::make_unique<ForceCheckpointImpl>(game, dicon)) {}
ForceCheckpoint::~ForceCheckpoint() = default;
