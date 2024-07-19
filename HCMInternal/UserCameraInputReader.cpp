#include "pch.h"
#include "UserCameraInputReader.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "IMCCStateHook.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"

#include "LinearSmoother.h"
#include "HotkeyDefinitions.h"

template <GameState::Value gameT>
class UserCameraInputReaderImpl : public IUserCameraInputReaderImpl
{
private:

	GameState mGame;

	// injected services
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	// hotkeys for input
	std::shared_ptr<RebindableHotkey> cameraTranslateUpBinding;
	std::shared_ptr<RebindableHotkey> cameraTranslateDownBinding;
	std::shared_ptr<RebindableHotkey> cameraRollLeftBinding;
	std::shared_ptr<RebindableHotkey> cameraRollRightBinding;
	std::shared_ptr<RebindableHotkey> cameraFOVIncreaseBinding;
	std::shared_ptr<RebindableHotkey> cameraFOVDecreaseBinding;


	// callbacks
	ScopedCallback< eventpp::CallbackList<void(float&)>> positionSpeedChangedCallback;
	ScopedCallback< eventpp::CallbackList<void(float&)>> rotationSpeedChangedCallback;
	ScopedCallback< eventpp::CallbackList<void(float&)>> FOVSpeedChangedCallback;
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;


	// data
	float mPositionSpeed;
	float mRotationSpeed;
	float mFOVSpeed;

	// MCC analog input pointer data
	std::shared_ptr<MultilevelPointer> analogMoveLeftRight;
	std::shared_ptr<MultilevelPointer> analogMoveForwardBack;
	std::shared_ptr<MultilevelPointer> analogTurnLeftRightMouse;
	std::shared_ptr<MultilevelPointer> analogTurnUpDownMouse;
	std::shared_ptr<MultilevelPointer> analogTurnLeftRightGamepad;
	std::shared_ptr<MultilevelPointer> analogTurnUpDownGamepad;
	std::shared_ptr<MultilevelPointer> isMouseInput;

	// cached of above
	float* cachedAnalogMoveLeftRight = nullptr;
	float* cachedAnalogMoveForwardBack = nullptr;
	float* cachedAnalogTurnLeftRightMouse = nullptr;
	float* cachedAnalogTurnUpDownMouse = nullptr; 
	float* cachedAnalogTurnLeftRightGamepad = nullptr;
	float* cachedAnalogTurnUpDownGamepad = nullptr;
	bool* cachedIsMouseInput = nullptr;

	bool cacheInitialised = false;

	// event to fire on mcc state change (just update caches)
	void updateCache()
	{

		LOG_ONCE(PLOG_DEBUG << "Updating cameraInputReader cache");

		cachedAnalogMoveLeftRight = nullptr;
		cachedAnalogMoveForwardBack = nullptr;
		cachedAnalogTurnLeftRightMouse = nullptr;
		cachedAnalogTurnUpDownMouse = nullptr;
		cachedAnalogTurnLeftRightGamepad = nullptr;
		cachedAnalogTurnUpDownGamepad = nullptr;
		cachedIsMouseInput = nullptr;
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

			LOG_ONCE(PLOG_DEBUG << "resolving cache");

			if (!analogMoveLeftRight->resolve((uintptr_t*)&cachedAnalogMoveLeftRight)) throw HCMRuntimeException(std::format("could not resolve analogMoveLeftRight: {}", MultilevelPointer::GetLastError()));
			if (!analogMoveForwardBack->resolve((uintptr_t*)&cachedAnalogMoveForwardBack)) throw HCMRuntimeException(std::format("could not resolve analogMoveForwardBack: {}", MultilevelPointer::GetLastError()));
			if (!analogTurnLeftRightMouse->resolve((uintptr_t*)&cachedAnalogTurnLeftRightMouse)) throw HCMRuntimeException(std::format("could not resolve analogTurnLeftRightMouse: {}", MultilevelPointer::GetLastError()));
			if (!analogTurnUpDownMouse->resolve((uintptr_t*)&cachedAnalogTurnUpDownMouse)) throw HCMRuntimeException(std::format("could not resolve analogTurnUpDownMouse: {}", MultilevelPointer::GetLastError()));
			if (!analogTurnLeftRightGamepad->resolve((uintptr_t*)&cachedAnalogTurnLeftRightGamepad)) throw HCMRuntimeException(std::format("could not resolve analogTurnLeftRightGamepad: {}", MultilevelPointer::GetLastError()));
			if (!analogTurnUpDownGamepad->resolve((uintptr_t*)&cachedAnalogTurnUpDownGamepad)) throw HCMRuntimeException(std::format("could not resolve analogTurnUpDownGamepad: {}", MultilevelPointer::GetLastError()));
			if (!isMouseInput->resolve((uintptr_t*)&cachedIsMouseInput)) throw HCMRuntimeException(std::format("could not resolve isMouseInput: {}", MultilevelPointer::GetLastError()));


		
			LOG_ONCE(PLOG_DEBUG << "cache resolved"); // can you tell I had an issue here
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onSpeedChange()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);

