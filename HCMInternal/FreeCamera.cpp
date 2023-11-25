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
	std::shared_ptr<GetGameCameraData> getGameCameraData;
	std::shared_ptr<UpdateGameCameraData> updateGameCameraData;
	//std::shared_ptr<ObjectAnchoredFreeCam> objectAnchoredFreeCam;
	//std::shared_ptr<PlayerControlledFreeCamera> playerControlledFreeCamera;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	CameraTransformer playerControlledCameraTransformer = CameraTransformer(
		std::make_shared<LinearSmoother<SimpleMath::Vector3>>(0.9f),
		std::make_shared<LinearSmoother<SimpleMath::Quaternion>>(0.6f),
		std::make_shared<LinearSmoother<float>>(0.85f)
	);

	std::shared_ptr<CameraInputReader> playerControlledCameraInput;

	CameraTransformer objectAnchoredCameraTransformer = CameraTransformer(
		std::make_shared<LinearSmoother<SimpleMath::Vector3>>(0.9f),
		std::make_shared<LinearSmoother<SimpleMath::Quaternion>>(0.6f),
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


			static int debugWorldLook = 0;

			if (GetKeyState('O') & 0x8000)
			{
				debugWorldLook++;
				debugWorldLook = debugWorldLook % 7;
				PLOG_DEBUG << "debugWorldLook: " << debugWorldLook;
				Sleep(250);
			}

			switch (debugWorldLook)
			{
			case 1: 
				freeCameraData.currentlookDirForward = SimpleMath::Vector3::UnitY * -1.f;
				freeCameraData.currentlookDirRight = SimpleMath::Vector3::UnitX;
				freeCameraData.currentlookDirUp = SimpleMath::Vector3::UnitZ;
				break;


			case 0:
				freeCameraData.currentlookDirForward = SimpleMath::Vector3::UnitX;
				freeCameraData.currentlookDirRight = SimpleMath::Vector3::UnitY;
				freeCameraData.currentlookDirUp = SimpleMath::Vector3::UnitZ;
				break;

			case 2:
				freeCameraData.currentlookDirForward = SimpleMath::Vector3::UnitX;
				freeCameraData.currentlookDirRight = SimpleMath::Vector3::UnitZ;
				freeCameraData.currentlookDirUp = SimpleMath::Vector3::UnitY;
				break;

			case 3:  // default
				freeCameraData.currentlookDirForward = SimpleMath::Vector3::UnitY;
				freeCameraData.currentlookDirRight = SimpleMath::Vector3::UnitX;
				freeCameraData.currentlookDirUp = SimpleMath::Vector3::UnitZ;
				break;

			case 4:
				freeCameraData.currentlookDirForward = SimpleMath::Vector3::UnitY;
				freeCameraData.currentlookDirRight = SimpleMath::Vector3::UnitZ;
				freeCameraData.currentlookDirUp = SimpleMath::Vector3::UnitX;
				break;

			case 5:
				freeCameraData.currentlookDirForward = SimpleMath::Vector3::UnitZ;
				freeCameraData.currentlookDirRight = SimpleMath::Vector3::UnitY;
				freeCameraData.currentlookDirUp = SimpleMath::Vector3::UnitX;
				break;

			case 6:
				freeCameraData.currentlookDirForward = SimpleMath::Vector3::UnitZ;
				freeCameraData.currentlookDirRight = SimpleMath::Vector3::UnitX;
				freeCameraData.currentlookDirUp = SimpleMath::Vector3::UnitY;
				break;


			}



			static int debugWorldLookSign = 0;

			if (GetKeyState('I') & 0x8000)
			{
				debugWorldLookSign++;
				debugWorldLookSign = debugWorldLookSign % 8;
				PLOG_DEBUG << "debugWorldLookSign: " << debugWorldLookSign;
				Sleep(250);
			}


#define FLIPA freeCameraData.currentlookDirForward = freeCameraData.currentlookDirForward * -1.f;
#define FLIPB freeCameraData.currentlookDirRight = freeCameraData.currentlookDirRight * -1.f;
#define FLIPC freeCameraData.currentlookDirUp = freeCameraData.currentlookDirUp * -1.f;

			switch (debugWorldLookSign)
			{
			case 0:
				break;

			case 1:
				FLIPA;
				break;

			case 2:
				FLIPB;
				break;

			case 3:
				FLIPC;
				break;

			case 4:
				FLIPA;
				FLIPB;
				break;

			case 5:
				FLIPA;
				FLIPC;
				break;

			case 6:
				FLIPB;
				FLIPC;
				break;

			case 7:
				FLIPA;
				FLIPB;
				FLIPC;
				break;
			}


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
				playerControlledCameraTransformer.relativeCameraState.targetlookDirForward = *gameCameraData.lookDirForward;
				playerControlledCameraTransformer.relativeCameraState.targetlookDirUp = *gameCameraData.lookDirUp;

				SimpleMath::Vector3 myUp = playerControlledCameraTransformer.relativeCameraState.targetlookDirUp;


				playerControlledCameraTransformer.relativeCameraState.targetlookDirRight = SimpleMath::Vector3::Transform(playerControlledCameraTransformer.relativeCameraState.targetlookDirForward, SimpleMath::Quaternion::CreateFromAxisAngle(myUp, DirectX::XM_PIDIV2));
				playerControlledCameraTransformer.relativeCameraState.targetPosition = *gameCameraData.position;
				playerControlledCameraTransformer.relativeCameraState.targetFOVOffset = 0;
					


				SimpleMath::Matrix dirMatrix = SimpleMath::Matrix(
					// You'd think this would be forward->right->up, but since game uses YXZ order and I prefer XYZ, we do right->backward->up to convert
					playerControlledCameraTransformer.relativeCameraState.targetlookDirForward,
					playerControlledCameraTransformer.relativeCameraState.targetlookDirRight,
					playerControlledCameraTransformer.relativeCameraState.targetlookDirUp
				);


				playerControlledCameraTransformer.relativeCameraState.targetLookQuat = SimpleMath::Quaternion::CreateFromRotationMatrix(dirMatrix);

				// set current value to.. current value
				playerControlledCameraTransformer.relativeCameraState.currentlookDirForward = playerControlledCameraTransformer.relativeCameraState.targetlookDirForward;
				playerControlledCameraTransformer.relativeCameraState.currentlookDirRight = playerControlledCameraTransformer.relativeCameraState.targetlookDirRight;
				playerControlledCameraTransformer.relativeCameraState.currentlookDirUp = playerControlledCameraTransformer.relativeCameraState.targetlookDirUp;
				playerControlledCameraTransformer.relativeCameraState.currentLookQuat = playerControlledCameraTransformer.relativeCameraState.targetLookQuat;
				playerControlledCameraTransformer.relativeCameraState.currentFOVOffset = 0;
				playerControlledCameraTransformer.relativeCameraState.currentPosition = playerControlledCameraTransformer.relativeCameraState.targetPosition;




				needToSetupCamera = false;
			}

			float frameDelta;
			if (!frameDeltaPointer->readData(&frameDelta)) throw HCMRuntimeException("Could not resolve frameDeltaPointer");

			playerControlledCameraInput->readPositionInput(playerControlledCameraTransformer.relativeCameraState, freeCameraData,  frameDelta);
			playerControlledCameraInput->readRotationInput(playerControlledCameraTransformer.relativeCameraState, freeCameraData, frameDelta);
			playerControlledCameraInput->readFOVInput(playerControlledCameraTransformer.relativeCameraState, freeCameraData, frameDelta);

			LOG_ONCE(PLOG_DEBUG << "reading done, transforming camera");

			playerControlledCameraTransformer.transformCameraPosition(freeCameraData, frameDelta);
			playerControlledCameraTransformer.transformCameraRotation(freeCameraData, frameDelta);
			playerControlledCameraTransformer.transformCameraFOV(currentFOVOffset, frameDelta);

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