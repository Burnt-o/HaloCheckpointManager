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



	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<GetPlayerViewAngle> getPlayerViewAngleWeak;
	std::shared_ptr<GetGameCameraData> getGameCameraData;
	std::shared_ptr<UpdateGameCameraData> updateGameCameraData;

	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;


	std::shared_ptr<UserCameraInputReader> userCameraInputReader;
	PositionTransformer userControlledPosition;
	RotationTransformer userControlledRotation;
	FOVTransformer userControlledFOV;

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


			if (needToSetupCamera)
			{
				LOG_ONCE("Setting up user controlled camera");

				userControlledPosition = PositionTransformer(
					std::make_unique<LinearSmoother<SimpleMath::Vector3>>(0.06f), 
					userCameraInputReader,
					*gameCameraData.position
				);
				
				float initialYaw = std::atan2(gameCameraData.lookDirForward->y, gameCameraData.lookDirForward->x);
				float initialPitch = std::asin(gameCameraData.lookDirForward->z) * -1.f;
				float initialRoll = 0.f;

				PLOG_DEBUG << "Initial EulerYaw: " << initialYaw;
				PLOG_DEBUG << "Initial EulerPitch: " << initialPitch;
				PLOG_DEBUG << "Initial EulerRoll: " << initialRoll;
				
				userControlledRotation = RotationTransformer(
					std::make_unique<LinearSmoother<float>>(0.06f), 
					userCameraInputReader,
					initialYaw,
					initialPitch,
					initialRoll
				);

				userControlledFOV = FOVTransformer(
					std::make_unique<LinearSmoother<float>>(0.06f),
					userCameraInputReader,
					1.0f
				);


				needToSetupCamera = false;
			}

			float frameDelta;
			if (!frameDeltaPointer->readData(&frameDelta)) throw HCMRuntimeException("Could not resolve frameDeltaPointer");

			LOG_ONCE(PLOG_DEBUG << "reading done, transforming camera");

			// rotate FIRST so position updater knows which direction is "forward/up/right" etc 
			userControlledRotation.transformRotation(freeCameraData, frameDelta);
			userControlledPosition.transformPosition(freeCameraData, frameDelta);
			userControlledFOV.transformFOV(freeCameraData, frameDelta);

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
		userCameraInputReader(resolveDependentCheat(UserCameraInputReader)),
		mFreeCameraToggleCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->freeCameraToggle->valueChangedEvent, [this](bool& n) { onToggleChange(n); }),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { onGameStateChange(state); }),
		userControlledPosition (
			std::make_unique<LinearSmoother<SimpleMath::Vector3>>(0.06f),
			userCameraInputReader,
			{ 0,0,0 }
		),
		userControlledRotation (
			std::make_unique<LinearSmoother<float>>(0.06f),
			userCameraInputReader,
			0,
			0,
			0
		),
		userControlledFOV (
			std::make_unique<LinearSmoother<float>>(0.06f),
			userCameraInputReader,
			1.0f
		)
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