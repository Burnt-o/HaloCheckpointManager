#include "pch.h"
#include "FreeCamera.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "ModuleHook.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MidhookFlagInterpreter.h"
#include "IMakeOrGetCheat.h"
#include "GetGameCameraData.h"
#include "GetPlayerViewAngle.h"
#include "ThirdPersonRendering.h"
#include "DisableScreenEffects.h"
#include "BlockPlayerCharacterInput.h"
//#include "PlayerControlledFreeCamera.h"
//#include "ObjectAnchoredFreeCam.h"
#include "UpdateGameCameraData.h"
#include "FreeCameraData.h"
#include "GameCameraData.h"
#include "CameraTransformer.h"
#include "Speedhack.h"

#include "ISmoother.h"
#include "NullSmoother.h"
#include "LinearSmoother.h"

#include "UserCameraInputReader.h"

template <GameState::Value gameT>  // templated so that each game gets a seperate instance of the static members
class FreeCameraImpl : public FreeCameraImplUntemplated
{
private:
	//GameState mGame;
	static inline FreeCameraImpl<gameT>* instance = nullptr;

	// event callbacks
	ScopedCallback <ToggleEvent> mFreeCameraToggleCallback;
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;
	ScopedCallback <ToggleEvent> mThirdPersonRenderingToggleCallback; 
	ScopedCallback <ToggleEvent> mDisableScreenEffectsToggleCallback;
	ScopedCallback <ToggleEvent> mBlockPlayerCharacterInputToggleCallback;


	ScopedCallback <ActionEvent> freeCameraUserInputCameraSetPositionCallback;
	ScopedCallback <ActionEvent> freeCameraUserInputCameraSetPositionFillCurrentCallback;
	ScopedCallback <ActionEvent> freeCameraUserInputCameraSetPositionCopyCallback;
	ScopedCallback <ActionEvent> freeCameraUserInputCameraSetPositionPasteCallback;
	ScopedCallback <ActionEvent> freeCameraUserInputCameraSetRotationCallback;
	ScopedCallback <ActionEvent> freeCameraUserInputCameraSetRotationFillCurrentCallback;
	ScopedCallback <ActionEvent> freeCameraUserInputCameraSetRotationCopyCallback;
	ScopedCallback <ActionEvent> freeCameraUserInputCameraSetRotationPasteCallback;

	ScopedCallback <ActionEvent> freeCameraUserInputCameraIncreaseTranslationSpeedHotkeyCallback;
	ScopedCallback <ActionEvent> freeCameraUserInputCameraDecreaseTranslationSpeedHotkeyCallback;


	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<GetPlayerViewAngle> getPlayerViewAngleWeak;
	std::weak_ptr<Speedhack> speedhackWeak;
	std::shared_ptr<GetGameCameraData> getGameCameraData;
	std::shared_ptr<UpdateGameCameraData> updateGameCameraData;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	std::optional<std::weak_ptr< DisableScreenEffects>> disableScreenEffectsOptionalWeak;
	std::optional<std::weak_ptr< ThirdPersonRendering>> thirdPersonRenderingOptionalWeak;
	std::optional<std::weak_ptr< BlockPlayerCharacterInput>> blockPlayerCharacterInputOptionalWeak;


	std::shared_ptr<UserCameraInputReader> userCameraInputReader;
	PositionTransformer userControlledPosition;
	RotationTransformer userControlledRotation;
	FOVTransformer userControlledFOV;

	// hooks
	static inline std::shared_ptr<ModuleMidHook> setCameraDataHook;
	static inline std::shared_ptr<ModulePatch> freeCameraHalo2ExtraHook; // stupid halo 2 fix for FOV

	// data
	bool needToResetCamera = true;
	std::shared_ptr<MultilevelPointer> frameDeltaPointer;
	
	// event to fire on mcc state change (just set needToSetupCamera to true)
	void onGameStateChange(const MCCState&)
	{
		FreeCamera::cameraIsFree = false;
		needToResetCamera = true;
	}


	

