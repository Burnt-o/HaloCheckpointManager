#include "pch.h"
#include "FreeCamera.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "ModuleHook.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
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
#include "GlobalKill.h"
#include "ForceTeleport.h"
#include "FreeCameraFOVOverride.h"

template <GameState::Value gameT>  // templated so that each game gets a seperate instance of the static members
class FreeCameraImpl : public FreeCameraImplUntemplated
{
private:
	//GameState mGame;
	static inline FreeCameraImpl<gameT>* instance = nullptr;
	GameState gameImpl;

	static inline std::atomic_bool hookIsRunning = false;

	// event callbacks
	ScopedCallback <ToggleEvent> mFreeCameraToggleCallback;
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;
	ScopedCallback <ToggleEvent> mThirdPersonRenderingToggleCallback; 
	ScopedCallback <ToggleEvent> mDisableScreenEffectsToggleCallback;
	ScopedCallback <ToggleEvent> mBlockPlayerCharacterInputToggleCallback;

	ScopedCallback <ActionEvent> freeCameraTeleportToCameraCallback;

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
	std::weak_ptr<GetGameCameraData> getGameCameraDataWeak;
	std::weak_ptr<UpdateGameCameraData> updateGameCameraDataWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	std::optional<std::weak_ptr< DisableScreenEffects>> disableScreenEffectsOptionalWeak;
	std::shared_ptr<SharedRequestToken> disableScreenEffectsRequest;
	std::optional<std::weak_ptr< ThirdPersonRendering>> thirdPersonRenderingOptionalWeak;
	std::shared_ptr<SharedRequestToken> thirdPersonRenderingRequest;
	std::optional<std::weak_ptr< BlockPlayerCharacterInput>> blockPlayerCharacterInputOptionalWeak;
	std::shared_ptr<SharedRequestToken> blockPlayerCharacterInputRequest;

	std::optional<std::weak_ptr<ForceTeleport>> forceTeleportOptionalWeak;


	std::weak_ptr<UserCameraInputReader> userCameraInputReaderWeak;
	PositionTransformer userControlledPosition;
	RotationTransformer userControlledRotation;
	FOVTransformer userControlledFOV;

	std::weak_ptr<FreeCameraFOVOverride> freeCameraFOVOverrideProviderWeak;
	std::shared_ptr<SharedRequestToken> freeCameraFOVOverrideRequest;

	// hooks
	static inline std::shared_ptr<ModuleMidHook> setCameraDataHook;

	// data
	bool needToResetCamera = true;
	std::shared_ptr<MultilevelPointer> gameFrameDeltaPointer;
	
