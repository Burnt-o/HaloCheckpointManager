#pragma once
#include "FreeCameraData.h"

class IUpdatePositionTransform
{
public:
	virtual ~IUpdatePositionTransform() = default;

	virtual void updatePositionTransform(const FreeCameraData& freeCameraData, const float frameDelta, SimpleMath::Vector3& positionTransform) = 0;
};

class IUpdateRotationTransform
{
public:
	virtual ~IUpdateRotationTransform() = default;

	virtual void updateRotationTransform(const FreeCameraData& freeCameraData, const float frameDelta, float& eulerYaw, float& eulerPitch, float& eulerRoll) = 0;
};

class IUpdateFOVTransform
{
public:
	virtual ~IUpdateFOVTransform() = default;

	virtual void updateFOVTransform(const FreeCameraData& freeCameraData, const float frameDelta, float& fov) = 0;
};