	static void setCameraDataHookFunction(SafetyHookContext& ctx)
	{
		if (instance == nullptr)
		{
			PLOG_ERROR << "null freeCameraImpl for gameState " << ((GameState)gameT).toString();
			return;
		}

		instance->setCameraData();
	
	}

	

	void resetCamera(GameCameraData& gameCameraData, std::shared_ptr<SettingsStateAndEvents> settings, float targetFOVCopy)
	{
	
		// position
		userControlledPosition.setPositionTransformation(*gameCameraData.position);


		// rotation
		float initialYaw = std::atan2(gameCameraData.lookDirForward->y, gameCameraData.lookDirForward->x);
		float initialPitch = std::asin(gameCameraData.lookDirForward->z) * -1.f;
		float initialRoll = 0.f;

		PLOG_DEBUG << "Initial EulerYaw: " << initialYaw;
		PLOG_DEBUG << "Initial EulerPitch: " << initialPitch;
		PLOG_DEBUG << "Initial EulerRoll: " << initialRoll;

		userControlledRotation.setRotationTransformation(initialYaw, initialPitch, initialRoll);

		// fov
		userControlledFOV.setFOV(targetFOVCopy);

	}


	void setCameraData()
	{
		try
		{
			FreeCamera::cameraIsFree = true;

			auto gameCameraData = getGameCameraData->getGameCameraData();

			// "world" absolute data. Gets transformed into final position/rotation by cameraTransformers.
			FreeCameraData freeCameraData;

			lockOrThrow(settingsWeak, settings);
			
			float targetFOVCopy = settings->freeCameraUserInputCameraBaseFOV->GetValue();


			if (needToResetCamera)
			{
				resetCamera(gameCameraData, settings, targetFOVCopy);
				needToResetCamera = false;
			}

			float frameDelta;
			if (!frameDeltaPointer->readData(&frameDelta)) throw HCMRuntimeException("Could not resolve frameDeltaPointer");

			// adjust by speedhack setting
			{
				lockOrThrow(speedhackWeak, speedhack);
				frameDelta = frameDelta * (1.00 / speedhack->getCurrentSpeedMultiplier());
			}

			LOG_ONCE(PLOG_DEBUG << "reading done, transforming camera");

			// rotate FIRST so position updater knows which direction is "forward/up/right" etc 
			userControlledRotation.transformRotation(freeCameraData, frameDelta);
			userControlledPosition.transformPosition(freeCameraData, frameDelta);
			userControlledFOV.transformFOV(freeCameraData, frameDelta, targetFOVCopy);

			// must be after userControlled and before any other transformers
			settings->freeCameraUserInputCameraBaseFOV->GetValue() = targetFOVCopy;
			settings->freeCameraUserInputCameraBaseFOV->GetValueDisplay() = targetFOVCopy; 

			LOG_ONCE(PLOG_DEBUG << "transforming done, updating game camera");
			updateGameCameraData->updateGameCameraData(gameCameraData, freeCameraData, targetFOVCopy);

		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
			settingsWeak.lock()->freeCameraToggle->GetValueDisplay() = false;
			settingsWeak.lock()->freeCameraToggle->UpdateValueWithInput();
		}
	}



