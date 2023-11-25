#pragma once
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "CameraTransformer.h"

class CameraInputReader : public IOptionalCheat
{
private:
	class CameraInputReaderImpl;
	std::unique_ptr< CameraInputReaderImpl> pimpl; // todo; working on hooking this up to multilevel pointer to read analog inputs
public:

	CameraInputReader(GameState game, IDIContainer& dicon);
	~CameraInputReader();

	void readPositionInput(RelativeCameraState& relativeCameraState, FreeCameraData& freeCameraData, float frameDelta);

	void readRotationInput(RelativeCameraState& relativeCameraState, FreeCameraData& freeCameraData, float frameDelta);

	void readFOVInput(RelativeCameraState& relativeCameraState, FreeCameraData& freeCameraData, float frameDelta);

	std::string_view getName() override { return nameof(CameraInputReader); }
};