	// event to fire on mcc state change (just set needToSetupCamera to true). also disable freecam.
	void onGameStateChange(const MCCState&)
	{
		try
		{
			PLOG_DEBUG << "free camera onGameStateChange";
			lockOrThrow(settingsWeak, settings);
			if (settings->freeCameraToggle->GetValueDisplay())
			{
				settings->freeCameraToggle->GetValueDisplay() = false;
				settings->freeCameraToggle->UpdateValueWithInput();
			}

				

		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

		needToResetCamera = true;
	}


	

	static void setCameraDataHookFunction(SafetyHookContext& ctx)
	{
		LOG_ONCE("setCameraDataHookFunction running for the first time")

		ScopedAtomicBool lock(hookIsRunning);

		if (instance == nullptr)
		{
			PLOG_ERROR << "null freeCameraImpl for gameState " << ((GameState)gameT).toString();
			return;
		}

		LOG_ONCE("calling instance->setCameraData()")
		instance->setCameraData();
	
	}

	

	void resetCamera(GameCameraData& gameCameraData, std::shared_ptr<SettingsStateAndEvents> settings, float targetFOVCopy)
	{
		LOG_ONCE(PLOG_VERBOSE << "resetting camera data");
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

		LOG_ONCE(PLOG_VERBOSE << "setCameraData running for the first time");

		try
		{
			if (GlobalKill::isKillSet())
			{
				onFreeCameraToggleChange(false);
				return;
			}


			LOG_ONCE(PLOG_VERBOSE << "getting game camera data");
			lockOrThrow(getGameCameraDataWeak, getGameCameraData);
			auto gameCameraData = getGameCameraData->getGameCameraData();

			// "world" absolute data. Gets transformed into final position/rotation by cameraTransformers.
			LOG_ONCE(PLOG_VERBOSE << "constructing world-null camera data");
			FreeCameraData freeCameraData;

			lockOrThrow(settingsWeak, settings);

			LOG_ONCE(PLOG_VERBOSE << "getting base FOV from setting value");
			float targetFOVCopy = settings->freeCameraUserInputCameraBaseFOV->GetValue();
			LOG_ONCE_CAPTURE(PLOG_VERBOSE << "FOV value: " << fov, fov = targetFOVCopy);

			if (needToResetCamera)
			{
				LOG_ONCE(PLOG_VERBOSE << "Resetting camera");
				resetCamera(gameCameraData, settings, targetFOVCopy);
				needToResetCamera = false;
			}

			LOG_ONCE(PLOG_VERBOSE << "getting frameDelta");
			float frameDelta;
			if (!gameFrameDeltaPointer->readData(&frameDelta)) throw HCMRuntimeException("Could not resolve MCCFrameDeltaPointer");
			LOG_ONCE_CAPTURE(PLOG_VERBOSE << "frameDelta value: " << fd, fd = frameDelta);

			// can be exactly zero if paused, but negative doesn't even make sense.
			if (frameDelta < 0.f) throw HCMRuntimeException(std::format("negative frame delta?! that's unpossible! {} ", frameDelta));

			// adjust frameDelta by speedhack setting
			{
				LOG_ONCE(PLOG_VERBOSE << "adjusting frameDelta by speedhack multiplier");
				lockOrThrow(speedhackWeak, speedhack);
				frameDelta = frameDelta * (1.00 / speedhack->getCurrentSpeedMultiplier()); // this is also why speedhack must never be zero
				LOG_ONCE_CAPTURE(PLOG_VERBOSE << "speedhack-adjusted frameDelta value: " << fd, fd = frameDelta);
			}

			LOG_ONCE(PLOG_DEBUG << "reading user inputs and applying userControlledCamera transformations");

			if (frameDelta != 0.f) // this would cause division by zeros. frameDelta can be zero when game is paused.
			{
				// we will scale rotation sensitivity by fov value so fov transform first
				userControlledFOV.transformFOV(freeCameraData, frameDelta, targetFOVCopy);
				LOG_ONCE(PLOG_DEBUG << "userControlledFOV.transformFOV DONE");
				freeCameraData.currentFOV = targetFOVCopy;

				// rotate before position so that the position transformer knows which direction is "forward/up/right" etc 
				userControlledRotation.transformRotation(freeCameraData, frameDelta);
				LOG_ONCE(PLOG_DEBUG << "userControlledRotation.transformRotation DONE");

				userControlledPosition.transformPosition(freeCameraData, frameDelta);
				LOG_ONCE(PLOG_DEBUG << "userControlledPosition.transformPosition DONE");


			}


			// must be after userControlled and before any other transformers
			LOG_ONCE_CAPTURE(PLOG_VERBOSE << "updating FOV setting with transformed value: " << fov, fov = targetFOVCopy);
			settings->freeCameraUserInputCameraBaseFOV->GetValueDisplay() = targetFOVCopy; 
			settings->freeCameraUserInputCameraBaseFOV->GetValue() = targetFOVCopy;
			LOG_ONCE(PLOG_VERBOSE << "fov setting updated");

			LOG_ONCE(PLOG_DEBUG << "updating game camera");
			lockOrThrow(updateGameCameraDataWeak, updateGameCameraData);
			updateGameCameraData->updateGameCameraData(gameCameraData, freeCameraData, targetFOVCopy);
			LOG_ONCE(PLOG_DEBUG << "updating game camera DONE");
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
			settingsWeak.lock()->freeCameraToggle->GetValueDisplay() = false;
			settingsWeak.lock()->freeCameraToggle->UpdateValueWithInput();
		}
	}



	// setting toggle event callback
	void onFreeCameraToggleChange(bool newValue)
	{
		try
		{

			needToResetCamera = true; 
			PLOG_DEBUG << "onToggleChange: newval: " << (newValue ? "true" : "false");



			if ( thirdPersonRenderingOptionalWeak.has_value())
			{
				lockOrThrow(settingsWeak, settings);
				lockOrThrow(thirdPersonRenderingOptionalWeak.value(), thirdPersonRendering);
				
				if (newValue && settings->freeCameraThirdPersonRendering->GetValue())
					thirdPersonRenderingRequest = thirdPersonRendering->makeScopedRequest();
				else
					thirdPersonRenderingRequest.reset();
			}

			if (disableScreenEffectsOptionalWeak.has_value())
			{
				lockOrThrow(settingsWeak, settings);
				lockOrThrow(disableScreenEffectsOptionalWeak.value(), disableScreenEffects);

				if (newValue && settings->freeCameraDisableScreenEffects->GetValue())
					disableScreenEffectsRequest = disableScreenEffects->makeScopedRequest();
				else
					disableScreenEffectsRequest.reset();
			}

			if (blockPlayerCharacterInputOptionalWeak.has_value())
			{
				lockOrThrow(settingsWeak, settings);
				lockOrThrow(blockPlayerCharacterInputOptionalWeak.value(), blockPlayerCharacterInput);

				if (newValue && settings->freeCameraGameInputDisable->GetValue())
					blockPlayerCharacterInputRequest = blockPlayerCharacterInput->makeScopedRequest();
				else
					blockPlayerCharacterInputRequest.reset();
			}

			// set hooks 
			{
				LOG_ONCE(PLOG_VERBOSE << "freezing threads as we update setCameraDataHook");


				LOG_ONCE(PLOG_VERBOSE << "setCameraDataHook->setWantsToBeAttached()")
				setCameraDataHook->setWantsToBeAttached(newValue);
				LOG_ONCE(PLOG_VERBOSE << "setCameraDataHook->setWantsToBeAttached() DONE");
				
				if (newValue)
				{
					PLOG_DEBUG << "setCameraDataHook redirecting to new func at: " << std::hex << &setCameraDataHookFunction;
				}

				if (newValue)
				{
					lockOrThrow(freeCameraFOVOverrideProviderWeak, freeCameraFOVOverrideProvider);
					freeCameraFOVOverrideRequest = freeCameraFOVOverrideProvider->makeScopedRequest();
				}
				else if (freeCameraFOVOverrideRequest)
					freeCameraFOVOverrideRequest.reset();

			}



			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameT))
			{
				lockOrThrow(messagesGUIWeak, messagesGUI)
				messagesGUI->addMessage(newValue ? "Free Camera enabled." : "Free Camera disabled.");
			}

		}
		catch (HCMRuntimeException ex)
		{
			thirdPersonRenderingRequest.reset();
			blockPlayerCharacterInputRequest.reset();
			disableScreenEffectsRequest.reset();
			freeCameraFOVOverrideRequest.reset();
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

				if (newValue)
					thirdPersonRenderingRequest = thirdPersonRendering->makeScopedRequest();
				else
					thirdPersonRenderingRequest.reset();
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

				if (newValue)
					disableScreenEffectsRequest = disableScreenEffects->makeScopedRequest();
				else
					disableScreenEffectsRequest.reset();
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

				if (newValue)
					blockPlayerCharacterInputRequest = blockPlayerCharacterInput->makeScopedRequest();
				else
					blockPlayerCharacterInputRequest.reset();
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
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameImpl) == false) return;

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
			PLOG_DEBUG << "onFreeCameraUserInputCameraSetPositionFillCurrent running game: " << gameImpl.toString();

			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameImpl) == false) return;

			lockOrThrow(settingsWeak, settings);

			if (settings->freeCameraToggle->GetValue() == false)
			{
				throw HCMRuntimeException("Can't do that while freecam is disabled!");
			}

			// why is this getting junk data?

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
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameImpl) == false) return;

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
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameImpl) == false) return;

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
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameImpl) == false) return;

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
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameImpl) == false) return;

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
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameImpl) == false) return;

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
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameImpl) == false) return;

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
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameImpl) == false) return;

			lockOrThrow(settingsWeak, settings);
			if (settings->freeCameraToggle->GetValue() == false) return;

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
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameImpl) == false) return;

			lockOrThrow(settingsWeak, settings);
			if (settings->freeCameraToggle->GetValue() == false) return;

			float factor = settings->freeCameraUserInputCameraTranslationSpeedChangeFactor->GetValue();
			settings->freeCameraUserInputCameraTranslationSpeed->GetValueDisplay() = settings->freeCameraUserInputCameraTranslationSpeed->GetValueDisplay() / factor;
			settings->freeCameraUserInputCameraTranslationSpeed->UpdateValueWithInput();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onFreeCameraTeleportToCameraCallback()
	{
		try
		{

			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(gameImpl) == false) return;

			lockOrThrow(settingsWeak, settings);

			if (settings->freeCameraToggle->GetValue() == false)
			{
				PLOG_ERROR << "Can't do that while freecam is disabled!";
				return;
			}

			if (forceTeleportOptionalWeak.has_value() == false)
			{
				throw HCMRuntimeException("Could not resolve force teleport service!");
			}

			lockOrThrow(forceTeleportOptionalWeak.value(), forceTeleport);


			auto cameraPosition = userControlledPosition.getPositionTransformation();

			if (settings->freeCameraTeleportToCameraSlightlyBehind->GetValue())
			{
				auto camRot = userControlledRotation.getRotationTransformation();
				// convert euler to lookdir
				SimpleMath::Vector3 cameraDirection =
				{
					std::cos(camRot.x) * std::cos(camRot.y),
					std::sin(camRot.x) * std::cos(camRot.y),
					std::sin(camRot.y)
				};

				// cam direction is a unit vector; one unit behind should be enough to make it not visible.
				cameraPosition -= cameraDirection;
			}

			forceTeleport->teleportPlayerTo(cameraPosition);


		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}