	// setting toggle event callback
	void onFreeCameraToggleChange(bool& newValue)
	{
		try
		{
			FreeCamera::cameraIsFree = false;

			needToResetCamera = true; 
			PLOG_DEBUG << "onToggleChange: newval: " << newValue;



			if ( thirdPersonRenderingOptionalWeak.has_value())
			{
				lockOrThrow(settingsWeak, settings);
				lockOrThrow(thirdPersonRenderingOptionalWeak.value(), thirdPersonRendering);
				thirdPersonRendering->toggleThirdPersonRendering(settings->freeCameraThirdPersonRendering->GetValue() && newValue);
			}

			if (disableScreenEffectsOptionalWeak.has_value())
			{
				lockOrThrow(settingsWeak, settings);
				lockOrThrow(disableScreenEffectsOptionalWeak.value(), disableScreenEffects);
				disableScreenEffects->toggleDisableScreenEffects(settings->freeCameraDisableScreenEffects->GetValue() && newValue);
			}

			if (blockPlayerCharacterInputOptionalWeak.has_value())
			{
				lockOrThrow(settingsWeak, settings);
				lockOrThrow(blockPlayerCharacterInputOptionalWeak.value(), blockPlayerCharacterInput);
				blockPlayerCharacterInput->toggleBlockPlayerCharacterInput(settings->freeCameraGameInputDisable->GetValue() && newValue);
			}

			// set hooks 
			{
				safetyhook::ThreadFreezer threadFreezer; 
				setCameraDataHook->setWantsToBeAttached(newValue);

				if constexpr (gameT == GameState::Value::Halo2)
				{
					freeCameraHalo2ExtraHook->setWantsToBeAttached(newValue);
				}

			}



			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameT))
			{
				FreeCamera::cameraIsFree = newValue;

				lockOrThrow(messagesGUIWeak, messagesGUI)
				messagesGUI->addMessage(newValue ? "Free Camera enabled." : "Free Camera disabled.");
			}

		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
			try
			{
				lockOrThrow(settingsWeak, settings);
				settings->freeCameraToggle->resetToDefaultValue();
			}
			catch (HCMRuntimeException ex)
			{
				runtimeExceptions->handleMessage(ex);
			}
		}

	}


	void onThirdPersonRenderingChange(bool& newValue)
	{
		if (!thirdPersonRenderingOptionalWeak.has_value()) return;

		try
		{
			lockOrThrow(settingsWeak, settings);
			if (settings->freeCameraToggle->GetValue()) // only need to bother flipping if freeCamera is enabled
			{
				lockOrThrow(thirdPersonRenderingOptionalWeak.value(), thirdPersonRendering);
				thirdPersonRendering->toggleThirdPersonRendering(newValue);
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onDisableScreenEffectsChange(bool& newValue)
	{
		if (!disableScreenEffectsOptionalWeak.has_value()) return;

		try
		{
			lockOrThrow(settingsWeak, settings);
			if (settings->freeCameraToggle->GetValue()) // only need to bother flipping if freeCamera is enabled
			{
				lockOrThrow(disableScreenEffectsOptionalWeak.value(), disableScreenEffects);
				disableScreenEffects->toggleDisableScreenEffects(newValue);
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onBlockPlayerCharacterInputChange(bool& newValue)
	{
		if (!blockPlayerCharacterInputOptionalWeak.has_value()) return;

		try
		{
			lockOrThrow(settingsWeak, settings);
			if (settings->freeCameraToggle->GetValue()) // only need to bother flipping if freeCamera is enabled
			{
				lockOrThrow(blockPlayerCharacterInputOptionalWeak.value(), blockPlayerCharacterInput);
				blockPlayerCharacterInput->toggleBlockPlayerCharacterInput(newValue);
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}



	void onFreeCameraUserInputCameraSetPosition()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);

			if (settings->freeCameraToggle->GetValue() == false)
			{
				throw HCMRuntimeException("Can't do that while freecam is disabled!");
			}

			userControlledPosition.setPositionTransformation(settings->freeCameraUserInputCameraSetPositionVec3->GetValue());
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}
	void onFreeCameraUserInputCameraSetPositionFillCurrent()
	{
		try 
		{
			lockOrThrow(settingsWeak, settings);

			if (settings->freeCameraToggle->GetValue() == false)
			{
				throw HCMRuntimeException("Can't do that while freecam is disabled!");
			}

			settings->freeCameraUserInputCameraSetPositionVec3->GetValueDisplay() = userControlledPosition.getPositionTransformation();
			settings->freeCameraUserInputCameraSetPositionVec3->UpdateValueWithInput();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}
	void onFreeCameraUserInputCameraSetPositionCopy()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			settings->freeCameraUserInputCameraSetPositionVec3->serialiseToClipboard();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}
	void onFreeCameraUserInputCameraSetPositionPaste()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			settings->freeCameraUserInputCameraSetPositionVec3->deserialiseFromClipboard();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}
	void onFreeCameraUserInputCameraSetRotation()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);

			if (settings->freeCameraToggle->GetValue() == false)
			{
				throw HCMRuntimeException("Can't do that while freecam is disabled!");
			}

			userControlledRotation.setRotationTransformation(settings->freeCameraUserInputCameraSetRotationVec3->GetValue());
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}
	void onFreeCameraUserInputCameraSetRotationFillCurrent()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);

			if (settings->freeCameraToggle->GetValue() == false)
			{
				throw HCMRuntimeException("Can't do that while freecam is disabled!");
			}

			settings->freeCameraUserInputCameraSetRotationVec3->GetValueDisplay() = userControlledRotation.getRotationTransformation();
			settings->freeCameraUserInputCameraSetRotationVec3->UpdateValueWithInput();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}
	void onFreeCameraUserInputCameraSetRotationCopy()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			settings->freeCameraUserInputCameraSetRotationVec3->serialiseToClipboard();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}
	void onFreeCameraUserInputCameraSetRotationPaste()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			settings->freeCameraUserInputCameraSetRotationVec3->deserialiseFromClipboard();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onFreeCameraUserInputCameraIncreaseTranslationSpeedHotkey()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			float factor = settings->freeCameraUserInputCameraTranslationSpeedChangeFactor->GetValue();
			settings->freeCameraUserInputCameraTranslationSpeed->GetValueDisplay() = settings->freeCameraUserInputCameraTranslationSpeed->GetValueDisplay() * factor;
			settings->freeCameraUserInputCameraTranslationSpeed->UpdateValueWithInput();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onFreeCameraUserInputCameraDecreaseTranslationSpeedHotkey()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			float factor = settings->freeCameraUserInputCameraTranslationSpeedChangeFactor->GetValue();
			settings->freeCameraUserInputCameraTranslationSpeed->GetValueDisplay() = settings->freeCameraUserInputCameraTranslationSpeed->GetValueDisplay() / factor;
			settings->freeCameraUserInputCameraTranslationSpeed->UpdateValueWithInput();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}