			mPositionSpeed = settings->freeCameraUserInputCameraTranslationSpeed->GetValue();
			mRotationSpeed = settings->freeCameraUserInputCameraRotationSpeed->GetValue();
			mFOVSpeed = settings->freeCameraUserInputCameraFOVSpeed->GetValue();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}



	float getMoveLeftRight()
	{
		return *cachedAnalogMoveLeftRight;

	}

	float getMoveForwardBackward()
	{
		return *cachedAnalogMoveForwardBack;
	}

	float getTurnLeftRight(const float frameDelta)
	{
		if (*cachedIsMouseInput)
			return *cachedAnalogTurnLeftRightMouse / frameDelta; 	// mouse movements are already multiplied by frameDelta, so we need to unmultiply them
		else
			return *cachedAnalogTurnLeftRightGamepad;
	}

	float getTurnUpDown(const float frameDelta)
	{
		if (*cachedIsMouseInput)
			return *cachedAnalogTurnUpDownMouse / frameDelta; 	// mouse movements are already multiplied by frameDelta, so we need to unmultiply them
		else
			return *cachedAnalogTurnUpDownGamepad;
	}

public:

	UserCameraInputReaderImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		positionSpeedChangedCallback(settingsWeak.lock()->freeCameraUserInputCameraTranslationSpeed->valueChangedEvent, [this](float& n) { onSpeedChange(); }),
		rotationSpeedChangedCallback(settingsWeak.lock()->freeCameraUserInputCameraRotationSpeed->valueChangedEvent, [this](float& n) { onSpeedChange(); }),
		FOVSpeedChangedCallback(settingsWeak.lock()->freeCameraUserInputCameraFOVSpeed->valueChangedEvent, [this](float& n) { onSpeedChange(); }),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { cacheInitialised = false; })
	{
		auto settings = settingsWeak.lock();
		mPositionSpeed = settings->freeCameraUserInputCameraTranslationSpeed->GetValue();
		mRotationSpeed = settings->freeCameraUserInputCameraRotationSpeed->GetValue();
		mFOVSpeed = settings->freeCameraUserInputCameraFOVSpeed->GetValue();


		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		analogMoveLeftRight = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(analogMoveLeftRight), game);
		analogMoveForwardBack = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(analogMoveForwardBack), game);
		analogTurnLeftRightMouse = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(analogTurnLeftRightMouse), game);
		analogTurnUpDownMouse = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(analogTurnUpDownMouse), game);
		analogTurnLeftRightGamepad = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(analogTurnLeftRightGamepad), game);
		analogTurnUpDownGamepad = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(analogTurnUpDownGamepad), game);
		isMouseInput = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(isMouseInput), game);



		auto& hkd = dicon.Resolve < HotkeyDefinitions>().lock()->getAllRebindableHotkeys();
		cameraTranslateUpBinding = hkd.at(RebindableHotkeyEnum::cameraTranslateUpBinding);
		cameraTranslateDownBinding = hkd.at(RebindableHotkeyEnum::cameraTranslateDownBinding);
		cameraRollLeftBinding = hkd.at(RebindableHotkeyEnum::cameraRollLeftBinding);
		cameraRollRightBinding = hkd.at(RebindableHotkeyEnum::cameraRollRightBinding);
		cameraFOVIncreaseBinding = hkd.at(RebindableHotkeyEnum::cameraFOVIncreaseBinding);
		cameraFOVDecreaseBinding = hkd.at(RebindableHotkeyEnum::cameraFOVDecreaseBinding);

	}

	void updatePositionTransform(const FreeCameraData& freeCameraData, const float frameDelta, SimpleMath::Vector3& positionTransform);

	void updateRotationTransform(const FreeCameraData& freeCameraData, const float frameDelta, float& eulerYaw, float& eulerPitch, float& eulerRoll);

	void updateFOVTransform(const FreeCameraData& freeCameraData, const float frameDelta, float& fov);
};



