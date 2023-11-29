#include "pch.h"
#include "CameraInputReader.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "IMCCStateHook.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"


class CameraInputReader::CameraInputReaderImpl
{
private:

	GameState mGame;

	// injected services
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	// callbacks
	ScopedCallback< eventpp::CallbackList<void(float&)>> positionSpeedChangedCallback;
	ScopedCallback< eventpp::CallbackList<void(float&)>> rotationSpeedChangedCallback;
	ScopedCallback< eventpp::CallbackList<void(float&)>> FOVSpeedChangedCallback;
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;


	// data
	float mPositionSpeed;
	float mRotationSpeed;
	float mFOVSpeed;

	std::shared_ptr<MultilevelPointer> analogMoveLeftRight;
	std::shared_ptr<MultilevelPointer> analogMoveForwardBack;
	std::shared_ptr<MultilevelPointer> analogTurnLeftRight;
	std::shared_ptr<MultilevelPointer> analogTurnUpDown;
	std::shared_ptr<MultilevelPointer> isMouseInput;

	float* cachedAnalogMoveLeftRight = nullptr;
	float* cachedAnalogMoveForwardBack = nullptr;
	float* cachedAnalogTurnLeftRight = nullptr;
	float* cachedAnalogTurnUpDown = nullptr; 
	bool* cachedIsMouseInput = nullptr;

	bool cacheInitialised = false;

	// event to fire on mcc state change (just update caches)
	void updateCache()
	{

		LOG_ONCE(PLOG_DEBUG << "Updating cameraInputReader cache");

		cachedAnalogMoveLeftRight = nullptr;
		cachedAnalogMoveForwardBack = nullptr;
		cachedAnalogTurnLeftRight = nullptr;
		cachedAnalogTurnUpDown = nullptr;

		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

			LOG_ONCE(PLOG_DEBUG << "resolving cache");

			if (!analogMoveLeftRight->resolve((uintptr_t*)&cachedAnalogMoveLeftRight)) throw HCMRuntimeException(std::format("could not resolve analogMoveLeftRight: {}", MultilevelPointer::GetLastError()));
			if (!analogMoveForwardBack->resolve((uintptr_t*)&cachedAnalogMoveForwardBack)) throw HCMRuntimeException(std::format("could not resolve analogMoveForwardBack: {}", MultilevelPointer::GetLastError()));
			if (!analogTurnLeftRight->resolve((uintptr_t*)&cachedAnalogTurnLeftRight)) throw HCMRuntimeException(std::format("could not resolve analogTurnLeftRight: {}", MultilevelPointer::GetLastError()));
			if (!analogTurnUpDown->resolve((uintptr_t*)&cachedAnalogTurnUpDown)) throw HCMRuntimeException(std::format("could not resolve analogTurnUpDown: {}", MultilevelPointer::GetLastError()));
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

			mPositionSpeed = settings->freeCameraPositionSpeed->GetValue();
			mRotationSpeed = settings->freeCameraRotationSpeed->GetValue();
			mFOVSpeed = settings->freeCameraFOVSpeed->GetValue();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}


public:

	CameraInputReaderImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		positionSpeedChangedCallback(settingsWeak.lock()->freeCameraPositionSpeed->valueChangedEvent, [this](float& n) { onSpeedChange(); }),
		rotationSpeedChangedCallback(settingsWeak.lock()->freeCameraRotationSpeed->valueChangedEvent, [this](float& n) { onSpeedChange(); }),
		FOVSpeedChangedCallback(settingsWeak.lock()->freeCameraFOVSpeed->valueChangedEvent, [this](float& n) { onSpeedChange(); }),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& state) { cacheInitialised = false; })
	{
		auto settings = settingsWeak.lock();
		mPositionSpeed = settings->freeCameraPositionSpeed->GetValue();
		mRotationSpeed = settings->freeCameraRotationSpeed->GetValue();
		mFOVSpeed = settings->freeCameraFOVSpeed->GetValue();


		auto ptr = dicon.Resolve<PointerManager>().lock();

		analogMoveLeftRight = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(analogMoveLeftRight), game);
		analogMoveForwardBack = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(analogMoveForwardBack), game);
		analogTurnLeftRight = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(analogTurnLeftRight), game);
		analogTurnUpDown = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(analogTurnUpDown), game);
		isMouseInput = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(isMouseInput), game);


	}

	void readPositionInput(RelativeCameraState& relativeCameraState, FreeCameraData& freeCameraData, float frameDelta);

	void readRotationInput(RelativeCameraState& relativeCameraState, FreeCameraData& freeCameraData, float frameDelta);

	void readFOVInput(RelativeCameraState& relativeCameraState, FreeCameraData& freeCameraData, float frameDelta);
};