public: 
	FreeCameraImpl(GameState game, IDIContainer& dicon)
		:
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		getGameCameraData(resolveDependentCheat(GetGameCameraData)),
		getPlayerViewAngleWeak(resolveDependentCheat(GetPlayerViewAngle)),
		updateGameCameraData(resolveDependentCheat(UpdateGameCameraData)),
		userCameraInputReader(resolveDependentCheat(UserCameraInputReader)),
		speedhackWeak(resolveDependentCheat(Speedhack)),
		mFreeCameraToggleCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraToggle->valueChangedEvent, [this](bool& n) { onFreeCameraToggleChange(n); }),
		mThirdPersonRenderingToggleCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraThirdPersonRendering->valueChangedEvent, [this](bool& n) { onThirdPersonRenderingChange(n); }),
		mDisableScreenEffectsToggleCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraDisableScreenEffects->valueChangedEvent, [this](bool& n) { onDisableScreenEffectsChange(n); }),
		mBlockPlayerCharacterInputToggleCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraGameInputDisable->valueChangedEvent, [this](bool& n) { onBlockPlayerCharacterInputChange(n); }),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); }),
		userControlledPosition(userCameraInputReader, settingsWeak.lock()->freeCameraUserInputCameraTranslationInterpolator, settingsWeak.lock()->freeCameraUserInputCameraTranslationInterpolatorLinearFactor),
		userControlledRotation(userCameraInputReader, settingsWeak.lock()->freeCameraUserInputCameraRotationInterpolator, settingsWeak.lock()->freeCameraUserInputCameraRotationInterpolatorLinearFactor),
		userControlledFOV(userCameraInputReader, settingsWeak.lock()->freeCameraUserInputCameraFOVInterpolator, settingsWeak.lock()->freeCameraUserInputCameraFOVInterpolatorLinearFactor),

		freeCameraUserInputCameraSetPositionCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetPosition, [this]() { onFreeCameraUserInputCameraSetPosition(); }),
		freeCameraUserInputCameraSetPositionFillCurrentCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetPositionFillCurrent, [this]() { onFreeCameraUserInputCameraSetPositionFillCurrent(); }),
		freeCameraUserInputCameraSetPositionCopyCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetPositionCopy, [this]() { onFreeCameraUserInputCameraSetPositionCopy(); }),
		freeCameraUserInputCameraSetPositionPasteCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetPositionPaste, [this]() { onFreeCameraUserInputCameraSetPositionPaste(); }),
		freeCameraUserInputCameraSetRotationCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetRotation, [this]() { onFreeCameraUserInputCameraSetRotation(); }),
		freeCameraUserInputCameraSetRotationFillCurrentCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetRotationFillCurrent, [this]() { onFreeCameraUserInputCameraSetRotationFillCurrent(); }),
		freeCameraUserInputCameraSetRotationCopyCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetRotationCopy, [this]() { onFreeCameraUserInputCameraSetRotationCopy(); }),
		freeCameraUserInputCameraSetRotationPasteCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetRotationPaste, [this]() { onFreeCameraUserInputCameraSetRotationPaste(); }),
	
		freeCameraUserInputCameraIncreaseTranslationSpeedHotkeyCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraIncreaseTranslationSpeedHotkey, [this]() { onFreeCameraUserInputCameraIncreaseTranslationSpeedHotkey(); }),
		freeCameraUserInputCameraDecreaseTranslationSpeedHotkeyCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraDecreaseTranslationSpeedHotkey, [this]() { onFreeCameraUserInputCameraDecreaseTranslationSpeedHotkey(); })

	{
		instance = this;
		auto ptr = dicon.Resolve<PointerManager>().lock();
		frameDeltaPointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(frameDeltaPointer), game);
		auto setCameraDataFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(setCameraDataFunction), game);
		setCameraDataHook = ModuleMidHook::make(game.toModuleName(), setCameraDataFunction, setCameraDataHookFunction);

		try
		{
			thirdPersonRenderingOptionalWeak = resolveDependentCheat(ThirdPersonRendering);
		}
		catch (HCMInitException ex)
		{
			PLOG_DEBUG << "failed to resolve optional dependent cheat thirdPersonRenderingOptionalWeak, error: " << ex.what();
		}

		try
		{
			disableScreenEffectsOptionalWeak = resolveDependentCheat(DisableScreenEffects);
		}
		catch (HCMInitException ex)
		{
			PLOG_DEBUG << "failed to resolve optional dependent cheat disableScreenEffectsOptionalWeak, error: " << ex.what();
		}

		try
		{
			blockPlayerCharacterInputOptionalWeak = resolveDependentCheat(BlockPlayerCharacterInput);
		}
		catch (HCMInitException ex)
		{
			PLOG_DEBUG << "failed to resolve optional dependent cheat blockPlayerCharacterInputOptionalWeak, error: " << ex.what();
		}

		if constexpr (gameT == GameState::Value::Halo2)
		{
			auto freeCameraHalo2ExtraFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(freeCameraHalo2ExtraFunction), game);
			auto freeCameraHalo2ExtraCode = ptr->getVectorData<byte>(nameof(freeCameraHalo2ExtraCode), game);
			freeCameraHalo2ExtraHook = ModulePatch::make(game.toModuleName(), freeCameraHalo2ExtraFunction, *freeCameraHalo2ExtraCode.get());

		}
		   

	}

	~FreeCameraImpl()
	{
		// TODO: add a safety check that hook isn't running
		instance = nullptr;
	}
};







FreeCamera::FreeCamera(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<FreeCameraImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<FreeCameraImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<FreeCameraImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<FreeCameraImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<FreeCameraImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<FreeCameraImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;

	default:
		throw HCMInitException("not impl");
	}
}

FreeCamera::~FreeCamera()
{
	PLOG_DEBUG << "~" << getName();
}