template <GameState::Value gameT>
void UserCameraInputReaderImpl<gameT>::updatePositionTransform(const FreeCameraData& freeCameraData, const float frameDelta, SimpleMath::Vector3& positionTransform)
{
	LOG_ONCE(PLOG_DEBUG << "readPositionInput");

	if (cacheInitialised == false)
	{
		updateCache();
		cacheInitialised = true;
	}

	lockOrThrow(settingsWeak, settings);
	if (settings->freeCameraCameraInputDisable->GetValue()) return;

	// Section: Translation
	float cameraTranslationSpeed = mPositionSpeed; 
	if constexpr (gameT == GameState::Value::Halo1 || gameT == GameState::Value::Halo2)
	{
		// I'm not sure why third gen doesn't behave properly with this.
		cameraTranslationSpeed = cameraTranslationSpeed * frameDelta;
	}
	else
	{
		cameraTranslationSpeed = cameraTranslationSpeed * 0.002f; // otherwise it's way too fast
	}

	

	if (cachedAnalogMoveLeftRight == nullptr || cachedAnalogMoveForwardBack == nullptr) throw HCMRuntimeException("null cachedAnalogMove pointers!");


	// Note: for this code to work properly, the freeCameraData lookDirs must be already updated this frame. 
	// In other words, call readRotationInput & transformRotations BEFORE reading position input, so that this code knows which way is "forward" when you press W, etc.

	// Any inputs this frame?
	if (getMoveForwardBackward() != 0.f || getMoveLeftRight() != 0.f || cameraTranslateUpBinding->isCurrentlyDown() || cameraTranslateDownBinding->isCurrentlyDown())
	{
		// To normalise inputs, what we need to do is map a unit cube of inputs [-1...1] to a unit sphere.
		float xCube = getMoveForwardBackward();
		float yCube = getMoveLeftRight();
		float zCube = cameraTranslateUpBinding->isCurrentlyDown() ? 1.f : (cameraTranslateDownBinding->isCurrentlyDown() ? -1.f : 0.f);

		//https://mathproofs.blogspot.com/2005/07/mapping-cube-to-sphere.html?m=1
		// precalc squared values
		float xCubeSquared = xCube * xCube;
		float yCubeSquared = yCube * yCube;
		float zCubeSquared = zCube * zCube;

		// map to sphere
		auto xSphere = xCube * std::sqrtf(1 - (yCubeSquared / 2) - (zCubeSquared / 2) + ((yCubeSquared * zCubeSquared) / 3));
		auto ySphere = yCube * std::sqrtf(1 - (zCubeSquared / 2) - (xCubeSquared / 2) + ((zCubeSquared * xCubeSquared) / 3));
		auto zSphere = zCube * std::sqrtf(1 - (xCubeSquared / 2) - (yCubeSquared / 2) + ((xCubeSquared * yCubeSquared) / 3));

		// Forward/back
		positionTransform = positionTransform + (freeCameraData.currentlookDirForward * cameraTranslationSpeed * xSphere);
		// Left/Right
		positionTransform = positionTransform + (freeCameraData.currentlookDirRight * cameraTranslationSpeed * ySphere);
		// Up/Down
		positionTransform = positionTransform + (freeCameraData.currentlookDirUp * cameraTranslationSpeed * zSphere);
	}


}

