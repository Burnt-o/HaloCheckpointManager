#pragma once
#include "pch.h"
#include "FreeCameraData.h"
#include "ISmoother.h"
#include "IUpdateCameraTransform.h"


class RotationTransformer
{
private:

	void applyRotationTransform(FreeCameraData& freeCameraData, float frameDelta)
	{

		// interpolate euler angles
		rotationSmoother->smooth(currentEulerYaw, targetEulerYaw);
		rotationSmoother->smooth(currentEulerPitch, targetEulerPitch);
		rotationSmoother->smooth(currentEulerRoll, targetEulerRoll);



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


	std::unique_ptr<ISmoother<float>> rotationSmoother;
	std::shared_ptr<IUpdateRotationTransform> rotationUpdater;

public:




	RotationTransformer(std::unique_ptr<ISmoother<float>> rotSmooth, std::shared_ptr<IUpdateRotationTransform> rotUp, float initialYaw, float initialPitch, float initialRoll)
		: 
		rotationSmoother(std::move(rotSmooth)), 
		rotationUpdater(std::move(rotUp)),
		currentEulerYaw(initialYaw),
		currentEulerPitch(initialPitch),
		currentEulerRoll(initialRoll),
		targetEulerYaw(initialYaw),
		targetEulerPitch(initialPitch),
		targetEulerRoll(initialRoll)

	{}

	
	

	void transformRotation(FreeCameraData& freeCameraData, float frameDelta)
	{
		rotationUpdater->updateRotationTransform(freeCameraData, frameDelta, targetEulerYaw, targetEulerPitch, targetEulerRoll);
		applyRotationTransform(freeCameraData, frameDelta);
	}



};

class PositionTransformer
{
private:




	void applyPositionTransform(FreeCameraData& freeCameraData, float frameDelta)
	{


		positionSmoother->smooth(currentPositionTransformation, targetPositionTransformation);

		freeCameraData.currentPosition = freeCameraData.currentPosition + currentPositionTransformation;
	}

	SimpleMath::Vector3 currentPositionTransformation;

	SimpleMath::Vector3 targetPositionTransformation;

	std::unique_ptr<ISmoother<SimpleMath::Vector3>> positionSmoother;
	std::shared_ptr<IUpdatePositionTransform> positionUpdater;



public:



	PositionTransformer(std::unique_ptr<ISmoother<SimpleMath::Vector3>> posSmooth, std::shared_ptr<IUpdatePositionTransform> posUp, SimpleMath::Vector3 initialPosition) 
		:
		positionSmoother(std::move(posSmooth)),
		positionUpdater(std::move(posUp)),
		currentPositionTransformation(initialPosition),
		targetPositionTransformation(initialPosition)
	{}



	void transformPosition(FreeCameraData& freeCameraData, float frameDelta)
	{
		positionUpdater->updatePositionTransform(freeCameraData, frameDelta, targetPositionTransformation);
		applyPositionTransform(freeCameraData, frameDelta);
	}


};

class FOVTransformer
{
private:


	void applyFOVTransform(FreeCameraData& freeCameraData, float frameDelta)
	{

		// TODO

		//fovSmoother->smooth(currentFOVOffset, targetFOVOffset);

		//currentFOVOffset = currentFOVOffset + currentFOVOffset;
	}



	float currentFOVScale;

	float targetFOVScale;

	std::unique_ptr<ISmoother<float>> fovSmoother;
	std::shared_ptr<IUpdateFOVTransform> fovUpdater;

public:




	FOVTransformer(std::unique_ptr<ISmoother<float>> fovSmooth, std::shared_ptr<IUpdateFOVTransform> fovUp, float initialFOVScale)
		:
		fovSmoother(std::move(fovSmooth)),
		fovUpdater(std::move(fovUp)),
		currentFOVScale(initialFOVScale),
		targetFOVScale(initialFOVScale)
	{}

	void transformFOV(FreeCameraData& freeCameraData, float frameDelta)
	{
		fovUpdater->updateFOVTransform(freeCameraData, frameDelta, targetFOVScale);
		applyFOVTransform(freeCameraData, frameDelta);
	}


};