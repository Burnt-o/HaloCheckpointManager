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


	// add from inputs
	if (*cachedAnalogMoveForwardBack != 0.f)
	{
		relativeCameraState.targetPosition = relativeCameraState.targetPosition + (relativeCameraState.currentlookDirForward * cameraTranslationSpeed * *cachedAnalogMoveForwardBack);
	}

	if (*cachedAnalogMoveLeftRight != 0.f)
	{
		//auto currentRightDir = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitY, relativeCameraState.currentLookQuat);
		relativeCameraState.targetPosition = relativeCameraState.targetPosition + (relativeCameraState.currentlookDirRight * cameraTranslationSpeed * *cachedAnalogMoveLeftRight);
	}


	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		//auto currentUp = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitZ, relativeCameraState.currentLookQuat);
		relativeCameraState.targetPosition = relativeCameraState.targetPosition + (relativeCameraState.currentlookDirUp * cameraTranslationSpeed);
	}

	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		//auto currentUp = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitZ, relativeCameraState.currentLookQuat);
		relativeCameraState.targetPosition = relativeCameraState.targetPosition - (relativeCameraState.currentlookDirUp * cameraTranslationSpeed);
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


	bool needToApplyRotation = false;
	SimpleMath::Quaternion quatYaw = SimpleMath::Quaternion::Identity;
	SimpleMath::Quaternion quatPitch = SimpleMath::Quaternion::Identity;
	SimpleMath::Quaternion quatRoll = SimpleMath::Quaternion::Identity;



	SimpleMath::Vector3 yawPitchRoll = SimpleMath::Vector3::Zero;

	// Yaw: means rotating around world UP axis. Rotating around world avoids inducing roll.
	if (*cachedAnalogTurnLeftRight != 0.f)
	{
		needToApplyRotation = true;
		//quatYaw = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirUp, analogRotationSpeed * *cachedAnalogTurnLeftRight);
		//yawPitchRoll.x = analogRotationSpeed * *cachedAnalogTurnLeftRight;

		quatYaw = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirUp, analogRotationSpeed * *cachedAnalogTurnLeftRight);


	}



	// Pitch: means rotating around local RIGHT axis. 
	if (*cachedAnalogTurnUpDown != 0.f)
	{
		needToApplyRotation = true;
		//auto currentRight = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, relativeCameraState.currentLookQuat);
		//currentRight = SimpleMath::Vector3::Transform(currentRight, SimpleMath::Quaternion::CreateFromAxisAngle(SimpleMath::Vector3::UnitZ, DirectX::XM_PIDIV2));
		//quatPitch = SimpleMath::Quaternion::CreateFromAxisAngle(currentRight, analogRotationSpeed * *cachedAnalogTurnUpDown);

		//yawPitchRoll.y = analogRotationSpeed * *cachedAnalogTurnUpDown;
		quatPitch = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.currentlookDirRight, analogRotationSpeed * *cachedAnalogTurnUpDown * -1.f);

		//auto rotQuat = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirRight, cameraRotationSpeed * *cachedAnalogTurnUpDown);
		//relativeCameraState.targetLookQuat = rotQuat * relativeCameraState.targetLookQuat;
	}

	// Roll: means rotating around local FORWARD axis.
	if (GetKeyState('G') & 0x8000)
	{
		needToApplyRotation = true;
		//auto currentForward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, relativeCameraState.currentLookQuat);
		//quatRoll = SimpleMath::Quaternion::CreateFromAxisAngle(currentForward, digitalRotationSpeed);

		quatRoll = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.currentlookDirForward, digitalRotationSpeed);

		//yawPitchRoll.z = digitalRotationSpeed;

		//auto rotQuat = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirForward, cameraRotationSpeed);
		//relativeCameraState.targetLookQuat = rotQuat * relativeCameraState.targetLookQuat;
	}

	if (GetKeyState('T') & 0x8000)
	{
		needToApplyRotation = true;
		//auto currentForward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, relativeCameraState.currentLookQuat);
		//quatRoll = SimpleMath::Quaternion::CreateFromAxisAngle(currentForward, digitalRotationSpeed * -1.f);

		quatRoll = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.currentlookDirForward, digitalRotationSpeed * -1.f);
		//auto rotQuat = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.currentlookDirForward, cameraRotationSpeed * -1.f);
		//relativeCameraState.targetLookQuat = rotQuat * relativeCameraState.targetLookQuat;
	}


	if (needToApplyRotation)
	{

		// shit this was "working" when dirUp was transformed by pitch * yaw. breaks with "correct" pitch and roll. hm.

		SimpleMath::Matrix basis = SimpleMath::Matrix::Matrix( relativeCameraState.targetlookDirRight, relativeCameraState.targetlookDirUp, relativeCameraState.targetlookDirForward);

		basis = SimpleMath::Matrix::Transform(basis, quatPitch * quatYaw * quatRoll);

		relativeCameraState.targetlookDirForward = basis.Backward();
		relativeCameraState.targetlookDirUp = basis.Down();
		relativeCameraState.targetlookDirRight = basis.Left();

		//relativeCameraState.targetlookDirForward = SimpleMath::Vector3::Transform(relativeCameraState.targetlookDirForward, quatPitch * quatYaw);
		//relativeCameraState.targetlookDirUp = SimpleMath::Vector3::Transform(relativeCameraState.targetlookDirUp, quatPitch * quatRoll);
		//relativeCameraState.targetlookDirRight = SimpleMath::Vector3::Transform(relativeCameraState.targetlookDirForward, SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.targetlookDirUp, DirectX::XM_PIDIV2));

		// need to orthonormalise to prevent error accumlation
		relativeCameraState.targetlookDirRight.Normalize();
		relativeCameraState.targetlookDirUp = relativeCameraState.targetlookDirForward.Cross(relativeCameraState.targetlookDirRight);
		relativeCameraState.targetlookDirUp.Normalize();
		relativeCameraState.targetlookDirForward = relativeCameraState.targetlookDirRight.Cross(relativeCameraState.targetlookDirUp);
		relativeCameraState.targetlookDirForward.Normalize();

		//relativeCameraState.targetLookQuat = relativeCameraState.targetLookQuat * quatYaw * quatPitch  * quatRoll; // the order matters. do pitch then yaw then roll.

	}



	//static_assert(false, "READ THIS");
	//https://gamedev.stackexchange.com/questions/129445/pitch-yaw-in-circle-causes-undesired-tilt
	/*
	Your code is allowing errors to accumulate in the up vector. Try replacing right = cross(forward, up) with right = normalized(cross(forward, Vector3(0, 1, 0))) if you don't need to look directly up/down. If you're using up to construct your matrix then you'll want to update it when yawing too. – 
	You're using the previous value of the up vector to update the right vector and the up vector itself. That means any small deviation from the vertical will tend to accumulate in the up vector and snowball over repeated updates.
	


	*/



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