template <GameState::Value gameT>
void UserCameraInputReaderImpl<gameT>::updateRotationTransform(const FreeCameraData& freeCameraData, const float frameDelta, float& eulerYaw, float& eulerPitch, float& eulerRoll)
{
	LOG_ONCE(PLOG_DEBUG << "readRotationInput");

	if (cacheInitialised == false)
	{
		updateCache();
		cacheInitialised = true;
	}

	lockOrThrow(settingsWeak, settings);
	if (settings->freeCameraCameraInputDisable->GetValue()) return;

	// Section: Rotation
	auto analogRotationSpeed = frameDelta * mRotationSpeed;
	auto digitalRotationSpeed = frameDelta * mRotationSpeed;

	if (cachedAnalogTurnLeftRightMouse == nullptr || cachedAnalogTurnUpDownMouse == nullptr || cachedAnalogTurnLeftRightGamepad == nullptr || cachedAnalogTurnUpDownGamepad == nullptr || cachedIsMouseInput == nullptr) throw HCMRuntimeException("null cachedAnalogTurn pointers!");


	float leftRightTurnAmount = getTurnLeftRight(frameDelta);
	float upDownTurnAmount = getTurnUpDown(frameDelta);



	// Yaw: rotation around WORLD up
	if (leftRightTurnAmount != 0.f)
	{
		// some trig to make it instead pitch if camera is rolled perpindicular to horizon.
		// when roll is zero : left yaws left. (duh)
		// when roll is 90 dir right (up is to the left): left pitches up.
		// when roll is 180 dir right (up is to the bottom): left yaws right.
		// when roll is 270 dir right (up is to the right): left pitches down.
		// https://www.geogebra.org/m/aavMVjyK sin and cos will give us the multipliers we need


		float yawProportion = std::cos(eulerRoll); // hmmm
		float pitchProportion = std::sin(eulerRoll) * -1.f;

		eulerYaw = (eulerYaw + (analogRotationSpeed * leftRightTurnAmount * yawProportion));
		eulerPitch = (eulerPitch + (analogRotationSpeed * leftRightTurnAmount * pitchProportion));
	}



	// Pitch: rotation around local right
	if (upDownTurnAmount != 0.f)
	{
		// some trig to make it instead yaw if camera is rolled perpindicular to horizon.
		// when roll is zero : up pitches up. (duh)
		// when roll is 90 dir right (up is to the left): up yaws right.
		// when roll is 180 dir right (up is to the bottom): up pitches down.
		// when roll is 270 dir right (up is to the right): up yaws left
		// https://www.geogebra.org/m/aavMVjyK sin and cos will give us the multipliers we need

		float pitchProportion = std::cos(eulerRoll);
		float yawProportion = std::sin(eulerRoll);

		eulerPitch = (eulerPitch + (analogRotationSpeed * upDownTurnAmount * -1.f * pitchProportion));
		eulerYaw = (eulerYaw + (analogRotationSpeed * upDownTurnAmount * -1.f * yawProportion));

	}

	// Roll: means rotating around local FORWARD axis.
	if (cameraRollLeftBinding->isCurrentlyDown())
	{
		eulerRoll = eulerRoll + digitalRotationSpeed * -1.f;
	}

	if (cameraRollRightBinding->isCurrentlyDown())
	{
		eulerRoll = eulerRoll + (digitalRotationSpeed);
	}




}

template <GameState::Value gameT>
void UserCameraInputReaderImpl<gameT>::updateFOVTransform(const FreeCameraData& freeCameraData, const float frameDelta, float& fov)
{
	LOG_ONCE(PLOG_DEBUG << "readFOVInput");

	if (cacheInitialised == false)
	{
		updateCache();
		cacheInitialised = true;
	}



	lockOrThrow(settingsWeak, settings);
	if (settings->freeCameraCameraInputDisable->GetValue()) return;

	float cameraFOVSpeed = mFOVSpeed;;
	if constexpr (gameT == GameState::Value::Halo1 || gameT == GameState::Value::Halo2)
	{
		// I'm not sure why third gen doesn't behave properly with this.
		cameraFOVSpeed = cameraFOVSpeed * frameDelta;
	}
	else
	{
		cameraFOVSpeed = cameraFOVSpeed * 0.002f; // otherwise it's way too fast
	}

	// Section: FOV
	// we scale by power for smoother transition
	// increase
	float scaleFactor = std::sqrt(fov) * 10.f;
	if (cameraFOVIncreaseBinding->isCurrentlyDown())
	{
		fov = std::clamp(fov + (cameraFOVSpeed * scaleFactor), 0.0001f, 179.999f);
	}

	// decrease
	if (cameraFOVDecreaseBinding->isCurrentlyDown())
	{
		fov = std::clamp(fov - (cameraFOVSpeed * scaleFactor), 0.0001f, 179.999f);
	}
}

 


UserCameraInputReader::UserCameraInputReader(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<UserCameraInputReaderImpl<GameState::Value::Halo1>>(game, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<UserCameraInputReaderImpl<GameState::Value::Halo2>>(game, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<UserCameraInputReaderImpl<GameState::Value::Halo3>>(game, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<UserCameraInputReaderImpl<GameState::Value::Halo3ODST>>(game, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<UserCameraInputReaderImpl<GameState::Value::HaloReach>>(game, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<UserCameraInputReaderImpl<GameState::Value::Halo4>>(game, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

UserCameraInputReader::~UserCameraInputReader()
{
	PLOG_DEBUG << "~" << getName();
}

void UserCameraInputReader::updatePositionTransform(const FreeCameraData& freeCameraData, const float frameDelta, SimpleMath::Vector3& positionTransform)
{
	return pimpl->updatePositionTransform(freeCameraData, frameDelta, positionTransform);
}

void UserCameraInputReader::updateRotationTransform(const FreeCameraData& freeCameraData, const float frameDelta, float& eulerYaw, float& eulerPitch, float& eulerRoll)
{
	return pimpl->updateRotationTransform(freeCameraData, frameDelta, eulerYaw, eulerPitch, eulerRoll);
}

void UserCameraInputReader::updateFOVTransform(const FreeCameraData& freeCameraData, const float frameDelta, float& fov)
{
	return pimpl->updateFOVTransform(freeCameraData, frameDelta, fov);
}