#pragma once
#include "pch.h"
#include "FreeCameraData.h"
#include "NullSmoother.h"
#include "LinearSmoother.h"
#include "IUpdateCameraTransform.h"


class RotationTransformer
{
private:
	ScopedCallback< eventpp::CallbackList<void(int&)>> currentInterpolationTypeChangedCallback;
	ScopedCallback< eventpp::CallbackList<void(float&)>> currentLinearInterpolationFactorChangedCallback;

	void applyRotationTransform(FreeCameraData& freeCameraData, float frameDelta)
	{

		// interpolate euler angles
		pCurrentRotationSmoother->smooth(currentEulerYaw, targetEulerYaw);
		pCurrentRotationSmoother->smooth(currentEulerPitch, targetEulerPitch);
		pCurrentRotationSmoother->smooth(currentEulerRoll, targetEulerRoll);



		// step one: yaw
		auto quatYaw = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirUp, currentEulerYaw);

		auto totalQuat = quatYaw;

		// transform currentLookDirs by rotations
		freeCameraData.currentlookDirForward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, totalQuat);
		freeCameraData.currentlookDirForward.Normalize();
		freeCameraData.currentlookDirUp = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirUp, totalQuat);
		freeCameraData.currentlookDirUp.Normalize();
		freeCameraData.currentlookDirRight = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirRight, totalQuat);
		freeCameraData.currentlookDirRight.Normalize();

		// step two: pitch
		auto quatPitch = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirRight, currentEulerPitch);

		totalQuat = quatPitch;

		// transform currentLookDirs by rotations
		freeCameraData.currentlookDirForward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, totalQuat);
		freeCameraData.currentlookDirForward.Normalize();
		freeCameraData.currentlookDirUp = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirUp, totalQuat);
		freeCameraData.currentlookDirUp.Normalize();
		freeCameraData.currentlookDirRight = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirRight, totalQuat);
		freeCameraData.currentlookDirRight.Normalize();


		// step 3: roll
		auto quatRoll = SimpleMath::Quaternion::CreateFromAxisAngle(freeCameraData.currentlookDirForward, currentEulerRoll);

		totalQuat = quatRoll;

		// transform currentLookDirs by rotations
		freeCameraData.currentlookDirForward = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirForward, totalQuat);
		freeCameraData.currentlookDirForward.Normalize();
		freeCameraData.currentlookDirUp = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirUp, totalQuat);
		freeCameraData.currentlookDirUp.Normalize();
		freeCameraData.currentlookDirRight = SimpleMath::Vector3::Transform(freeCameraData.currentlookDirRight, totalQuat);
		freeCameraData.currentlookDirRight.Normalize();

	}
	float currentEulerYaw;
	float currentEulerPitch;
	float currentEulerRoll;



	float targetEulerYaw;
	float targetEulerPitch;
	float targetEulerRoll;

	NullSmoother<float> nullRotationSmoother;
	LinearSmoother<float> linearRotationSmoother;
	ISmoother<float>* pCurrentRotationSmoother;

	std::shared_ptr<IUpdateRotationTransform> rotationUpdater;

	std::atomic_bool dataInUse = false;

	void onInterpolationTypeChanged(int& newType)
	{
		ScopedAtomicBool lock(dataInUse);
		switch (newType)
		{
		case (int)SettingsStateAndEvents::FreeCameraInterpolationTypesEnum::None:
			pCurrentRotationSmoother = &nullRotationSmoother;
			break;

		case (int)SettingsStateAndEvents::FreeCameraInterpolationTypesEnum::Linear:
			pCurrentRotationSmoother = &linearRotationSmoother;
			break;

		default: PLOG_ERROR << "Invalid smoother enum value!" << newType;
		}
	}

	void onLinearInterpolationFactorChanged(float& newValue)
	{
		ScopedAtomicBool lock(dataInUse);
		linearRotationSmoother.setSmoothRate(newValue);
	}

