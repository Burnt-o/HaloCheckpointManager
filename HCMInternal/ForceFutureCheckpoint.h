#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"

// Forces a checkpoint but at a specific tick in the future chosen by the user.
class ForceFutureCheckpoint : public IOptionalCheat
{
private:
	class ForceFutureCheckpointImpl;
	std::unique_ptr<ForceFutureCheckpointImpl> pimpl;

public:
	ForceFutureCheckpoint(GameState gameImpl, IDIContainer& dicon);
	~ForceFutureCheckpoint();

	virtual std::string_view getName() override { return nameof(ForceFutureCheckpoint); }
};