void CameraInputReader::CameraInputReaderImpl::readPositionInput(RelativeCameraState& relativeCameraState, FreeCameraData& freeCameraData, float frameDelta)
{
	LOG_ONCE(PLOG_DEBUG << "readPositionInput");

	if (cacheInitialised == false)
	{
		updateCache();
		cacheInitialised = true;
	}

	// Section: Translation
	auto cameraTranslationSpeed = frameDelta * mPositionSpeed;

	if (cachedAnalogMoveLeftRight == nullptr || cachedAnalogMoveForwardBack == nullptr) throw HCMRuntimeException("null cachedAnalogMove pointers!");


	// Note: for this code to work properly, the freeCameraData lookDirs must be already updated this frame. 
	// In other words, call readRotationInput & transformRotations BEFORE reading position input, so that this code knows which way is "forward" when you press W, etc.

	// Forward/back
	if (*cachedAnalogMoveForwardBack != 0.f)
	{
		relativeCameraState.targetPositionTransformation = relativeCameraState.targetPositionTransformation + (freeCameraData.currentlookDirForward * cameraTranslationSpeed * *cachedAnalogMoveForwardBack);
	}

	// Left/Right
	if (*cachedAnalogMoveLeftRight != 0.f)
	{
		relativeCameraState.targetPositionTransformation = relativeCameraState.targetPositionTransformation + (freeCameraData.currentlookDirRight * cameraTranslationSpeed * *cachedAnalogMoveLeftRight);
	}

	// Up
	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		relativeCameraState.targetPositionTransformation = relativeCameraState.targetPositionTransformation + (freeCameraData.currentlookDirUp * cameraTranslationSpeed);
	}

	// Down
	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		relativeCameraState.targetPositionTransformation = relativeCameraState.targetPositionTransformation - (freeCameraData.currentlookDirUp * cameraTranslationSpeed * -1.f);
	}
	

}