public:

	const SimpleMath::Vector3 getRotationTransformation()
	{
		ScopedAtomicBool lock(dataInUse);
		return SimpleMath::Vector3(currentEulerYaw, currentEulerPitch, currentEulerRoll);
	}

	void setRotationTransformation(float yaw, float pitch, float roll)
	{
		ScopedAtomicBool lock(dataInUse);
		currentEulerYaw = yaw;
		targetEulerYaw = yaw;
		currentEulerPitch = pitch;
		targetEulerPitch = pitch;
		currentEulerRoll = roll;
		targetEulerRoll = roll;
	}

	void setRotationTransformation(SimpleMath::Vector3 rotvec)
	{
		ScopedAtomicBool lock(dataInUse);
		currentEulerYaw = rotvec.x;
		targetEulerYaw = rotvec.x;
		currentEulerPitch = rotvec.y;
		targetEulerPitch = rotvec.y;
		currentEulerRoll = rotvec.z;
		targetEulerRoll = rotvec.z;
	}


	RotationTransformer(std::shared_ptr<IUpdateRotationTransform> rotUpate, std::shared_ptr<BinarySetting<int>> currentInterpolationType, std::shared_ptr<BinarySetting<float>> currentLinearInterpolationFactor)
		: 
		rotationUpdater(std::move(rotUpate)),
		currentInterpolationTypeChangedCallback(currentInterpolationType->valueChangedEvent, [this](int& n) { onInterpolationTypeChanged(n); }),
		currentLinearInterpolationFactorChangedCallback(currentLinearInterpolationFactor->valueChangedEvent, [this](float& n) { onLinearInterpolationFactorChanged(n); }),
		linearRotationSmoother(currentLinearInterpolationFactor->GetValue())
	{
		int curInterpolationType = currentInterpolationType->GetValue();
		switch (curInterpolationType)
		{
		case (int)SettingsStateAndEvents::FreeCameraInterpolationTypesEnum::None:
			pCurrentRotationSmoother = &nullRotationSmoother;
			break;

		case (int)SettingsStateAndEvents::FreeCameraInterpolationTypesEnum::Linear:
			pCurrentRotationSmoother = &linearRotationSmoother;
			break;

		default:  throw HCMInitException(std::format("Invalid smoother enum value! {}", curInterpolationType));
		}
	}

	
	

	void transformRotation(FreeCameraData& freeCameraData, float frameDelta)
	{
		ScopedAtomicBool lock(dataInUse);
		rotationUpdater->updateRotationTransform(freeCameraData, frameDelta, targetEulerYaw, targetEulerPitch, targetEulerRoll);
		applyRotationTransform(freeCameraData, frameDelta);
	}



};

class PositionTransformer
{
private:

	ScopedCallback< eventpp::CallbackList<void(int&)>> currentInterpolationTypeChangedCallback;
	ScopedCallback< eventpp::CallbackList<void(float&)>> currentLinearInterpolationFactorChangedCallback;

	void applyPositionTransform(FreeCameraData& freeCameraData, float frameDelta)
	{


		pCurrentPositionSmoother->smooth(currentPositionTransformation, targetPositionTransformation);

		freeCameraData.currentPosition = freeCameraData.currentPosition + currentPositionTransformation;
	}



	NullSmoother<SimpleMath::Vector3> nullPositionSmoother;
	LinearSmoother<SimpleMath::Vector3> linearPositionSmoother;
	ISmoother<SimpleMath::Vector3> * pCurrentPositionSmoother;
	std::shared_ptr<IUpdatePositionTransform> positionUpdater;

	SimpleMath::Vector3 currentPositionTransformation;
	SimpleMath::Vector3 targetPositionTransformation;

	std::atomic_bool dataInUse = false;

	void onInterpolationTypeChanged(int& newType)
	{
		ScopedAtomicBool lock(dataInUse);
		switch (newType)
		{
		case (int)SettingsStateAndEvents::FreeCameraInterpolationTypesEnum::None:
			pCurrentPositionSmoother = &nullPositionSmoother;
			break;

		case (int)SettingsStateAndEvents::FreeCameraInterpolationTypesEnum::Linear:
			pCurrentPositionSmoother = &linearPositionSmoother;
			break;

		default: PLOG_ERROR << "Invalid smoother enum value!" << newType;
		}
	}

	void onLinearInterpolationFactorChanged(float& newValue)
	{
		ScopedAtomicBool lock(dataInUse);
		linearPositionSmoother.setSmoothRate(newValue);
	}

public:

	const SimpleMath::Vector3 getPositionTransformation()
	{
		ScopedAtomicBool lock(dataInUse);
		return currentPositionTransformation;
	}

	void setPositionTransformation(SimpleMath::Vector3 newPos)
	{
		ScopedAtomicBool lock(dataInUse);
		currentPositionTransformation = newPos;
		targetPositionTransformation = newPos;
	}