public: 
	FreeCameraImpl(GameState game, IDIContainer& dicon)
		:
		gameImpl(game),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		getGameCameraDataWeak(resolveDependentCheat(GetGameCameraData)),
		getPlayerViewAngleWeak(resolveDependentCheat(GetPlayerViewAngle)),
		updateGameCameraDataWeak(resolveDependentCheat(UpdateGameCameraData)),
		userCameraInputReaderWeak(resolveDependentCheat(UserCameraInputReader)),
		speedhackWeak(resolveDependentCheat(Speedhack)),
		mFreeCameraToggleCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraToggle->valueChangedEvent, [this](bool& n) { onFreeCameraToggleChange(n); }),
		mThirdPersonRenderingToggleCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraThirdPersonRendering->valueChangedEvent, [this](bool& n) { onThirdPersonRenderingChange(n); }),
		mDisableScreenEffectsToggleCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraDisableScreenEffects->valueChangedEvent, [this](bool& n) { onDisableScreenEffectsChange(n); }),
		mBlockPlayerCharacterInputToggleCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraGameInputDisable->valueChangedEvent, [this](bool& n) { onBlockPlayerCharacterInputChange(n); }),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); }),
		userControlledPosition(userCameraInputReaderWeak.lock(), settingsWeak.lock()->freeCameraUserInputCameraTranslationInterpolator, settingsWeak.lock()->freeCameraUserInputCameraTranslationInterpolatorLinearFactor),
		userControlledRotation(userCameraInputReaderWeak.lock(), settingsWeak.lock()->freeCameraUserInputCameraRotationInterpolator, settingsWeak.lock()->freeCameraUserInputCameraRotationInterpolatorLinearFactor),
		userControlledFOV(userCameraInputReaderWeak.lock(), settingsWeak.lock()->freeCameraUserInputCameraFOVInterpolator, settingsWeak.lock()->freeCameraUserInputCameraFOVInterpolatorLinearFactor),

		freeCameraUserInputCameraSetPositionCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetPosition, [this]() { onFreeCameraUserInputCameraSetPosition(); }),
		freeCameraUserInputCameraSetPositionFillCurrentCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetPositionFillCurrent, [this]() { onFreeCameraUserInputCameraSetPositionFillCurrent(); }),
		freeCameraUserInputCameraSetPositionCopyCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetPositionCopy, [this]() { onFreeCameraUserInputCameraSetPositionCopy(); }),
		freeCameraUserInputCameraSetPositionPasteCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetPositionPaste, [this]() { onFreeCameraUserInputCameraSetPositionPaste(); }),
		freeCameraUserInputCameraSetRotationCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetRotation, [this]() { onFreeCameraUserInputCameraSetRotation(); }),
		freeCameraUserInputCameraSetRotationFillCurrentCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetRotationFillCurrent, [this]() { onFreeCameraUserInputCameraSetRotationFillCurrent(); }),
		freeCameraUserInputCameraSetRotationCopyCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetRotationCopy, [this]() { onFreeCameraUserInputCameraSetRotationCopy(); }),
		freeCameraUserInputCameraSetRotationPasteCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraSetRotationPaste, [this]() { onFreeCameraUserInputCameraSetRotationPaste(); }),
	
		freeCameraUserInputCameraIncreaseTranslationSpeedHotkeyCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraIncreaseTranslationSpeedHotkey, [this]() { onFreeCameraUserInputCameraIncreaseTranslationSpeedHotkey(); }),
		freeCameraUserInputCameraDecreaseTranslationSpeedHotkeyCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraUserInputCameraDecreaseTranslationSpeedHotkey, [this]() { onFreeCameraUserInputCameraDecreaseTranslationSpeedHotkey(); }),
		freeCameraTeleportToCameraCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->freeCameraTeleportToCameraEvent, [this]() {onFreeCameraTeleportToCameraCallback(); }),
		freeCameraFOVOverrideProviderWeak(resolveDependentCheat(FreeCameraFOVOverride))
	{
		instance = this;
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		gameFrameDeltaPointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameFrameDeltaPointer));
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

		try
		{
			forceTeleportOptionalWeak = resolveDependentCheat(ForceTeleport);
		}
		catch (HCMInitException ex)
		{
			PLOG_DEBUG << "failed to resolve optional dependent cheat forceTeleportOptionalWeak, error: " << ex.what();
		}

		   

	}

	~FreeCameraImpl()
	{
		PLOG_DEBUG << "~FreeCameraImpl";
		// safety check that hook isn't running
		if (hookIsRunning)
		{
			PLOG_INFO << "Waiting for freeCamera hook to finish execution";
			hookIsRunning.wait(true);
		}



		if (setCameraDataHook)
			setCameraDataHook.reset();


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
	PLOG_DEBUG << "FreeCamera destructor was called! are we not cleaning up?";
	PLOG_DEBUG << "~" << getName();
}