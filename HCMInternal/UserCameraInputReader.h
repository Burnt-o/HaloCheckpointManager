#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IUpdateCameraTransform.h"


class IUserCameraInputReaderImpl : public IUpdatePositionTransform, public IUpdateRotationTransform, public IUpdateFOVTransform
{
public:
	virtual ~IUserCameraInputReaderImpl() = default;
};

class UserCameraInputReader : public IOptionalCheat, public IUpdatePositionTransform, public IUpdateRotationTransform, public IUpdateFOVTransform
{
private:
	std::unique_ptr< IUserCameraInputReaderImpl> pimpl; // todo; working on hooking this up to multilevel pointer to read analog inputs


	virtual void updateFOVTransform(const FreeCameraData& freeCameraData, const float frameDelta, float& fov) override;
	virtual void updateRotationTransform(const FreeCameraData& freeCameraData, const float frameDelta, float& eulerYaw, float& eulerPitch, float& eulerRoll) override;
	virtual void updatePositionTransform(const FreeCameraData& freeCameraData, const float frameDelta, SimpleMath::Vector3& positionTransform) override;






public:

	UserCameraInputReader(GameState game, IDIContainer& dicon);
	~UserCameraInputReader();


	std::string_view getName() override { return nameof(UserCameraInputReader); }
};

