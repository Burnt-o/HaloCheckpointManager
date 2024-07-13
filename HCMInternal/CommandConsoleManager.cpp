#include "pch.h"
#include "CommandConsoleManager.h"
#include "EngineCommand.h"

#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "ControlServiceContainer.h"
#include "PauseGame.h"
#include "BlockGameInput.h"
#include "FreeMCCCursor.h"
#include "IMakeOrGetCheat.h"
#include "CommandConsoleGUI.h"
#include "EngineCommandOutputEvent.h"

class CommandConsoleManager::CommandConsoleManagerImpl
{
private:
	typedef std::optional<std::unique_ptr<ScopedServiceRequest>> opScRqst;

	// callbacks
	ScopedCallback<ActionEvent> commandConsoleHotkeyEventCallback;
	ScopedCallback<ActionEvent> commandConsoleExecuteBufferEventCallback;
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;
	ScopedCallback< eventpp::CallbackList<void(float&)>> fontSizeChangedCallback;

	// events we'll subscribe to when console is open
	std::shared_ptr<RenderEvent> renderEvent;
	std::shared_ptr<ObservedEvent<EngineOutputEvent>> engineCommandOutputEvent;
	


	// injected services
	std::weak_ptr<GenericScopedServiceProvider> hotkeyDisablerWeak;
	std::optional<std::weak_ptr<PauseGame>> pauseGameOptionalWeak;
	std::optional<std::weak_ptr<BlockGameInput>> blockGameInputOptionalWeak;
	std::optional < std::weak_ptr<FreeMCCCursor>> freeCursorOptionalWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;



	struct ConsoleScopedRequests
	{
		std::unique_ptr<ScopedCallback<RenderEvent>> renderEventCallback; // render console on each new frame
		std::shared_ptr<ScopedCallback<EngineOutputEvent>> engineCommandOutputEventCallback; // log engine command output
		std::unique_ptr<ScopedServiceRequest> disableHotkeys; // disable hcm hotkeys
		opScRqst pauseGame; // pause the game (if user checked the setting for it)
		opScRqst blockInputs; // block game inputs (if user checked the setting for it)
		opScRqst freeCursor; // free the cursor (if user checked the setting for it)

	};

	// has value when console open, nullopt when console closed
	std::optional< ConsoleScopedRequests> consoleScopedRequests = std::nullopt;

	// data
	GameState mGame;
	std::unique_ptr< CommandConsoleGUI> commandConsole;





	void onCommandConsoleHotkeyEvent()
	{

		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false)
			{
				if (consoleScopedRequests)
				{
					PLOG_DEBUG << "Closing command console because the game changed!";
					consoleScopedRequests = std::nullopt;
				}

			}
			else
			{
				// basic toggle logic
				if (consoleScopedRequests)
				{
					PLOG_DEBUG << "Closing command console on user input";
					consoleScopedRequests = std::nullopt;
				}
				else
				{
					PLOG_DEBUG << "Opening command console on user input";
					// construct console object! pass it services it wants as per user requests
					lockOrThrow(settingsWeak, settings);



					opScRqst pauseRequest = std::nullopt;
					if (settings->consoleCommandPauseGame->GetValue() && pauseGameOptionalWeak.has_value())
					{
						lockOrThrow(pauseGameOptionalWeak.value(), pauseGame);
						pauseRequest = pauseGame->scopedRequest(nameof(CommandConsoleManagerImpl));
					}

					opScRqst blockInputRequest = std::nullopt;
					if (settings->consoleCommandBlockInput->GetValue() && blockGameInputOptionalWeak.has_value())
					{
						lockOrThrow(blockGameInputOptionalWeak.value(), blockGameInput);
						blockInputRequest = blockGameInput->scopedRequest(nameof(CommandConsoleManagerImpl));
					}

					opScRqst freeCursorRequest = std::nullopt;
					if (settings->consoleCommandFreeCursor->GetValue() && freeCursorOptionalWeak.has_value())
					{
						lockOrThrow(freeCursorOptionalWeak.value(), freeCursor);
						freeCursorRequest = freeCursor->scopedRequest(nameof(CommandConsoleManagerImpl));
					}

					lockOrThrow(hotkeyDisablerWeak, hotkeyDisabler);
					auto hotkeyDisableRequest = hotkeyDisabler->makeRequest(nameof(CommandConsoleManagerImpl));

					std::unique_ptr< ScopedCallback<RenderEvent>> renderEventCallback(new ScopedCallback<RenderEvent>(renderEvent, [this](SimpleMath::Vector2 ss) { commandConsole->render(ss); }));

					consoleScopedRequests = ConsoleScopedRequests(
						std::move(renderEventCallback),
						std::move(engineCommandOutputEvent->subscribe([this](const std::string& s) { commandConsole->onCommandOutput(s); })),
						std::move(hotkeyDisableRequest),
						std::move(pauseRequest),
						std::move(blockInputRequest),
						std::move(freeCursorRequest)
					);


				}
				
			}
				
		}
		catch (HCMRuntimeException ex)
		{
			if (consoleScopedRequests)
			{
				PLOG_DEBUG << "Closing command console due to exception";
				consoleScopedRequests = std::nullopt;
			}

			runtimeExceptions->handleMessage(ex);
		}


	}



	void onMCCStateChanged(const MCCState& newState)
	{
		if (newState.currentGameState != mGame && consoleScopedRequests)
		{
			PLOG_DEBUG << "Closing console because gamestate changed";
			consoleScopedRequests = std::nullopt;
		}
	}






public:
	CommandConsoleManagerImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		commandConsoleHotkeyEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->commandConsoleHotkeyEvent, [this]() { onCommandConsoleHotkeyEvent(); }),
		commandConsoleExecuteBufferEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->commandConsoleExecuteBufferEvent, [this]() { commandConsole->execute(false); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) { onMCCStateChanged(n); }),
		renderEvent(dicon.Resolve<RenderEvent>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		fontSizeChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->consoleCommandFontSize->valueChangedEvent, [this](float& n) { commandConsole->fontSize = n; })
	{
		auto controls = dicon.Resolve< ControlServiceContainer>().lock();

		hotkeyDisablerWeak = controls->hotkeyDisabler;
		pauseGameOptionalWeak = controls->pauseGameService;
		blockGameInputOptionalWeak = controls->blockGameInputService;
		freeCursorOptionalWeak = controls->freeMCCSCursorService;

		auto engineCommand = resolveDependentCheat(EngineCommand);
		commandConsole = std::make_unique<CommandConsoleGUI>(engineCommand, dicon.Resolve<SettingsStateAndEvents>().lock()->consoleCommandFontSize->GetValue());

		auto engineCommandOutputEventProvider = resolveDependentCheat(EngineCommandOutputEvent);
		engineCommandOutputEvent = engineCommandOutputEventProvider->getEngineCommandOutputEvent();

	}

	~CommandConsoleManagerImpl()
	{
		consoleScopedRequests = std::nullopt;
	}
};




CommandConsoleManager::CommandConsoleManager(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique < CommandConsoleManagerImpl>(gameImpl, dicon);
}

CommandConsoleManager::~CommandConsoleManager() = default;