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
		auto forward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, relativeCameraState.currentRotationTransformation);

		relativeCameraState.targetPositionTransformation = relativeCameraState.targetPositionTransformation + (forward * cameraTranslationSpeed * *cachedAnalogMoveForwardBack);
	}

	if (*cachedAnalogMoveLeftRight != 0.f)
	{
		auto right = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirRight, relativeCameraState.currentRotationTransformation);
		relativeCameraState.targetPositionTransformation = relativeCameraState.targetPositionTransformation + (right * cameraTranslationSpeed * *cachedAnalogMoveLeftRight);
	}


	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		auto up = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirUp, relativeCameraState.currentRotationTransformation);
		relativeCameraState.targetPositionTransformation = relativeCameraState.targetPositionTransformation + (up * cameraTranslationSpeed);
	}

	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		auto up = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirUp, relativeCameraState.currentRotationTransformation);
		relativeCameraState.targetPositionTransformation = relativeCameraState.targetPositionTransformation - (up * cameraTranslationSpeed * -1.f);
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




	// Yaw: rotation around WORLD up
	if (*cachedAnalogTurnLeftRight != 0.f)
	{
		needToApplyRotation = true;

		
		//auto quatYaw = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirUp, analogRotationSpeed * *cachedAnalogTurnLeftRight);

		////https://gamedev.stackexchange.com/questions/136174/im-rotating-an-object-on-two-axes-so-why-does-it-keep-twisting-around-the-thir
		//relativeCameraState.targetRotationTransformation = quatYaw * relativeCameraState.targetRotationTransformation;

		relativeCameraState.eulerYaw = (relativeCameraState.eulerYaw + (analogRotationSpeed * *cachedAnalogTurnLeftRight));
		relativeCameraState.eulerYaw = std::remainderf(relativeCameraState.eulerYaw, DirectX::XM_2PI);
	}



	// Pitch: rotation around local right
	if (*cachedAnalogTurnUpDown != 0.f)
	{
		needToApplyRotation = true;


		//auto quatPitch = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.targetLookDirRight, analogRotationSpeed * *cachedAnalogTurnUpDown);


		////https://gamedev.stackexchange.com/questions/136174/im-rotating-an-object-on-two-axes-so-why-does-it-keep-twisting-around-the-thir
		//relativeCameraState.targetRotationTransformation =  relativeCameraState.targetRotationTransformation * quatPitch;

		constexpr float maxPitch = DirectX::XM_PIDIV2 - 0.01;
		constexpr float minPitch = maxPitch * -1.f;

		relativeCameraState.eulerPitch = (relativeCameraState.eulerPitch + (analogRotationSpeed * *cachedAnalogTurnUpDown * -1.f));
		relativeCameraState.eulerPitch = std::clamp(relativeCameraState.eulerPitch, minPitch, maxPitch);
	}

	// Roll: means rotating around local FORWARD axis.
	if (GetKeyState('G') & 0x8000)
	{
		needToApplyRotation = true;
		
		//auto quatRoll = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.targetLookDirForward, digitalRotationSpeed);

		////https://gamedev.stackexchange.com/questions/136174/im-rotating-an-object-on-two-axes-so-why-does-it-keep-twisting-around-the-thir
		//relativeCameraState.targetRotationTransformation = relativeCameraState.targetRotationTransformation * quatRoll;

		relativeCameraState.eulerRoll = relativeCameraState.eulerRoll + digitalRotationSpeed;
		relativeCameraState.eulerRoll = std::remainderf(relativeCameraState.eulerRoll, DirectX::XM_2PI);

	}

	if (GetKeyState('T') & 0x8000)
	{
		needToApplyRotation = true;

		//auto quatRoll = SimpleMath::Quaternion::CreateFromAxisAngle(relativeCameraState.targetLookDirForward, digitalRotationSpeed * -1.f);

		////https://gamedev.stackexchange.com/questions/136174/im-rotating-an-object-on-two-axes-so-why-does-it-keep-twisting-around-the-thir
		//relativeCameraState.targetRotationTransformation = relativeCameraState.targetRotationTransformation * quatRoll;

		relativeCameraState.eulerRoll = relativeCameraState.eulerRoll + (digitalRotationSpeed * -1.f);
		relativeCameraState.eulerRoll = std::remainderf(relativeCameraState.eulerRoll, DirectX::XM_2PI);
	}


	if (needToApplyRotation)
	{
		//relativeCameraState.targetRotationTransformation = relativeCameraState.targetRotationTransformation *  quatYaw  * quatPitch;
		//relativeCameraState.targetRotationTransformation.Normalize();


		// todo: try instead of having one quat, make a quat out of each euler (axisangle?) then apply

		auto quatYaw = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirUp, relativeCameraState.eulerYaw);
		auto quatPitch = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirRight, relativeCameraState.eulerPitch);

		auto quatRoll = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirForward, relativeCameraState.eulerRoll);


		//auto quatYaw = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirUp, relativeCameraState.eulerYaw);

		//auto right = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirRight, quatYaw);

		//auto quatPitch = SimpleMath::Quaternion::CreateFromAxisAngle(right, relativeCameraState.eulerPitch);

		//auto forward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, quatPitch * quatYaw);
		//auto quatRoll = SimpleMath::Quaternion::CreateFromAxisAngle(forward, relativeCameraState.eulerRoll);


		// i haven't quite got rolling working properly...
		relativeCameraState.targetRotationTransformation =  quatPitch * quatRoll * quatYaw; // order matters
		//  quatPitch * quatRoll * quatYaw ; mostly works but roll is kinda broken

		//SimpleMath::Vector3 forward = SimpleMath::Vector3(
		//	std::cos(relativeCameraState.eulerPitch) * std::sin(relativeCameraState.eulerYaw),
		//	std::sin(relativeCameraState.eulerPitch),
		//	std::cos(relativeCameraState.eulerPitch) * std::cos(relativeCameraState.eulerYaw));

		//relativeCameraState.targetRotationTransformation = SimpleMath::Quaternion::LookRotation(forward, { 0, 1, 0 });



		//relativeCameraState.targetRotationTransformation = SimpleMath::Quaternion::CreateFromYawPitchRoll(
		//	relativeCameraState.eulerPitch, 
		//	relativeCameraState.eulerRoll,
		//	relativeCameraState.eulerYaw
		//);

		// recalculate lookdirs for next input
		/*relativeCameraState.targetLookDirForward = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitX, relativeCameraState.currentRotationTransformation);
		relativeCameraState.targetLookDirForward.Normalize();
		relativeCameraState.targetLookDirUp = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitZ, relativeCameraState.currentRotationTransformation);
		relativeCameraState.targetLookDirUp.Normalize();
		relativeCameraState.targetLookDirRight = SimpleMath::Vector3::Transform(SimpleMath::Vector3::UnitY, relativeCameraState.currentRotationTransformation);
		relativeCameraState.targetLookDirRight.Normalize();*/

		//relativeCameraState.targetRotationTransformation = SimpleMath::Quaternion::CreateFromYawPitchRoll(relativeCameraState.eulerYaw, relativeCameraState.eulerPitch, relativeCameraState.eulerRoll);
		//relativeCameraState.targetRotationTransformation.Conjugate();
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