void CameraInputReader::CameraInputReaderImpl::readRotationInput(RelativeCameraState& relativeCameraState, FreeCameraData& freeCameraData, float frameDelta)
{
	LOG_ONCE(PLOG_DEBUG << "readRotationInput");

	if (cacheInitialised == false)
	{
		updateCache();
		cacheInitialised = true;
	}

	// Section: Rotation
	auto analogRotationSpeed = frameDelta * mRotationSpeed;
	auto digitalRotationSpeed = frameDelta * mRotationSpeed;

	if (cachedAnalogTurnLeftRight == nullptr || cachedAnalogTurnUpDown == nullptr || cachedIsMouseInput == nullptr) throw HCMRuntimeException("null cachedAnalogTurn pointers!");

	if (*cachedIsMouseInput == true)
	{
		analogRotationSpeed = analogRotationSpeed * 50.f;
	}




	// Yaw: rotation around WORLD up
	if (*cachedAnalogTurnLeftRight != 0.f)
	{
		// some trig to make it instead pitch if camera is rolled perpindicular to horizon.
		// when roll is zero : left yaws left. (duh)
		// when roll is 90 dir right (up is to the left): left pitches up.
		// when roll is 180 dir right (up is to the bottom): left yaws right.
		// when roll is 270 dir right (up is to the right): left pitches down.
		// https://www.geogebra.org/m/aavMVjyK sin and cos will give us the multipliers we need


		float yawProportion = std::cos(relativeCameraState.targetEulerRoll); // hmmm
		float pitchProportion = std::sin(relativeCameraState.targetEulerRoll) * -1.f;

		relativeCameraState.targetEulerYaw = (relativeCameraState.targetEulerYaw + (analogRotationSpeed * *cachedAnalogTurnLeftRight * yawProportion));
		relativeCameraState.targetEulerPitch = (relativeCameraState.targetEulerPitch + (analogRotationSpeed * *cachedAnalogTurnLeftRight * pitchProportion));
	}



	// Pitch: rotation around local right
	if (*cachedAnalogTurnUpDown != 0.f)
	{
		// some trig to make it instead yaw if camera is rolled perpindicular to horizon.
		// when roll is zero : up pitches up. (duh)
		// when roll is 90 dir right (up is to the left): up yaws right.
		// when roll is 180 dir right (up is to the bottom): up pitches down.
		// when roll is 270 dir right (up is to the right): up yaws left
		// https://www.geogebra.org/m/aavMVjyK sin and cos will give us the multipliers we need

		float pitchProportion = std::cos(relativeCameraState.targetEulerRoll);
		float yawProportion = std::sin(relativeCameraState.targetEulerRoll);

		relativeCameraState.targetEulerPitch = (relativeCameraState.targetEulerPitch + (analogRotationSpeed * *cachedAnalogTurnUpDown * -1.f * pitchProportion));
		relativeCameraState.targetEulerYaw = (relativeCameraState.targetEulerYaw + (analogRotationSpeed * *cachedAnalogTurnUpDown * -1.f * yawProportion));

	}

	// Roll: means rotating around local FORWARD axis.
	if (GetKeyState('G') & 0x8000)
	{
		relativeCameraState.targetEulerRoll = relativeCameraState.targetEulerRoll + digitalRotationSpeed;
	}

	if (GetKeyState('T') & 0x8000)
	{
		relativeCameraState.targetEulerRoll = relativeCameraState.targetEulerRoll + (digitalRotationSpeed * -1.f);
	}




}

void CameraInputReader::CameraInputReaderImpl::readFOVInput(RelativeCameraState& relativeCameraState, FreeCameraData& freeCameraData, float frameDelta)
{
	LOG_ONCE(PLOG_DEBUG << "readFOVInput");

	if (cacheInitialised == false)
	{
		updateCache();
		cacheInitialised = true;
	}

	float cameraFOVSpeed = mFOVSpeed * frameDelta;

	// Section: FOV
	// increase
	if (GetKeyState(VK_NUMPAD1) & 0x8000)
	{
		relativeCameraState.targetFOVOffset = relativeCameraState.targetFOVOffset + cameraFOVSpeed;
	}

	// decrease
	if (GetKeyState(VK_NUMPAD3) & 0x8000)
	{
		relativeCameraState.targetFOVOffset = relativeCameraState.targetFOVOffset - cameraFOVSpeed;
	}
}




CameraInputReader::CameraInputReader(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique<CameraInputReaderImpl>(game, dicon);
}

CameraInputReader::~CameraInputReader()
{
	PLOG_DEBUG << "~" << getName();
}

void CameraInputReader::readPositionInput(RelativeCameraState& relativeCameraState, FreeCameraData& freeCameraData, float frameDelta)
{
	return pimpl->readPositionInput(relativeCameraState, freeCameraData, frameDelta);
}

void CameraInputReader::readRotationInput(RelativeCameraState& relativeCameraState, FreeCameraData& freeCameraData, float frameDelta)
{
	return pimpl->readRotationInput(relativeCameraState, freeCameraData, frameDelta);
}

void CameraInputReader::readFOVInput(RelativeCameraState& relativeCameraState, FreeCameraData& freeCameraData, float frameDelta)
{
	return pimpl->readFOVInput(relativeCameraState, freeCameraData, frameDelta);
}