	PositionTransformer (std::shared_ptr<IUpdatePositionTransform> posUpdate, std::shared_ptr<BinarySetting<int>> currentInterpolationType, std::shared_ptr<BinarySetting<float>> currentLinearInterpolationFactor)
		: positionUpdater(std::move(posUpdate)),
		currentInterpolationTypeChangedCallback(currentInterpolationType->valueChangedEvent, [this](int& n) { onInterpolationTypeChanged(n); }),
		currentLinearInterpolationFactorChangedCallback(currentLinearInterpolationFactor->valueChangedEvent, [this](float& n) { onLinearInterpolationFactorChanged(n); }),
		linearPositionSmoother(currentLinearInterpolationFactor->GetValue())
	{
		PLOG_DEBUG << "constructing PositionTransformer";

		int curInterpolationType = currentInterpolationType->GetValue();
		switch (curInterpolationType)
		{
		case (int)SettingsStateAndEvents::FreeCameraInterpolationTypesEnum::None:
			pCurrentPositionSmoother = &nullPositionSmoother;
			break;

		case (int)SettingsStateAndEvents::FreeCameraInterpolationTypesEnum::Linear:
			pCurrentPositionSmoother = &linearPositionSmoother;
			break;

		default:  throw HCMInitException(std::format("Invalid smoother enum value! {}", curInterpolationType));
		}

	}



	void transformPosition(FreeCameraData& freeCameraData, float frameDelta)
	{
		ScopedAtomicBool lock(dataInUse);
		positionUpdater->updatePositionTransform(freeCameraData, frameDelta, targetPositionTransformation);
		applyPositionTransform(freeCameraData, frameDelta);
	}


};

class FOVTransformer
{
private:
	ScopedCallback< eventpp::CallbackList<void(int&)>> currentInterpolationTypeChangedCallback;
	ScopedCallback< eventpp::CallbackList<void(float&)>> currentLinearInterpolationFactorChangedCallback;


	void applyFOVTransform(FreeCameraData& freeCameraData, float frameDelta, float& targetFOV)
	{

		pCurrentFOVSmoother->smooth(currentFOV, targetFOV);
	}



	float currentFOV = 90;



	NullSmoother<float> nullFOVSmoother;
	LinearSmoother<float> linearFOVSmoother;
	ISmoother<float>* pCurrentFOVSmoother;
	std::shared_ptr<IUpdateFOVTransform> fovUpdater;

	std::atomic_bool dataInUse = false;

	void onInterpolationTypeChanged(int& newType)
	{
		ScopedAtomicBool lock(dataInUse);
		switch (newType)
		{
		case (int)SettingsStateAndEvents::FreeCameraInterpolationTypesEnum::None:
			pCurrentFOVSmoother = &nullFOVSmoother;
			break;

		case (int)SettingsStateAndEvents::FreeCameraInterpolationTypesEnum::Linear:
			pCurrentFOVSmoother = &linearFOVSmoother;
			break;

		default: PLOG_ERROR << "Invalid smoother enum value!" << newType;
		}
	}

	void onLinearInterpolationFactorChanged(float& newValue)
	{
		ScopedAtomicBool lock(dataInUse);
		linearFOVSmoother.setSmoothRate(newValue);
	}

public:


	void setFOV(float newValue)
	{
		ScopedAtomicBool lock(dataInUse);
		currentFOV = newValue;
	}

	FOVTransformer(std::shared_ptr<IUpdateFOVTransform> fovUpate, std::shared_ptr<BinarySetting<int>> currentInterpolationType, std::shared_ptr<BinarySetting<float>> currentLinearInterpolationFactor)
		:
		fovUpdater(std::move(fovUpate)),
		currentInterpolationTypeChangedCallback(currentInterpolationType->valueChangedEvent, [this](int& n) { onInterpolationTypeChanged(n); }),
		currentLinearInterpolationFactorChangedCallback(currentLinearInterpolationFactor->valueChangedEvent, [this](float& n) { onLinearInterpolationFactorChanged(n); }),
		linearFOVSmoother(currentLinearInterpolationFactor->GetValue())
	{
		int curInterpolationType = currentInterpolationType->GetValue();
		switch (curInterpolationType)
		{
		case (int)SettingsStateAndEvents::FreeCameraInterpolationTypesEnum::None:
			pCurrentFOVSmoother = &nullFOVSmoother;
			break;

		case (int)SettingsStateAndEvents::FreeCameraInterpolationTypesEnum::Linear:
			pCurrentFOVSmoother = &linearFOVSmoother;
			break;

		default:  throw HCMInitException(std::format("Invalid smoother enum value! {}", curInterpolationType));
		}
	}

	void transformFOV(FreeCameraData& freeCameraData, float frameDelta, float& targetFOV)
	{
		ScopedAtomicBool lock(dataInUse);
		fovUpdater->updateFOVTransform(freeCameraData, frameDelta, targetFOV);
		applyFOVTransform(freeCameraData, frameDelta, targetFOV);
	}


};