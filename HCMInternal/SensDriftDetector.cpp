#include "pch.h"
#include "SensDriftDetector.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "RuntimeExceptionHandler.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "IMakeOrGetCheat.h"
#include "IMCCStateHook.h"
#include "ModuleHook.h"
#include "MidhookContextInterpreter.h"
#include "GameTickEventHook.h"

template<GameState::Value gameT>
class SensDriftDetectorImpl : public SensDriftDetectorImplUntemplated
{
private:
	static inline std::mutex mDestructionGuard{};
	static inline SensDriftDetectorImpl<gameT>* instance = nullptr;

	//injected services
	std::weak_ptr< SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMessagesGUI> messagesWeak;
	std::weak_ptr< IMCCStateHook> mccStateHookWeak;
	std::weak_ptr< GameTickEventHook> gameTickEventHookWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	// callbacks
	ScopedCallback<ToggleEvent> sensDriftDetectorToggleCallback;
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	// data
	GameState mGame;
	std::shared_ptr<MultilevelPointer> currentSensitivityPointer;
	float* pCurrentSensitivity;

	std::shared_ptr<ModuleMidHook> mouseMovementHook;
	std::shared_ptr<MidhookContextInterpreter> mouseMovementContextInterpreter;


	static void mouseMovementFunctionHook(SafetyHookContext& ctx)
	{
		if (!instance) PLOG_FATAL << "null instance!";

		std::unique_lock<std::mutex> lock(instance->mDestructionGuard); 

		constexpr float incrementConversionFactor = 0.02222222276f * DirectX::XM_PI / 180.f; // converts from in-game-sensitivity value to how much your viewangle will be incremented by when moving a single mouse-pixel

		try
		{
			if (IsBadReadPtr(instance->pCurrentSensitivity, sizeof(float))) throw HCMRuntimeException("Bad sensitivity read ptr!");

			float expectedIncrement = *instance->pCurrentSensitivity * incrementConversionFactor;

			enum class param
			{
				pCurrentMouseMovement
			};
			auto* ctxInterpreter = instance->mouseMovementContextInterpreter.get();

			float currentMouseMovement = **(float**)ctxInterpreter->getParameterRef(ctx, (int)param::pCurrentMouseMovement);




				if (std::abs(currentMouseMovement) > expectedIncrement)
				{
#ifdef HCM_DEBUG

					PLOG_DEBUG << "incrementConversionFactor: " << std::setprecision(10) << incrementConversionFactor;
					PLOG_DEBUG << "expectedIncrement: " << std::setprecision(10) << expectedIncrement;
					PLOG_DEBUG << "*instance->pCurrentSensitivity: " << std::setprecision(10) << *instance->pCurrentSensitivity;
					PLOG_DEBUG << "currentMouseMovement: " << std::setprecision(10) << currentMouseMovement;
#endif

					lockOrThrow(instance->messagesWeak, messages);
					lockOrThrow(instance->gameTickEventHookWeak, gameTickEventHook)
					messages->addMessage(std::format("Sensitivity drift on tick: {}", gameTickEventHook->getCurrentGameTick()));
				}

		}
		catch (HCMRuntimeException ex)
		{
			lockOrThrow(instance->settingsWeak, settings);
			settings->sensDriftDetectorToggle->GetValueDisplay() = false;
			settings->sensDriftDetectorToggle->UpdateValueWithInput();
			instance->mouseMovementHook->setWantsToBeAttached(false);
			instance->runtimeExceptions->handleMessage(ex);
		}
		

	}

	void onSettingChanged()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			lockOrThrow(mccStateHookWeak, mccStateHook);
			bool shouldEnable = mccStateHook->isGameCurrentlyPlaying(mGame) && settings->sensDriftDetectorToggle->GetValue();

			if (shouldEnable)
			{
				uintptr_t currentSensitivityPointerResolved;
				currentSensitivityPointer->resolve(&currentSensitivityPointerResolved);
				pCurrentSensitivity = (float*)currentSensitivityPointerResolved;
			}

			mouseMovementHook->setWantsToBeAttached(shouldEnable);


		}
		catch (HCMRuntimeException ex)
		{
			lockOrThrow(settingsWeak, settings);
			settings->sensDriftDetectorToggle->GetValueDisplay() = false;
			settings->sensDriftDetectorToggle->UpdateValueWithInput();
			mouseMovementHook->setWantsToBeAttached(false);
			runtimeExceptions->handleMessage(ex);
		}
	}

public:
	SensDriftDetectorImpl(GameState game, IDIContainer& dicon)
		:
		mGame(game),
		sensDriftDetectorToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->sensDriftDetectorToggle->valueChangedEvent, [this](bool& n) {onSettingChanged(); }),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) {onSettingChanged(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		messagesWeak(dicon.Resolve<IMessagesGUI>()),
		gameTickEventHookWeak(resolveDependentCheat(GameTickEventHook))
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		currentSensitivityPointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(currentSensitivityPointer), game);

		auto mouseMovementFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(mouseMovementFunction), game);
		mouseMovementHook = ModuleMidHook::make(game.toModuleName(), mouseMovementFunction, mouseMovementFunctionHook);

		mouseMovementContextInterpreter = ptr->getData < std::shared_ptr<MidhookContextInterpreter>>(nameof(mouseMovementContextInterpreter), game);

		instance = this;

	}

	~SensDriftDetectorImpl()
	{
		std::unique_lock<std::mutex> lock(mDestructionGuard); // block until callbacks/hooks finish executing
		instance = nullptr;
	}
};

SensDriftDetector::SensDriftDetector(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique <SensDriftDetectorImpl < GameState::Value::Halo1>>(game, dicon);
		break;


	default:
		throw HCMInitException("Unimplemented game");
	}
}

SensDriftDetector::~SensDriftDetector() = default;