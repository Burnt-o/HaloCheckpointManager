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
//#include "PlayerControlledFreeCamera.h"
//#include "ObjectAnchoredFreeCam.h"
#include "UpdateGameCameraData.h"
#include "FreeCameraData.h"
#include "GameCameraData.h"
#include "CameraTransformer.h"

#include "ISmoother.h"
#include "NullSmoother.h"
#include "LinearSmoother.h"
#include "SquareSmoother.h"

#include "CameraInputReader.h"

template <GameState::Value gameT>  // templated so that each game gets a seperate instance of the static members
class FreeCameraImpl : public FreeCameraImplUntemplated
{
private:
	//GameState mGame;
	static inline FreeCameraImpl<gameT>* instance = nullptr;

	// event callbacks
	ScopedCallback <ToggleEvent> mFreeCameraToggleCallback;
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;



	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<GetPlayerViewAngle> getPlayerViewAngleWeak;
	std::shared_ptr<GetGameCameraData> getGameCameraData;
	std::shared_ptr<UpdateGameCameraData> updateGameCameraData;
	//std::shared_ptr<ObjectAnchoredFreeCam> objectAnchoredFreeCam;
	//std::shared_ptr<PlayerControlledFreeCamera> playerControlledFreeCamera;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	CameraTransformer playerControlledCameraTransformer = CameraTransformer(
		std::make_shared<LinearSmoother<SimpleMath::Vector3>>(0.06f),
		std::make_shared<LinearSmoother<float>>(0.06f),
		std::make_shared<LinearSmoother<float>>(0.85f)
	);

	std::shared_ptr<CameraInputReader> playerControlledCameraInput;

	CameraTransformer objectAnchoredCameraTransformer = CameraTransformer(
		std::make_shared<LinearSmoother<SimpleMath::Vector3>>(0.9f),
		std::make_shared<LinearSmoother<float>>(0.6f),
		std::make_shared<LinearSmoother<float>>(0.85f)
	);


	// hooks
	static inline std::shared_ptr<ModuleMidHook> setCameraDataHook;
	static inline std::shared_ptr<ModuleMidHook> renderPlayerHook; 
	std::shared_ptr< MidhookFlagInterpreter> renderPlayerFunctionFlagSetter;

	// data
	bool needToSetupCamera = true;
	std::shared_ptr<MultilevelPointer> frameDeltaPointer;
	
	// event to fire on mcc state change (just set needToSetupCamera to true)
	void onGameStateChange(const MCCState&)
	{
		needToSetupCamera = true;
	}


	static void renderPlayerHookFunction(SafetyHookContext& ctx)
	{
		if (instance == nullptr)
		{
			PLOG_ERROR << "null freeCameraImpl for gameState " << ((GameState)gameT).toString();
			return;
		}

		instance->renderPlayerFunctionFlagSetter->setFlag(ctx);

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

	void setCameraData()
	{
		try
		{
			auto gameCameraData = getGameCameraData->getGameCameraData();

			// "world" absolute data. Gets transformed into final position/rotation by cameraTransformers.
			FreeCameraData freeCameraData;
			float currentFOVOffset = 0;




			static float interpolationRate = 0.05f;
			if (GetKeyState('J') & 0x8000)
			{
				interpolationRate += 0.001f;
				playerControlledCameraTransformer.positionSmoother->setSmoothRate(interpolationRate);
				playerControlledCameraTransformer.fovSmoother->setSmoothRate(interpolationRate);
				playerControlledCameraTransformer.rotationSmoother->setSmoothRate(interpolationRate);
				PLOG_DEBUG << "interpolationRate: " << interpolationRate;
				Sleep(50);
			}

			if (GetKeyState('K') & 0x8000)
			{
				interpolationRate -= 0.001f;
				playerControlledCameraTransformer.positionSmoother->setSmoothRate(interpolationRate);
				playerControlledCameraTransformer.fovSmoother->setSmoothRate(interpolationRate);
				playerControlledCameraTransformer.rotationSmoother->setSmoothRate(interpolationRate);
					PLOG_DEBUG << "interpolationRate: " << interpolationRate;
				Sleep(50);
			}


			if (needToSetupCamera)
			{
				LOG_ONCE("Setting up camera transformers");

				// if objectAnchoring enabled then this target position actually needs to be like 0 or something close to it.
				playerControlledCameraTransformer.relativeCameraState.targetPositionTransformation = *gameCameraData.position;
				playerControlledCameraTransformer.relativeCameraState.targetFOVOffset = 0;
					

				auto forward = *gameCameraData.lookDirForward;
				auto up = *gameCameraData.lookDirUp;
				auto right = SimpleMath::Vector3::Transform(forward, SimpleMath::Quaternion::CreateFromAxisAngle(up, DirectX::XM_PIDIV2));


				// https://stackoverflow.com/questions/21622956/how-to-convert-direction-vector-to-euler-angles
				playerControlledCameraTransformer.relativeCameraState.targetEulerYaw = std::atan2(forward.y, forward.x);
				playerControlledCameraTransformer.relativeCameraState.targetEulerPitch = std::asin(forward.z) * -1.f; // dunno why this is backwards
				playerControlledCameraTransformer.relativeCameraState.targetEulerRoll = 0.f;

				PLOG_DEBUG << "Initial EulerYaw: " << playerControlledCameraTransformer.relativeCameraState.targetEulerYaw;
				PLOG_DEBUG << "Initial EulerPitch: " << playerControlledCameraTransformer.relativeCameraState.targetEulerPitch;
				PLOG_DEBUG << "Initial EulerRoll: " << playerControlledCameraTransformer.relativeCameraState.targetEulerRoll;



				// set current value to.. current value
				playerControlledCameraTransformer.relativeCameraState.currentPositionTransformation = playerControlledCameraTransformer.relativeCameraState.targetPositionTransformation;
				playerControlledCameraTransformer.relativeCameraState.currentFOVOffset = 0;
				playerControlledCameraTransformer.relativeCameraState.currentEulerYaw = playerControlledCameraTransformer.relativeCameraState.targetEulerYaw;
				playerControlledCameraTransformer.relativeCameraState.currentEulerPitch = playerControlledCameraTransformer.relativeCameraState.targetEulerPitch;
				playerControlledCameraTransformer.relativeCameraState.currentEulerRoll = playerControlledCameraTransformer.relativeCameraState.targetEulerRoll;



				needToSetupCamera = false;
			}

			float frameDelta;
			if (!frameDeltaPointer->readData(&frameDelta)) throw HCMRuntimeException("Could not resolve frameDeltaPointer");

			LOG_ONCE(PLOG_DEBUG << "reading done, transforming camera");

			// rotate FIRST so position updater knows which direction is "forward/up/right" etc 
			playerControlledCameraInput->readRotationInput(playerControlledCameraTransformer.relativeCameraState, freeCameraData, frameDelta);
			playerControlledCameraTransformer.transformCameraRotation(freeCameraData, frameDelta);

			playerControlledCameraInput->readPositionInput(playerControlledCameraTransformer.relativeCameraState, freeCameraData,  frameDelta);
			playerControlledCameraTransformer.transformCameraPosition(freeCameraData, frameDelta);

			playerControlledCameraInput->readFOVInput(playerControlledCameraTransformer.relativeCameraState, freeCameraData, frameDelta);
			playerControlledCameraTransformer.transformCameraFOV(currentFOVOffset, frameDelta);


				if (GetKeyState('L') & 0x8000)
				{
					PLOG_DEBUG << "y: " << playerControlledCameraTransformer.relativeCameraState.targetEulerYaw;
					PLOG_DEBUG << "p: " << playerControlledCameraTransformer.relativeCameraState.targetEulerPitch;
					PLOG_DEBUG << "r: " << playerControlledCameraTransformer.relativeCameraState.targetEulerRoll;
				}


;

			LOG_ONCE(PLOG_DEBUG << "transforming done, updating game camera");

			updateGameCameraData->updateGameCameraData(gameCameraData, freeCameraData, currentFOVOffset);

		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
			settingsWeak.lock()->freeCameraToggle->GetValueDisplay() = false;
			settingsWeak.lock()->freeCameraToggle->UpdateValueWithInput();
		}
	}



	// setting toggle event callback
	void onToggleChange(bool& newValue)
	{
		try
		{


			needToSetupCamera = true;
			PLOG_DEBUG << "onToggleChange: newval: " << newValue;

			// set hooks 
			{
				safetyhook::ThreadFreezer threadFreezer; 
				setCameraDataHook->setWantsToBeAttached(newValue);
				renderPlayerHook->setWantsToBeAttached(newValue);
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
		playerControlledCameraInput(resolveDependentCheat(CameraInputReader)),
		mFreeCameraToggleCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraToggle->valueChangedEvent, [this](bool& n) { onToggleChange(n); }),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); })
	{
		instance = this;
		auto ptr = dicon.Resolve<PointerManager>().lock();
		frameDeltaPointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(frameDeltaPointer), game);
		auto setCameraDataFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(setCameraDataFunction), game);
		setCameraDataHook = ModuleMidHook::make(game.toModuleName(), setCameraDataFunction, setCameraDataHookFunction);

		auto renderPlayerFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(renderPlayerFunction), game);
		renderPlayerFunctionFlagSetter = ptr->getData< std::shared_ptr< MidhookFlagInterpreter>>(nameof(renderPlayerFunctionFlagSetter), game);
		renderPlayerHook = ModuleMidHook::make(game.toModuleName(), renderPlayerFunction, renderPlayerHookFunction);

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